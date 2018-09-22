#include "rc_rpc_plugin/rc_pending_resumption_handler.h"
#include "rc_rpc_plugin/rc_app_extension.h"
#include "application_manager/message_helper.h"
#include "application_manager/resumption/resumption_data_processor.h"
#include "smart_objects/smart_object.h"
#include "utils/helpers.h"

#include <vector>

namespace rc_rpc_plugin {

namespace app_mngr = application_manager;

CREATE_LOGGERPTR_GLOBAL(logger_, "RCPendingResumptionHandler")

static const char* module_type_key = "moduleType";
static const char* intended_action_key = "subscribe";

RCPendingResumptionHandler::ResumptionAwaitingHandling::
    ResumptionAwaitingHandling(const uint32_t app_id,
                               app_mngr::AppExtension& ext,
                               resumption::Subscriber sub)
    : extension(ext), application_id(app_id), subscriber(sub) {
  RCAppExtension& rc_app_extension = dynamic_cast<RCAppExtension&>(extension);
  std::set<std::string> subscriptions = rc_app_extension.Subscriptions();

  for (const auto& subscription : subscriptions) {
    handled_subscriptions[subscription] = false;
  }
}

RCPendingResumptionHandler::RCPendingResumptionHandler(
    app_mngr::ApplicationManager& application_manager)
    : ExtensionPendingResumptionHandler(application_manager) {}

void RCPendingResumptionHandler::on_event(
    const app_mngr::event_engine::Event& event) {
  using namespace application_manager;
  LOG4CXX_AUTO_TRACE(logger_);

  const smart_objects::SmartObject& response = event.smart_object();
  const uint32_t correlation_id = event.smart_object_correlation_id();

  if (pending_subscription_requests_.find(correlation_id) ==
      pending_subscription_requests_.end()) {
    LOG4CXX_WARN(logger_,
                 "pending subscription request for correlation id"
                     << correlation_id << " not found");
    return;
  }

  smart_objects::SmartObject pending_subscription_request =
      pending_subscription_requests_[correlation_id];

  if (frozen_resumptions_.empty()) {
    LOG4CXX_DEBUG(logger_, "There are no frozen resumptions");
    return;
  }

  const hmi_apis::Common_Result::eType result_code =
      static_cast<hmi_apis::Common_Result::eType>(
          response[strings::params][application_manager::hmi_response::code]
              .asInt());
  const bool is_subscription_successful =
      (result_code == hmi_apis::Common_Result::SUCCESS ||
       result_code == hmi_apis::Common_Result::WARNINGS);

  const std::string subscription_module_type =
      pending_subscription_request[strings::msg_params][module_type_key]
          .asString();

  if (is_subscription_successful) {
    LOG4CXX_DEBUG(logger_,
                  "subscription for " << subscription_module_type
                                      << " successful");
    for (auto& frozen_resumption : frozen_resumptions_) {
      if (frozen_resumption.handled_subscriptions.find(
              subscription_module_type) !=
          frozen_resumption.handled_subscriptions.end()) {
        frozen_resumption.handled_subscriptions[subscription_module_type];
      }
    }
    pending_subscription_requests_.erase(correlation_id);
  } else {
    pending_subscription_requests_.clear();
    ResumptionAwaitingHandling freezed_resumption = frozen_resumptions_.front();
    frozen_resumptions_.pop_front();
    std::set<std::string> unhandled_subscriptions =
        GetFrozenResumptionUnhandledSubscriptions(freezed_resumption);
    ProcessSubscriptionRequests(CreateSubscriptionRequests(
        unhandled_subscriptions, freezed_resumption.application_id));
  }
}

void RCPendingResumptionHandler::HandleResumptionSubscriptionRequest(
    app_mngr::AppExtension& extension,
    resumption::Subscriber& subscriber,
    app_mngr::Application& app) {
  LOG4CXX_AUTO_TRACE(logger_);
  // TODO create Subscriptions method in AppExtension
  RCAppExtension& rc_app_extension = dynamic_cast<RCAppExtension&>(extension);
  std::set<std::string> subscriptions = rc_app_extension.Subscriptions();

  if (pending_subscription_requests_.empty()) {
    smart_objects::SmartObjectList subscription_requests =
        CreateSubscriptionRequests(subscriptions, app.app_id());
    ProcessSubscriptionRequests(subscription_requests);
  } else {
    ResumptionAwaitingHandling frozen_resumption(
        app.app_id(), rc_app_extension, subscriber);
    frozen_resumptions_.push_back(frozen_resumption);
  }
}

void RCPendingResumptionHandler::ClearPendingResumptionRequests() {
  LOG4CXX_AUTO_TRACE(logger_);
  using namespace application_manager;

  const hmi_apis::FunctionID::eType timed_out_pending_request_fid =
      static_cast<hmi_apis::FunctionID::eType>(
          pending_subscription_requests_.begin()
              ->second[strings::params][strings::function_id]
              .asInt());
  unsubscribe_from_event(timed_out_pending_request_fid);
  pending_subscription_requests_.clear();

  if (!frozen_resumptions_.empty()) {
    ResumptionAwaitingHandling freezed_resumption = frozen_resumptions_.front();
    frozen_resumptions_.pop_front();

    std::set<std::string> unhandled_subscriptions =
        GetFrozenResumptionUnhandledSubscriptions(freezed_resumption);
    ProcessSubscriptionRequests(CreateSubscriptionRequests(
        unhandled_subscriptions, freezed_resumption.application_id));
  }
}

smart_objects::SmartObjectList
RCPendingResumptionHandler::CreateSubscriptionRequests(
    const std::set<std::string> subscriptions, const uint32_t application_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  using namespace application_manager;

  smart_objects::SmartObjectList subscription_requests;

  smart_objects::SmartObject msg_params =
      smart_objects::SmartObject(smart_objects::SmartType_Map);

  msg_params[strings::app_id] = application_id;

  for (auto& module_type : subscriptions) {
    msg_params[module_type_key] = module_type;
    msg_params[intended_action_key] = true;

    smart_objects::SmartObjectSPtr request =
         application_manager::MessageHelper::CreateModuleInfoSO(
             hmi_apis::FunctionID::RC_GetInteriorVehicleData,
             application_manager_);

    smart_objects::SmartObject& object = *request;
    object[strings::params][strings::message_type] = static_cast<int>(kRequest);
    object[strings::params][strings::function_id] =
        static_cast<int>(hmi_apis::FunctionID::RC_GetInteriorVehicleData);
    object[strings::params][strings::correlation_id] =
        application_manager_.GetNextHMICorrelationID();
    object[strings::msg_params] =
        smart_objects::SmartObject(smart_objects::SmartType_Map);

    (*request)[strings::msg_params] = msg_params;
    subscription_requests.push_back(request);
  }

  return subscription_requests;
}

void RCPendingResumptionHandler::ProcessSubscriptionRequests(
    const smart_objects::SmartObjectList& subscription_requests) {
  LOG4CXX_AUTO_TRACE(logger_);
  using namespace application_manager;
  using namespace resumption;

  for (const auto& subscription_request : subscription_requests) {
    auto it = pending_subscription_requests_.end();
    std::find_if(
        pending_subscription_requests_.begin(),
        pending_subscription_requests_.end(),
        [&subscription_request](
            const std::pair<uint32_t, smart_objects::SmartObject>& item) {
          return (*subscription_request)[strings::msg_params][module_type_key]
                     .asString() ==
                 item.second[strings::msg_params][module_type_key].asString();
        });
    if (it == pending_subscription_requests_.end()) {
      const uint32_t correlation_id =
          (*subscription_request)[strings::params][strings::correlation_id]
              .asUInt();
      pending_subscription_requests_[correlation_id] = *subscription_request;
      const auto function_id = static_cast<hmi_apis::FunctionID::eType>(
          (*subscription_request)[application_manager::strings::params]
                                 [application_manager::strings::function_id]
                                     .asInt());
      auto resumption_request =
          ExtensionPendingResumptionHandler::MakeResumptionRequest(
              correlation_id, function_id, *subscription_request);

      subscribe_on_event(function_id, correlation_id);
      // TODO
      // subscriber(app.app_id(), resumption_request);

      application_manager_.GetRPCService().ManageHMICommand(
          subscription_request);
    }
  }
}

std::set<std::string>
RCPendingResumptionHandler::GetFrozenResumptionUnhandledSubscriptions(
    const ResumptionAwaitingHandling& frozen_resumption) {
  LOG4CXX_AUTO_TRACE(logger_);
  std::set<std::string> unhandled_subscriptions;

  for (const auto& subscription : frozen_resumption.handled_subscriptions) {
    if (!subscription.second) {
      unhandled_subscriptions.insert(subscription.first);
    }
  }

  return unhandled_subscriptions;
}

}  // namespace rc_rpc_plugin

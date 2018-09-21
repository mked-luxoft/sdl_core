#ifndef SRC_COMPONENTS_APPLICATION_MANAGER_RPC_PLUGINS_RC_RPC_PLUGIN_INCLUDE_RC_PENDING_RESUMPTION_HANDLER_H
#define SRC_COMPONENTS_APPLICATION_MANAGER_RPC_PLUGINS_RC_RPC_PLUGIN_INCLUDE_RC_PENDING_RESUMPTION_HANDLER_H

#include "application_manager/resumption/extension_pending_resumption_handler.h"

namespace rc_rpc_plugin {

namespace app_mngr = application_manager;

class RCPendingResumptionHandler
    : public resumption::ExtensionPendingResumptionHandler {
 public:
  RCPendingResumptionHandler(app_mngr::ApplicationManager& application_manager);

  ~RCPendingResumptionHandler() {}

  void on_event(const app_mngr::event_engine::Event& event) OVERRIDE;

  void HandleResumptionSubscriptionRequest(app_mngr::AppExtension& extension,
                                           resumption::Subscriber& subscriber,
                                           app_mngr::Application& app) OVERRIDE;

  void ClearPendingResumptionRequests() OVERRIDE;

 private:
  smart_objects::SmartObjectList CreateSubscriptionRequests(
      const std::set<std::string> subscriptions, const uint32_t application_id);

  void ProcessSubscriptionRequests(
      const smart_objects::SmartObjectList& subscription_requests);

  struct ResumptionAwaitingHandling {
    app_mngr::AppExtension& extension;
    const uint32_t application_id;
    resumption::Subscriber subscriber;
    std::map<std::string, bool> handled_subscriptions;

    ResumptionAwaitingHandling(const uint32_t app_id,
                               app_mngr::AppExtension& ext,
                               resumption::Subscriber sub);
  };

  std::set<std::string> GetFrozenResumptionUnhandledSubscriptions(
      const ResumptionAwaitingHandling& frozen_resumption);

  std::map<uint32_t, smart_objects::SmartObject> pending_subscription_requests_;
  std::deque<ResumptionAwaitingHandling> frozen_resumptions_;
};

}  // namespace rc_rpc_plugin

#endif  // SRC_COMPONENTS_APPLICATION_MANAGER_RPC_PLUGINS_RC_RPC_PLUGIN_INCLUDE_RC_PENDING_RESUMPTION_HANDLER_H

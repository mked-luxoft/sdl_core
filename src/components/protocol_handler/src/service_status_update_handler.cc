#include "protocol_handler/service_status_update_handler.h"
#include "interfaces/HMI_API.h"

namespace protocol_handler {

hmi_apis::Common_ServiceType::eType GetHMIServiceType(
    protocol_handler::ServiceType service_type) {
  using namespace hmi_apis;
  using namespace protocol_handler;
  switch (service_type) {
    case SERVICE_TYPE_RPC: {
      return Common_ServiceType::RPC;
    }
    case SERVICE_TYPE_AUDIO: {
      return Common_ServiceType::AUDIO;
    }
    case SERVICE_TYPE_NAVI: {
      return Common_ServiceType::VIDEO;
    }
    default: { return Common_ServiceType::INVALID_ENUM; }
  }
}

void ServiceStatusUpdateHandler::OnPTUFailed(
    protocol_handler::ServiceType service_type) {
  const auto hmi_service_type = GetHMIServiceType(service_type);
  const auto service_event = hmi_apis::Common_ServiceEvent::REQUEST_REJECTED;
  const auto service_update = hmi_apis::Common_ServiceUpdateReason::PTU_FAILED;

  for (const auto listener : listeners_) {
    listener->ProcessFailedStatusUpdate(
        hmi_service_type, service_event, service_update);
  }
}

void ServiceStatusUpdateHandler::OnGetSystemTimeExpired(
    protocol_handler::ServiceType service_type) {
  const auto hmi_service_type = GetHMIServiceType(service_type);
  const auto service_event = hmi_apis::Common_ServiceEvent::REQUEST_REJECTED;
  const auto service_update =
      hmi_apis::Common_ServiceUpdateReason::INVALID_TIME;

  for (const auto listener : listeners_) {
    listener->ProcessFailedStatusUpdate(
        hmi_service_type, service_event, service_update);
  }
}

void ServiceStatusUpdateHandler::OnCertInvalid(
    protocol_handler::ServiceType service_type) {
  const auto hmi_service_type = GetHMIServiceType(service_type);
  const auto service_event = hmi_apis::Common_ServiceEvent::REQUEST_REJECTED;
  const auto service_update =
      hmi_apis::Common_ServiceUpdateReason::INVALID_CERT;

  for (const auto listener : listeners_) {
    listener->ProcessFailedStatusUpdate(
        hmi_service_type, service_event, service_update);
  }
}

void ServiceStatusUpdateHandler::OnSuccessfulServiceUpdate(
    protocol_handler::ServiceType service_type, const bool accepted) {
  const auto hmi_service_type = GetHMIServiceType(service_type);
  const auto service_event =
      accepted ? hmi_apis::Common_ServiceEvent::REQUEST_ACCEPTED
               : hmi_apis::Common_ServiceEvent::REQUEST_RECEIVED;

  for (const auto listener : listeners_) {
    listener->ProcessSuccessfulStatusUpdate(hmi_service_type, service_event);
  }
}

void ServiceStatusUpdateHandler::AddListener(
    ServiceStatusUpdateHandlerListener* listener) {
  listeners_.push_back(listener);
}

}  // namespace protocol_handler
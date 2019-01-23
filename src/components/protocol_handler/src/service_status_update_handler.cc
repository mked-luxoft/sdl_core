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

hmi_apis::Common_ServiceUpdateReason::eType GetHMIServiceUpdateReason(
    protocol_handler::ServiceUpdateFailureReason update_reason) {
  using namespace hmi_apis;
  using namespace protocol_handler;
  switch (update_reason) {
    case ServiceUpdateFailureReason::PTU_FAILED: {
      return Common_ServiceUpdateReason::PTU_FAILED;
    }
    case ServiceUpdateFailureReason::CERT_INVALID: {
      return Common_ServiceUpdateReason::INVALID_CERT;
    }
    case ServiceUpdateFailureReason::INVALID_TIME: {
      return Common_ServiceUpdateReason::INVALID_TIME;
    }
    default: { return Common_ServiceUpdateReason::INVALID_ENUM; }
  }
}

void ServiceStatusUpdateHandler::OnSuccessfulServiceUpdate(
    const uint32_t session_id,
    protocol_handler::ServiceType service_type,
    const bool accepted) {
  const auto hmi_service_type = GetHMIServiceType(service_type);
  const auto service_event =
      accepted ? hmi_apis::Common_ServiceEvent::REQUEST_ACCEPTED
               : hmi_apis::Common_ServiceEvent::REQUEST_RECEIVED;

  for (const auto listener : listeners_) {
    listener->ProcessSuccessfulStatusUpdate(
        session_id, hmi_service_type, service_event);
  }
}

void ServiceStatusUpdateHandler::OnFailedServiceUpdate(
    const uint8_t session_id,
    const protocol_handler::ServiceType service_type,
    ServiceUpdateFailureReason update_reason) {
  const auto hmi_service_type = GetHMIServiceType(service_type);
  const auto service_event = hmi_apis::Common_ServiceEvent::REQUEST_REJECTED;
  const auto service_update_reason = GetHMIServiceUpdateReason(update_reason);

  for (const auto& listener : listeners_) {
    listener->ProcessFailedStatusUpdate(
        session_id, hmi_service_type, service_event, service_update_reason);
  }
}

void ServiceStatusUpdateHandler::AddListener(
    ServiceStatusUpdateHandlerListener* listener) {
  listeners_.push_back(listener);
}

}  // namespace protocol_handler

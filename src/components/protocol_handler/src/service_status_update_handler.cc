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

void ServiceStatusUpdateHandler::OnServiceUpdate(
    const uint32_t connection_key,
    const protocol_handler::ServiceType service_type,
    ServiceStatus service_status) {
  using namespace hmi_apis;
  auto hmi_service_type = GetHMIServiceType(service_type);
  Common_ServiceEvent::eType service_event;
  Common_ServiceUpdateReason::eType service_update_reason;

  switch (service_status) {
    case ServiceStatus::SERVICE_RECEIVED: {
      service_event = Common_ServiceEvent::REQUEST_RECEIVED;
      service_update_reason = Common_ServiceUpdateReason::INVALID_ENUM;
      break;
    }
    case ServiceStatus::SERVICE_ACCEPTED: {
      service_event = Common_ServiceEvent::REQUEST_ACCEPTED;
      service_update_reason = Common_ServiceUpdateReason::INVALID_ENUM;
      break;
    }
    case ServiceStatus::SERVICE_START_FAILED: {
      service_event = Common_ServiceEvent::REQUEST_REJECTED;
      service_update_reason = Common_ServiceUpdateReason::INVALID_ENUM;
      break;
    }
    case ServiceStatus::PTU_FAILED: {
      service_event = Common_ServiceEvent::REQUEST_REJECTED;
      service_update_reason = Common_ServiceUpdateReason::PTU_FAILED;
      break;
    }
    case ServiceStatus::CERT_INVALID: {
      service_event = Common_ServiceEvent::REQUEST_REJECTED;
      service_update_reason = Common_ServiceUpdateReason::INVALID_CERT;
      break;
    }
    case ServiceStatus::INVALID_TIME: {
      service_event = Common_ServiceEvent::REQUEST_REJECTED;
      service_update_reason = Common_ServiceUpdateReason::INVALID_TIME;
      break;
    }
    default: { return; }
  }

  for (const auto listener : listeners_) {
    listener->ProcessStatusUpdate(
        connection_key, hmi_service_type, service_event, service_update_reason);
  }
}

void ServiceStatusUpdateHandler::AddListener(
    ServiceStatusUpdateHandlerListener* listener) {
  listeners_.push_back(listener);
}

void ServiceStatusUpdateHandler::RemoveListener(
    ServiceStatusUpdateHandlerListener* listener) {
  if (!listener) {
    return;
  }
  listeners_.remove(listener);
}

}  // namespace protocol_handler

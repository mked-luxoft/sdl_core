#include "protocol_handler/service_status_update_handler.h"
#include "interfaces/HMI_API.h"

namespace protocol_handler {

CREATE_LOGGERPTR_GLOBAL(logger_, "ServiceStatusUpdateHandler")

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
  typedef utils::Optional<Common_ServiceUpdateReason::eType>
      UpdateReasonOptional;
  auto hmi_service_type = GetHMIServiceType(service_type);
  Common_ServiceEvent::eType service_event;
  UpdateReasonOptional service_update_reason =
      UpdateReasonOptional(UpdateReasonOptional::EMPTY);

  switch (service_status) {
    case ServiceStatus::SERVICE_RECEIVED: {
      service_event = Common_ServiceEvent::REQUEST_RECEIVED;
      break;
    }
    case ServiceStatus::SERVICE_ACCEPTED: {
      service_event = Common_ServiceEvent::REQUEST_ACCEPTED;
      break;
    }
    case ServiceStatus::SERVICE_START_FAILED: {
      service_event = Common_ServiceEvent::REQUEST_REJECTED;
      break;
    }
    case ServiceStatus::PTU_FAILED: {
      service_event = Common_ServiceEvent::REQUEST_REJECTED;
      auto service_update_reason_val = Common_ServiceUpdateReason::PTU_FAILED;
      service_update_reason = UpdateReasonOptional(service_update_reason_val);
      break;
    }
    case ServiceStatus::CERT_INVALID: {
      service_event = Common_ServiceEvent::REQUEST_REJECTED;
      auto service_update_reason_val = Common_ServiceUpdateReason::INVALID_CERT;
      service_update_reason = UpdateReasonOptional(service_update_reason_val);
      break;
    }
    case ServiceStatus::INVALID_TIME: {
      service_event = Common_ServiceEvent::REQUEST_REJECTED;
      auto service_update_reason_val = Common_ServiceUpdateReason::INVALID_TIME;
      service_update_reason = UpdateReasonOptional(service_update_reason_val);
      break;
    }
    default: {
      LOG4CXX_WARN(logger_,
                   "Received unknown ServiceStatus: "
                       << static_cast<int32_t>(service_status));
      return;
    }
  }

  listener_->ProcessServiceStatusUpdate(
      connection_key, hmi_service_type, service_event, service_update_reason);
}
}  // namespace protocol_handler

#include "application_manager/rpc_protection_manager_impl.h"
#include "application_manager/message_helper.h"
#include "application_manager/application.h"

CREATE_LOGGERPTR_LOCAL(logger_, "RPCProtectionManagerImpl");

namespace application_manager {

namespace rpc_encryption_exceptions {
const std::string kRegisterAppInterface = "RegisterAppInterface";
const std::string kSystemRequest = "SystemRequest";
const std::string kOnPermissionChange = "OnPermissionsChange";
const std::string kOnSystemRequest = "OnSystemRequest";
const std::string kPutFile = "PutFile";
const std::string kOnHMIStatus = "OnHMIStatus";
}

RPCProtectionManagerImpl::RPCProtectionManagerImpl(
    policy::PolicyHandlerInterface& policy_handler)
    : policy_handler_(policy_handler) {
  LOG4CXX_AUTO_TRACE(logger_);
}

bool RPCProtectionManagerImpl::IsFunctionInGroup(
    const std::string& function, const std::string& group) const {
  const auto& rpc_encryption_data_accessor =
      policy_handler_.RPCEncryptionDataAccessor();

  const auto group_rpcs = rpc_encryption_data_accessor.GetRPCsForGroup(group);

  const auto it = std::find(group_rpcs.begin(), group_rpcs.end(), function);

  return it != group_rpcs.end();
}

bool RPCProtectionManagerImpl::CheckPolicyEncryptionFlag(
    const uint32_t function_id,
    const Application& app,
    const uint32_t correlation_id,
    const bool is_rpc_service_secure) {
  LOG4CXX_AUTO_TRACE(logger_);
  const auto& rpc_encryption_data_accessor =
      policy_handler_.RPCEncryptionDataAccessor();
  const std::string function_name =
      rpc_encryption_data_accessor.GetPolicyFunctionName(function_id);
  LOG4CXX_DEBUG(logger_,
                "Function for check is " << function_name
                                         << " conrrelation_id is "
                                         << correlation_id);

  if (!is_rpc_service_secure && IsExceptionRPC(function_id)) {
    return false;
  }

  const auto policy_app_id = app.policy_app_id();
  if (!rpc_encryption_data_accessor.AppNeedEncryption(policy_app_id)) {
    return false;
  }

  const auto app_rpc_groups =
      rpc_encryption_data_accessor.GetGroupsForApp(policy_app_id);

  for (const auto& group : app_rpc_groups) {
    const bool is_function_in_group = IsFunctionInGroup(function_name, group);
    if (is_function_in_group &&
        rpc_encryption_data_accessor.GroupNeedEncryption(group)) {
      LOG4CXX_DEBUG(logger_,
                    "Message need encryption. Function name is "
                        << function_name);
      message_needed_encryption_.insert(
          std::make_pair(app.app_id(), correlation_id));
      return true;
    }
  }

  return false;
}

bool RPCProtectionManagerImpl::DoesRPCNeedEncryption(
    const uint32_t app_id, const uint32_t correlation_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_DEBUG(logger_, "conrrelation_id is " << correlation_id);

  auto it =
      message_needed_encryption_.find(std::make_pair(app_id, correlation_id));
  if (it != message_needed_encryption_.end()) {
    message_needed_encryption_.erase(it);
    return true;
  }
  return false;
}

bool RPCProtectionManagerImpl::IsExceptionRPC(
    const uint32_t function_id) const {
  using namespace rpc_encryption_exceptions;
  const std::string policy_fucntion_id = policy_table::EnumToJsonString(
      static_cast<policy_table::FunctionID>(function_id));
  return (kRegisterAppInterface == policy_fucntion_id ||
          kSystemRequest == policy_fucntion_id ||
          kOnSystemRequest == policy_fucntion_id ||
          kOnPermissionChange == policy_fucntion_id ||
          kPutFile == policy_fucntion_id || kOnHMIStatus == policy_fucntion_id);
}

void RPCProtectionManagerImpl::ForceEncryptResponse(
    const uint32_t app_id, const uint32_t correlation_id) {
  message_needed_encryption_.insert(std::make_pair(app_id, correlation_id));
};

smart_objects::SmartObjectSPtr
RPCProtectionManagerImpl::CreateEncryptionNeededResponse(
    const uint32_t connection_key,
    const uint32_t function_id,
    const uint32_t correlation_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  auto it = message_needed_encryption_.find(
      std::make_pair(connection_key, correlation_id));
  if (it != message_needed_encryption_.end()) {
    message_needed_encryption_.erase(it);
  } else {
    LOG4CXX_WARN(logger_,
                 "RPC for correlation id: " << correlation_id << " and app id: "
                                            << connection_key << " not found");
  }
  return MessageHelper::CreateNegativeResponse(
      connection_key,
      function_id,
      correlation_id,
      static_cast<int32_t>(mobile_apis::Result::ENCRYPTION_NEEDED));
}
}  // namespace protocol_handler

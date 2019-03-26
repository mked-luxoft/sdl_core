#include "application_manager/rpc_protection_mediator_impl.h"
#include "application_manager/message_helper.h"
#include "application_manager/application.h"

CREATE_LOGGERPTR_LOCAL(logger_, "RPCProtectionMediatorImpl");

namespace application_manager {

namespace rpc_encryption_exceptions {
const std::string kRegisterAppInterface = "RegisterAppInterface";
const std::string kSystemRequest = "SystemRequest";
const std::string kOnPermissionChange = "OnPermissionsChange";
const std::string kOnSystemRequest = "OnSystemRequest";
const std::string kPutFile = "PutFile";
const std::string kOnHMIStatus = "OnHMIStatus";
}

RPCProtectionMediatorImpl::RPCProtectionMediatorImpl(
    policy::PolicyHandlerInterface& policy_handler)
    : policy_handler_(policy_handler) {
  LOG4CXX_AUTO_TRACE(logger_);
}

bool RPCProtectionMediatorImpl::IsFunctionInGroup(
    const std::string& function, const std::string& group) const {
  const auto& rpc_encryption_manager = policy_handler_.RPCEncryptionManager();

  const auto group_rpcs = rpc_encryption_manager.GetRPCsForGroup(group);

  auto it = std::find(group_rpcs.begin(), group_rpcs.end(), function);

  return it != group_rpcs.end();
}

bool RPCProtectionMediatorImpl::DoesRPCNeedEncryption(
    const uint32_t function_id,
    std::shared_ptr<Application> app,
    const uint32_t conrrelation_id,
    const bool is_rpc_service_secure) {
  LOG4CXX_AUTO_TRACE(logger_);
  const auto& rpc_encryption_manager = policy_handler_.RPCEncryptionManager();
  const std::string function_name =
      rpc_encryption_manager.GetPolicyFunctionName(function_id);
  LOG4CXX_DEBUG(logger_,
                "Function for check is " << function_name
                                         << " conrrelation_id is "
                                         << conrrelation_id);

  if (!is_rpc_service_secure && IsExceptionRPC(function_id)) {
    return false;
  }

  const auto policy_app_id = app->policy_app_id();
  if (!rpc_encryption_manager.AppNeedEncryption(policy_app_id)) {
    return false;
  }

  const auto& app_rpc_groups =
      rpc_encryption_manager.GetGroupsForApp(policy_app_id);

  bool encrypted_need = false;
  for (const auto& group : app_rpc_groups) {
    const bool is_function_in_group = IsFunctionInGroup(function_name, group);
    if (is_function_in_group) {
      encrypted_need |= rpc_encryption_manager.GroupNeedEncryption(group);
    }
  }

  if (encrypted_need) {
    LOG4CXX_DEBUG(
        logger_, "Message need encryption. Function name is " << function_name);
    message_needed_encryption_.insert(conrrelation_id);
  }

  return encrypted_need;
}

bool RPCProtectionMediatorImpl::DoesRPCNeedEncryption(
    const uint32_t conrrelation_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_DEBUG(logger_, "conrrelation_id is " << conrrelation_id);

  auto itr = message_needed_encryption_.find(conrrelation_id);
  if (itr != message_needed_encryption_.end()) {
    message_needed_encryption_.erase(itr);
    return !IsNegativeResponse(conrrelation_id);
  }
  return false;
}

bool RPCProtectionMediatorImpl::IsExceptionRPC(
    const uint32_t function_id) const {
  using namespace rpc_encryption_exceptions;
  const std::string policy_fucntion_id = policy_table::EnumToJsonString(
      static_cast<policy_table::FunctionID>(function_id));
  return (policy_fucntion_id == kRegisterAppInterface ||
          policy_fucntion_id == kSystemRequest ||
          policy_fucntion_id == kOnSystemRequest ||
          policy_fucntion_id == kOnPermissionChange ||
          policy_fucntion_id == kPutFile || policy_fucntion_id == kOnHMIStatus);
}

bool RPCProtectionMediatorImpl::IsNegativeResponse(
    const uint32_t conrrelation_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  auto itr = negative_responses_.find(conrrelation_id);
  if (itr != negative_responses_.end()) {
    negative_responses_.erase(itr);
    return true;
  }
  return false;
}

void RPCProtectionMediatorImpl::EncryptByForce(const uint32_t conrrelation_id) {
  message_needed_encryption_.insert(conrrelation_id);
};

smart_objects::SmartObjectSPtr
RPCProtectionMediatorImpl::CreateNegativeResponse(
    const uint32_t connection_key,
    const uint32_t function_id,
    const uint32_t conrrelation_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  negative_responses_.insert(conrrelation_id);
  return MessageHelper::CreateNegativeResponse(
      connection_key,
      function_id,
      conrrelation_id,
      static_cast<int32_t>(mobile_apis::Result::ENCRYPTION_NEEDED));
}
}  // namespace protocol_handler

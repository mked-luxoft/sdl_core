#include "application_manager/rpc_protection_mediator_impl.h"
#include "application_manager/message_helper.h"
CREATE_LOGGERPTR_LOCAL(logger_, "RPCProtectionMediatorImpl");

namespace application_manager {

namespace rpc_encryption_exceptions {
const char* kRegisterAppInterface = "RegisterAppInterface";
const char* kSystemRequest = "SystemRequest";
const char* kOnPermissionChange = "OnPermissionsChange";
const char* kOnSystemRequest = "OnSystemRequest";
const char* kPutFile = "PutFile";
}

RPCProtectionMediatorImpl::RPCProtectionMediatorImpl(
    policy::PolicyHandlerInterface& policy_handler,
    ApplicationManager& app_manager)
    : policy_handler_(policy_handler), app_manager_(app_manager) {
  LOG4CXX_AUTO_TRACE(logger_);
}

bool RPCProtectionMediatorImpl::IsFunctionInGroup(
    const std::string& function, const std::string& group) const {
  const auto& rpc_encryption_manager = policy_handler_.RPCEncryptionManager();

  const auto group_rpcs = rpc_encryption_manager->GetRPCsForGroup(group);

  auto it = std::find(group_rpcs.begin(), group_rpcs.end(), function);

  return it != group_rpcs.end();
}

bool RPCProtectionMediatorImpl::DoesRPCNeedEncryption(
    const uint32_t function_id, const uint32_t app_id) {
  LOG4CXX_AUTO_TRACE(logger_);

  const auto& rpc_encryption_manager = policy_handler_.RPCEncryptionManager();
  const auto policy_app_id = app_manager_.application(app_id)->policy_app_id();
  const auto policy_function_id =
      rpc_encryption_manager->GetPolicyFunctionName(function_id);

  if (!rpc_encryption_manager->AppNeedEncryption(policy_app_id)) {
    return false;
  }

  const auto& app_rpc_groups =
      rpc_encryption_manager->GetGroupsForApp(policy_app_id);

  bool encrypted_need = false;
  for (const auto& group : app_rpc_groups) {
    const bool is_fid_in_group = IsFunctionInGroup(policy_function_id, (group));
    if (is_fid_in_group) {
      encrypted_need |= rpc_encryption_manager->GroupNeedEncryption(group);
    }
  }
  return encrypted_need;

  return false;
}

bool RPCProtectionMediatorImpl::IsException(const uint32_t function_id) const {
  using namespace rpc_encryption_exceptions;
  const std::string policy_fucntion_id = policy_table::EnumToJsonString(
      static_cast<policy_table::FunctionID>(function_id));
  return (policy_fucntion_id == kRegisterAppInterface ||
          policy_fucntion_id == kSystemRequest ||
          policy_fucntion_id == kOnSystemRequest ||
          policy_fucntion_id == kOnPermissionChange ||
          policy_fucntion_id == kPutFile);
}

void RPCProtectionMediatorImpl::SendEncryptionNeededError(
    const uint32_t function_id,
    const uint32_t correlation_id,
    const uint32_t connection_key) {
  LOG4CXX_AUTO_TRACE(logger_);

  const auto response = MessageHelper::CreateNegativeResponse(
      connection_key,
      function_id,
      correlation_id,
      static_cast<int32_t>(mobile_apis::Result::ENCRYPTION_NEEDED));

  app_manager_.GetRPCService().ManageMobileCommand(
      response, commands::Command::SOURCE_SDL);
}
}  // namespace protocol_handler

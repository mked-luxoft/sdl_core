#include "application_manager/rpc_protection_mediator_impl.h"

CREATE_LOGGERPTR_LOCAL(logger_, "RPCProtectionMediatorImpl");

namespace application_manager {

RPCProtectionMediatorImpl::RPCProtectionMediatorImpl(
    policy::PolicyManager& policy_manager, ApplicationManager& app_manager)
    : policy_manager_(policy_manager), app_manager_(app_manager) {
  LOG4CXX_AUTO_TRACE(logger_);
}

bool RPCProtectionMediatorImpl::DoesRPCNeedEncryption(
    const uint32_t function_id, const uint32_t app_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_WARN(logger_, "NOT YET IMPLEMENTED!");
  return false;
}

void RPCProtectionMediatorImpl::SendEncryptionNeededError(
    const uint32_t function_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_WARN(logger_, "NOT YET IMPLEMENTED!");
}
}  // namespace protocol_handler

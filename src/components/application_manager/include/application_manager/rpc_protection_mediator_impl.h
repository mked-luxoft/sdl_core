/*
 * Copyright (c) 2019, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MEDIATOR_IMPL_H_
#define SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MEDIATOR_IMPL_H_

#include "application_manager/rpc_protection_mediator.h"
#include "policy/policy_manager.h"
#include "application_manager/application_manager.h"

namespace application_manager {
class RPCProtectionMediatorImpl : application_manager::RPCProtectionMediator {
  RPCProtectionMediatorImpl(policy::PolicyManager& policy_manager,
                            ApplicationManager& app_manager);

  ~RPCProtectionMediatorImpl() OVERRIDE {}

  bool DoesRPCNeedEncryption(const uint32_t function_id,
                             const uint32_t app_id) OVERRIDE;
  void SendEncryptionNeededError(const uint32_t function_id) OVERRIDE;

 private:
  policy::PolicyManager& policy_manager_;
  ApplicationManager& app_manager_;
};
}  // namespace policy

#endif  // SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MEDIATOR_IMPL_H_

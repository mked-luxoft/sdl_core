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

#ifndef SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MANAGER_IMPL_H_
#define SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MANAGER_IMPL_H_

#include <set>
#include "application_manager/rpc_protection_manager.h"
#include "application_manager/policies/policy_handler.h"

namespace application_manager {
/*
* @brief RPCProtectionManager implementation
*/
class RPCProtectionManagerImpl : public RPCProtectionManager {
 public:
  /*!
   * @brief constructor RPCProtectionManagerImpl
   * @param policy_handler policy handler interface
   */
  RPCProtectionManagerImpl(policy::PolicyHandlerInterface& policy_handler);

  /*!
   * @brief destructor RPCProtectionManagerImpl
   */
  ~RPCProtectionManagerImpl() OVERRIDE {}

  /*
   * @param function_id function id
   * @param app smart pointer to Application
   * @param conrrelation_id conrrelation id
   * @param is_rpc_service_secure the flag the secure service started
   * @return true if function need encryption for current app,  else false
   */
  bool CheckPolicyEncryptionFlag(const uint32_t function_id,
                                 const Application& app,
                                 const uint32_t conrrelation_id,
                                 const bool is_rpc_service_secure) OVERRIDE;
  /*
   * @param conrrelation_id conrrelation id
   * @return true if the message with correlation id correlation_id needed e
   * ncryption else false
   */
  bool DoesRPCNeedEncryption(const uint32_t app_id,
                             const uint32_t conrrelation_id) OVERRIDE;
  /*
   * @brief massage will be encrypted by force
   * If request encrypted but not needed by policy, sdl must encrypted response
   * too
   * @param conrrelation_id conrrelation id
   */
  void ForceEncryptResponse(const uint32_t app_id,
                            const uint32_t conrrelation_id) OVERRIDE;
  /*
   * @param connection_key connection key
   * @param function_id function id
   * @param conrrelation_id conrrelation id
   * @return response with error code ENCRYPTION_NEEDED
   */
  smart_objects::SmartObjectSPtr CreateEncryptionNeededResponse(
      const uint32_t connection_key,
      const uint32_t function_id,
      const uint32_t conrrelation_id) OVERRIDE;

 private:
  /*
   * @param function_id function id
   * @return true if function_id is an exception (rpc that can be sent before
   * app is registered, hence before secure rpc service is established)
   */
  bool IsExceptionRPC(const uint32_t function_id) const;
  /*
   * @param app_id application id
   * @param conrrelation_id conrrelation id
   * @return true if the message with correlation id is a negative response with
   * result code ENCRYPTION_NEEDED
   */
  // bool CheckNegativeResponse(const uint32_t app_id,
  //                            const uint32_t conrrelation_id);
  /*
   * @param function function name
   * @param group group name
   * @return true if the function exists in group else return false
   */
  bool IsFunctionInGroup(const std::string& function,
                         const std::string& group) const;

  policy::PolicyHandlerInterface& policy_handler_;

  typedef std::pair<uint32_t, uint32_t> AppIdCorrIdPair;

  std::set<AppIdCorrIdPair> negative_responses_;
  std::set<AppIdCorrIdPair> message_needed_encryption_;
};
}  // namespace policy

#endif  // SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MANAGER_IMPL_H_

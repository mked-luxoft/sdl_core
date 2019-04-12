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

#ifndef SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MANAGER_H_
#define SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MANAGER_H_

#include <cstdint>
#include <memory>

namespace ns_smart_device_link {
namespace ns_smart_objects {
class SmartObject;
}  // namespace ns_smart_objects
}  // namespace ns_smart_device_link
namespace smart_objects = ns_smart_device_link::ns_smart_objects;

namespace application_manager {
class Application;
}  // namespace application_manager

namespace application_manager {
/**
  * @brief RPCProtectionManager interface
  * This entity exists to get info from policy table regarding encryption on
  * application and function group level, as well as make decisions whether
  * certain RPC should be encrypted or not.
  * It mediates communication between PRCService and
  * RPCEncryptionDataAccessorInterface which is implemented by PolicyManager,
  * providing adequate level of abstraction.
  */
class RPCProtectionManager {
 public:
  /*!
   * @brief virtual destructor RPCProtectionManager
   */
  virtual ~RPCProtectionManager() {}

  /*
   * @param function_id function id
   * @param app smart pointer to Application
   * @param conrrelation_id conrrelation id
   * @param is_rpc_service_secure the flag the secure service started
   * @return true if function need encryption for current app,  else false
   */
  virtual bool CheckPolicyEncryptionFlag(const uint32_t function_id,
                                         std::shared_ptr<Application> app,
                                         const uint32_t conrrelation_id,
                                         const bool is_rpc_service_secure) = 0;
  /*
   * @param app_id application id
   * @param correlation_id conrrelation id
   * @return true if the message with correlation id correlation_id needed e
   * ncryption else false
   */
  virtual bool DoesRPCNeedEncryption(const uint32_t app_id,
                                     const uint32_t conrrelation_id) = 0;
  /*
  * @brief massage will be encrypted by force
  * If request encrypted but not needed by policy, sdl must encrypted response
  * too
  * @param app_id application id
  * @param conrrelation_id conrrelation id
  */
  virtual void EncryptResponseByForce(const uint32_t app_id,
                                      const uint32_t conrrelation_id) = 0;
  /*
   * @param connection_key connection key
   * @param function_id function id
   * @param conrrelation_id conrrelation id
   * @return response with error code ENCRYPTION_NEEDED
   */
  virtual std::shared_ptr<smart_objects::SmartObject>
  CreateEncryptionNeededResponse(const uint32_t connection_key,
                                 const uint32_t function_id,
                                 const uint32_t conrrelation_id) = 0;
};
}  // namespace policy

#endif  // SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_PROTOCOL_HANDLER_RPC_PROTECTION_MANAGER_H_

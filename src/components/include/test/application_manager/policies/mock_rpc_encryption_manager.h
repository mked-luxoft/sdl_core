/*
 * Copyright (c) 2015, Ford Motor Company
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

#ifndef SRC_COMPONENTS_INCLUDE_TEST_APPLICATION_MANAGER_POLICIES_MOCK_RPC_ENCRYPTION_MANAGER_H_
#define SRC_COMPONENTS_INCLUDE_TEST_APPLICATION_MANAGER_POLICIES_MOCK_RPC_ENCRYPTION_MANAGER_H_

#include <map>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "application_manager/policies/rpc_encryption_manager_interface.h"

namespace test {
namespace components {
namespace application_manager_test {

class MockRPCEncryptionManagerInterface
    : public ::policy::RPCEncryptionManagerInterface {
 public:
  MOCK_CONST_METHOD1(AppNeedEncryption, bool(const std::string& policy_app_id));
  MOCK_CONST_METHOD1(GetGroupsForApp,
                     const Strings&(const std::string& policy_app_id));
  MOCK_CONST_METHOD2(FunctionNeedEncryption,
                     bool(const std::string& policy_group,
                          const std::string& policy_function_id));
  MOCK_CONST_METHOD1(GroupNeedEncryption,
                     bool(const std::string& policy_group));
  MOCK_CONST_METHOD1(
      GetAppEncryptionRequired,
      const rpc::Optional<rpc::Boolean>&(const std::string& policy_app_id));
  MOCK_CONST_METHOD1(GetRPCsForGroup,
                     const std::vector<std::string>(const std::string& group));
  MOCK_CONST_METHOD1(GetPolicyFunctionName,
                     const std::string(const uint32_t function_id));
};
}  //  namespace application_manager_test
}  //  namespace components
}  //  namespace test

#endif  // SRC_COMPONENTS_INCLUDE_TEST_APPLICATION_MANAGER_POLICIES_MOCK_RPC_ENCRYPTION_MANAGER_H_

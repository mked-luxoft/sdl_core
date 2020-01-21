/*
 * Copyright (c) 2020 Ford Motor Company
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
 * Neither the names of the copyright holders nor the names of its contributors
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

#include "hmi/bc_get_app_properties_response.h"
#include "application_manager/commands/command_impl.h"
#include "application_manager/commands/commands_test.h"
#include "application_manager/event_engine/event.h"
#include "application_manager/mock_event_dispatcher.h"
#include "application_manager/smart_object_keys.h"
#include "gtest/gtest.h"
#include "smart_objects/smart_object.h"

namespace test {
namespace components {
namespace commands_test {
namespace hmi_commands_test {
namespace bc_get_app_properties_response {

using sdl_rpc_plugin::commands::BCGetAppPropertiesResponse;
using test::components::event_engine_test::MockEventDispatcher;
using test::components::policy_test::MockPolicyHandlerInterface;
using ::testing::_;
using ::testing::NiceMock;
using ::testing::ReturnRef;

class BCGetAppPropertiesResponseTest
    : public CommandsTest<CommandsTestMocks::kIsNice> {};

TEST_F(BCGetAppPropertiesResponseTest, RUN_SUCCESS) {
  MessageSharedPtr msg = CreateMessage();

  std::shared_ptr<BCGetAppPropertiesResponse> command(
      CreateCommand<BCGetAppPropertiesResponse>(msg));

  EXPECT_CALL(mock_rpc_service_, SendMessageToHMI(msg));

  ASSERT_TRUE(command->Init());
  command->Run();
}

}  // namespace bc_get_app_properties_response
}  // namespace hmi_commands_test
}  // namespace commands_test
}  // namespace components
}  // namespace test

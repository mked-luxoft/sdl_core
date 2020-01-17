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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "transport_manager/mock_transport_manager_settings.h"
#include "transport_manager/transport_adapter/mock_transport_adapter_controller.h"
#include "transport_manager/websocket/websocket_listener.h"

#include "transport_manager/websocket_server/websocket_sample_client.h"

namespace test {
namespace components {
namespace transport_manager_test {

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using namespace ::transport_manager;
using namespace ::transport_manager::transport_adapter;

class WebSocketListenerTest : public ::testing::Test {
 protected:
  std::shared_ptr<WebSocketListener> ws_listener;
  MockTransportManagerSettings mock_tm_settings_;
  MockTransportAdapterController mock_ta_controller_;
  std::shared_ptr<WSSampleClient> ws_client_;

 public:
  void SetUp() OVERRIDE {
    ON_CALL(mock_tm_settings_, websocket_server_address())
        .WillByDefault(Return("127.0.0.1"));
    ON_CALL(mock_tm_settings_, websocket_server_port())
        .WillByDefault(Return("2020"));
  }
};

TEST_F(WebSocketListenerTest, StartListening_ClientConnect_SUCCESS) {
  ws_listener.reset();
  ws_client_.reset();
  ws_listener = std::make_shared<WebSocketListener>(nullptr, mock_tm_settings_);
  ws_client_ = std::make_shared<WSSampleClient>();

  EXPECT_CALL(mock_ta_controller_, AddDevice(_));
  //  EXPECT_CALL(mock_ta_controller_, ConnectDone(_, _));
  //  EXPECT_CALL(mock_ta_controller_, ConnectionCreated(_, _, _))

  ws_listener->StartListening();
  ws_client_->run();
}

}  // namespace transport_manager_test
}  // namespace components
}  // namespace test

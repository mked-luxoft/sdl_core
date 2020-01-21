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
#include "utils/timer.h"
#include "utils/timer_task_impl.h"

#include <thread>
#include "transport_manager/websocket_server/websocket_sample_client.h"
template class WSSampleClient<websocket::stream<tcp::socket> >;

namespace test {
namespace components {
namespace transport_manager_test {

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnArg;
using ::testing::ReturnPointee;
using ::testing::ReturnRef;
using namespace ::transport_manager;
using namespace ::transport_manager::transport_adapter;

namespace {
const std::string kDefaultAddress = "127.0.0.1";
const std::string kDefaultCertPath = "";
const std::string kDefaultKeyPath = "";
const std::string kDefaultCACertPath = "";
const uint32_t kDefaultPort = 2020;
const std::string kDefaultPortStr = "2020";
const std::string kWSPath = "ws://";
const std::string kWSSPath = "wss://";
const std::string kWSValidTarget =
    kWSPath + kDefaultAddress + ":" + std::to_string(kDefaultPort);
const std::string kWSSValidTarget =
    kWSSPath + kDefaultAddress + ":" + std::to_string(kDefaultPort);
const std::string kCACertPath = "./test_certs/ca-cert.pem";
const std::string kClientCertPath = "./test_certs/client-cert.pem";
const std::string kClientKeyPath = "./test_certs/client-key.pem";
}  // namespace

class WebSocketListenerTest : public ::testing::Test {
 protected:
  std::shared_ptr<WebSocketListener> ws_listener_;
  MockTransportAdapterController mock_ta_controller_;
  MockTransportManagerSettings mock_tm_settings_;
  std::shared_ptr<WSSampleClient<WS> > ws_client_;
  std::shared_ptr<WSSampleClient<WSS> > wss_client_;

 public:
  WebSocketListenerTest() {}
  ~WebSocketListenerTest() OVERRIDE {}

  void SetUp() OVERRIDE {
    ON_CALL(mock_tm_settings_, websocket_server_address())
        .WillByDefault(ReturnRef(kDefaultAddress));
    ON_CALL(mock_tm_settings_, websocket_server_port())
        .WillByDefault(Return(kDefaultPort));
  }
};

TEST_F(WebSocketListenerTest, StartListening_ClientConnect_SUCCESS) {
  ON_CALL(mock_tm_settings_, ws_server_cert_path())
      .WillByDefault(ReturnRef(kDefaultCertPath));
  ON_CALL(mock_tm_settings_, ws_server_key_path())
      .WillByDefault(ReturnRef(kDefaultKeyPath));
  ON_CALL(mock_tm_settings_, ws_server_ca_cert_path())
      .WillByDefault(ReturnRef(kDefaultCACertPath));

  ws_listener_ = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  ws_client_ =
      std::make_shared<WSSampleClient<WS> >(kDefaultAddress, kDefaultPortStr);

  EXPECT_CALL(mock_ta_controller_, AddDevice(_)).WillOnce(ReturnArg<0>());
  EXPECT_CALL(mock_ta_controller_, ConnectDone(_, _));
  EXPECT_CALL(mock_ta_controller_, ConnectionCreated(_, _, _));

  //  std::thread server_thread(
  //      std::bind(&WebSocketListener::StartListening, ws_listener_.get()));
  ws_listener_->StartListening();
  usleep(1000);
  EXPECT_TRUE(ws_client_->run());
  usleep(1000);
  ws_client_->stop();
  //  server_thread.join();
  ws_listener_.reset();
  ws_client_.reset();
}

TEST_F(WebSocketListenerTest, StartListening_ClientConnectSecure_SUCCESS) {
  const std::string server_cert = "./test_certs/server-cert.pem";
  const std::string server_key = "./test_certs/server-key.pem";
  const std::string ca_cert = "./test_certs/ca-cert.pem";
  ON_CALL(mock_tm_settings_, ws_server_cert_path())
      .WillByDefault(ReturnRef(server_cert));
  ON_CALL(mock_tm_settings_, ws_server_key_path())
      .WillByDefault(ReturnRef(server_key));
  ON_CALL(mock_tm_settings_, ws_server_ca_cert_path())
      .WillByDefault(ReturnRef(ca_cert));

  ws_listener_ = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  const SecurityParams params{kCACertPath, kClientCertPath, kClientKeyPath};
  wss_client_ = std::make_shared<WSSampleClient<WSS> >(
      kDefaultAddress, kDefaultPortStr, params);

  EXPECT_CALL(mock_ta_controller_, AddDevice(_)).WillOnce(ReturnArg<0>());
  EXPECT_CALL(mock_ta_controller_, ConnectDone(_, _));
  EXPECT_CALL(mock_ta_controller_, ConnectionCreated(_, _, _));

  //  std::thread server_thread(
  //      std::bind(&WebSocketListener::StartListening, ws_listener_.get()));
  ws_listener_->StartListening();
  sleep(1);
  EXPECT_TRUE(wss_client_->run());
  usleep(5000);
  wss_client_->stop();
  //  server_thread.join();
  ws_listener_.reset();
  wss_client_.reset();
}

// TEST_F(WebSocketListenerTest,
//       StartListening_ClientConnectSecureInvalidTarget_FAIL) {
//  const std::string server_cert = "./test_certs/server-cert.pem";
//  const std::string server_key = "./test_certs/server-key.pem";
//  const std::string ca_cert = "./test_certs/ca-cert.pem";
//  ON_CALL(mock_tm_settings_, ws_server_cert_path())
//      .WillByDefault(ReturnRef(server_cert));
//  ON_CALL(mock_tm_settings_, ws_server_key_path())
//      .WillByDefault(ReturnRef(server_key));
//  ON_CALL(mock_tm_settings_, ws_server_ca_cert_path())
//      .WillByDefault(ReturnRef(ca_cert));

//  ws_listener_ = std::make_shared<WebSocketListener>(
//      &mock_ta_controller_, mock_tm_settings_, 2);
//  const SecurityParams params{kCACertPath, kClientCertPath, kClientKeyPath};
//  wss_client_ = std::make_shared<WSSampleClient<WSS> >(
//      kDefaultAddress, kWSValidTarget, params);

//  EXPECT_CALL(mock_ta_controller_, AddDevice(_)).WillOnce(ReturnArg<0>());
//  EXPECT_CALL(mock_ta_controller_, ConnectDone(_, _));
//  EXPECT_CALL(mock_ta_controller_, ConnectionCreated(_, _, _));

//  //  std::thread server_thread(
//  //      std::bind(&WebSocketListener::StartListening, ws_listener_.get()));
//  ws_listener_->StartListening();
//  usleep(1000);
//  EXPECT_FALSE(wss_client_->run());
//  usleep(5000);
//  wss_client_->stop();
//  //  server_thread.join();
//  ws_listener_.reset();
//  wss_client_.reset();
//}

TEST_F(WebSocketListenerTest,
       StartListening_ClientConnectSecureInvalidCert_FAIL) {
  const std::string server_cert = "./test_certs/server-cert.pem";
  const std::string server_key = "./test_certs/server-key.pem";
  const std::string ca_cert = "./test_certs/ca-cert.pem";
  ON_CALL(mock_tm_settings_, ws_server_cert_path())
      .WillByDefault(ReturnRef(server_cert));
  ON_CALL(mock_tm_settings_, ws_server_key_path())
      .WillByDefault(ReturnRef(server_key));
  ON_CALL(mock_tm_settings_, ws_server_ca_cert_path())
      .WillByDefault(ReturnRef(ca_cert));

  ws_listener_ = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  const SecurityParams params{kCACertPath,
                              "./test_certs/invalid_cert.pem",
                              "./test_certs/invalid_key.pem"};
  wss_client_ = std::make_shared<WSSampleClient<WSS> >(
      kDefaultAddress, kDefaultPortStr, params);

  EXPECT_CALL(mock_ta_controller_, AddDevice(_)).WillOnce(ReturnArg<0>());
  EXPECT_CALL(mock_ta_controller_, ConnectDone(_, _));
  EXPECT_CALL(mock_ta_controller_, ConnectionCreated(_, _, _));

  //  std::thread server_thread(
  //      std::bind(&WebSocketListener::StartListening, ws_listener_.get()));
  ws_listener_->StartListening();
  sleep(1);
  //  std::thread client_thread(
  //      std::bind(&WSSampleClient<WSS>::run, wss_client_.get()));
  timer::Timer handshake_timer(
      "HandshakeTimer",
      new ::timer::TimerTaskImpl<WSSampleClient<WSS> >(
          wss_client_.get(), &WSSampleClient<WSS>::on_handshake_timeout));
  handshake_timer.Start(3000, timer::kSingleShot);
  wss_client_->run();
  usleep(5000);
  EXPECT_EQ(wss_client_->is_handshake_successful(), false);
  //  client_thread.join();
  //  wss_client_->stop();
  //  server_thread.join();
  ws_listener_.reset();
  wss_client_.reset();
}

}  // namespace transport_manager_test
}  // namespace components
}  // namespace test

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
const uint32_t kWrongPort = 1000;
const std::string kCACertPath = "./test_certs/ca-cert.pem";
const std::string kClientCertPath = "./test_certs/client-cert.pem";
const std::string kClientKeyPath = "./test_certs/client-key.pem";
const std::string kServerCert = "./test_certs/server-cert.pem";
const std::string kServerKey = "./test_certs/server-key.pem";
const std::string kCACert = "./test_certs/ca-cert.pem";
}  // namespace

class WebSocketListenerTest : public ::testing::Test {
 protected:
  MockTransportAdapterController mock_ta_controller_;
  MockTransportManagerSettings mock_tm_settings_;

 public:
  WebSocketListenerTest() {}
  ~WebSocketListenerTest() OVERRIDE {}

  void SetUp() OVERRIDE {
    ON_CALL(mock_tm_settings_, websocket_server_address())
        .WillByDefault(ReturnRef(kDefaultAddress));
    ON_CALL(mock_tm_settings_, websocket_server_port())
        .WillByDefault(Return(kDefaultPort));
    ON_CALL(mock_tm_settings_, ws_server_cert_path())
        .WillByDefault(ReturnRef(kServerCert));
    ON_CALL(mock_tm_settings_, ws_server_key_path())
        .WillByDefault(ReturnRef(kServerKey));
    ON_CALL(mock_tm_settings_, ws_server_ca_cert_path())
        .WillByDefault(ReturnRef(kCACert));
  }
};

TEST_F(WebSocketListenerTest, StartListening_ClientConnect_SUCCESS) {
  ON_CALL(mock_tm_settings_, ws_server_cert_path())
      .WillByDefault(ReturnRef(kDefaultCertPath));
  ON_CALL(mock_tm_settings_, ws_server_key_path())
      .WillByDefault(ReturnRef(kDefaultKeyPath));
  ON_CALL(mock_tm_settings_, ws_server_ca_cert_path())
      .WillByDefault(ReturnRef(kDefaultCACertPath));

  const auto ws_listener = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  const auto ws_client =
      std::make_shared<WSSampleClient<WS> >(kDefaultAddress, kDefaultPortStr);

  EXPECT_CALL(mock_ta_controller_, AddDevice(_)).WillOnce(ReturnArg<0>());
  EXPECT_CALL(mock_ta_controller_, ConnectDone(_, _));
  EXPECT_CALL(mock_ta_controller_, ConnectionCreated(_, _, _));

  std::thread server_thread(
      std::bind(&WebSocketListener::StartListening, ws_listener.get()));
  sleep(1);
  EXPECT_TRUE(ws_client->Run());
  ws_client->Stop();
  server_thread.join();
}

TEST_F(WebSocketListenerTest, StartListening_ClientConnectSecure_SUCCESS) {
  const auto ws_listener = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  const SecurityParams params{kCACertPath, kClientCertPath, kClientKeyPath};
  const auto wss_client = std::make_shared<WSSampleClient<WSS> >(
      kDefaultAddress, kDefaultPortStr, params);

  EXPECT_CALL(mock_ta_controller_, AddDevice(_)).WillOnce(ReturnArg<0>());
  EXPECT_CALL(mock_ta_controller_, ConnectDone(_, _));
  EXPECT_CALL(mock_ta_controller_, ConnectionCreated(_, _, _));

  std::thread server_thread(
      std::bind(&WebSocketListener::StartListening, ws_listener.get()));
  sleep(1);
  EXPECT_TRUE(wss_client->Run());
  wss_client->Stop();
  server_thread.join();
}

TEST_F(WebSocketListenerTest,
       StartListening_ClientConnectSecureInvalidCert_FAIL) {
  const auto ws_listener = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  const SecurityParams params{kCACertPath,
                              "./test_certs/invalid_cert.pem",
                              "./test_certs/invalid_key.pem"};
  const auto wss_client = std::make_shared<WSSampleClient<WSS> >(
      kDefaultAddress, kDefaultPortStr, params);

  EXPECT_CALL(mock_ta_controller_, AddDevice(_)).WillOnce(ReturnArg<0>());
  EXPECT_CALL(mock_ta_controller_, ConnectDone(_, _));
  EXPECT_CALL(mock_ta_controller_, ConnectionCreated(_, _, _));

  std::thread server_thread(
      std::bind(&WebSocketListener::StartListening, ws_listener.get()));
  sleep(1);
  timer::Timer handshake_timer(
      "HandshakeTimer",
      new ::timer::TimerTaskImpl<WSSampleClient<WSS> >(
          wss_client.get(), &WSSampleClient<WSS>::OnHandshakeTimeout));
  handshake_timer.Start(3000, timer::kSingleShot);
  wss_client->Run();
  EXPECT_EQ(wss_client->IsHandshakeSuccessful(), false);
  server_thread.join();
}

TEST_F(WebSocketListenerTest, StartListening_CertificateNotFound_Fail) {
  const std::string server_cert = "./test_certs/server-cert.pem";
  const std::string server_key = "./test_certs/server-key.pem";
  const std::string ca_cert = "./not_valid_path/ca-cert.pem";
  ON_CALL(mock_tm_settings_, ws_server_cert_path())
      .WillByDefault(ReturnRef(server_cert));
  ON_CALL(mock_tm_settings_, ws_server_key_path())
      .WillByDefault(ReturnRef(server_key));
  ON_CALL(mock_tm_settings_, ws_server_ca_cert_path())
      .WillByDefault(ReturnRef(ca_cert));

  ws_listener_ = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  ws_client_ =
      std::make_shared<WSSampleClient<WS> >(kDefaultAddress, kWSValidTarget);

  EXPECT_EQ(ws_listener_->StartListening(), TransportAdapter::Error::FAIL);
}

TEST_F(WebSocketListenerTest, StartListening_WrongConfig_Fail) {
  const std::string server_cert = "./test_certs/server-cert.pem";

  ON_CALL(mock_tm_settings_, ws_server_cert_path())
      .WillByDefault(ReturnRef(server_cert));
  ON_CALL(mock_tm_settings_, ws_server_key_path())
      .WillByDefault(ReturnRef(kDefaultKeyPath));
  EXPECT_CALL(mock_tm_settings_, ws_server_ca_cert_path()).WillOnce(ReturnRef(kDefaultCACertPath));

  ws_listener_ = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  ws_client_ =
      std::make_shared<WSSampleClient<WS> >(kDefaultAddress, kWSValidTarget);

  EXPECT_EQ(ws_listener_->StartListening(), TransportAdapter::Error::FAIL);
}

TEST_F(WebSocketListenerTest, StartListening_OpenAcceptor_Fail) {
  ON_CALL(mock_tm_settings_, ws_server_cert_path())
      .WillByDefault(ReturnRef(kDefaultCertPath));
  ON_CALL(mock_tm_settings_, ws_server_key_path())
      .WillByDefault(ReturnRef(kDefaultKeyPath));
  EXPECT_CALL(mock_tm_settings_, ws_server_ca_cert_path()).WillOnce(ReturnRef(kDefaultCACertPath));
  EXPECT_CALL(mock_tm_settings_, websocket_server_port())
        .WillOnce(Return(kWrongPort));

  ws_listener_ = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  ws_client_ =
      std::make_shared<WSSampleClient<WS> >(kDefaultAddress, kWSValidTarget);

  EXPECT_EQ(ws_listener_->StartListening(), TransportAdapter::Error::FAIL);
}

TEST_F(WebSocketListenerTest, StartListening_SuccesRun_Fail) {
  ON_CALL(mock_tm_settings_, ws_server_cert_path())
      .WillByDefault(ReturnRef(kDefaultCertPath));
  ON_CALL(mock_tm_settings_, ws_server_key_path())
      .WillByDefault(ReturnRef(kDefaultKeyPath));
  EXPECT_CALL(mock_tm_settings_, ws_server_ca_cert_path()).WillOnce(ReturnRef(kDefaultCACertPath));

  ws_listener_ = std::make_shared<WebSocketListener>(
      &mock_ta_controller_, mock_tm_settings_, 2);
  ws_client_ =
      std::make_shared<WSSampleClient<WS> >(kDefaultAddress, kWSValidTarget);

  ws_listener_->Terminate();
  EXPECT_EQ(ws_listener_->StartListening(), TransportAdapter::Error::FAIL);
}

}  // namespace transport_manager_test
}  // namespace components
}  // namespace test

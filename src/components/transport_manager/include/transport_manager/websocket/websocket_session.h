/*
Copyright (c) 2019 Livio, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

* Neither the name of SmartDeviceLink Consortium, Inc. nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <algorithm>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <thread>
#include <vector>

using tcp = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;  // from <boost/asio/ssl.hpp>
namespace websocket =
    boost::beast::websocket;  // from <boost/beast/websocket.hpp>

namespace transport_manager {
namespace transport_adapter {

CREATE_LOGGERPTR_GLOBAL(ws_logger_, "WebSocketSession")

class TransportAdapterController;

template <class ExecutorType = tcp::socket&>
class WebSocketSession
    : public std::enable_shared_from_this<WebSocketSession<ExecutorType> > {
 public:
  WebSocketSession(boost::asio::ip::tcp::socket socket,
                   TransportAdapterController* controller);

  WebSocketSession(boost::asio::ip::tcp::socket socket,
                   ssl::context& ctx,
                   TransportAdapterController* controller);

  virtual ~WebSocketSession();

  virtual void AsyncAccept();
  virtual void AsyncRead(boost::system::error_code ec);
  virtual void WriteDown(::protocol_handler::RawMessagePtr message);
  virtual ExecutorType GetExecutor();
  virtual void Read(boost::system::error_code ec,
                    std::size_t bytes_transferred);

 protected:
  tcp::socket socket_;
  websocket::stream<ExecutorType> ws_;
  boost::asio::strand<boost::asio::io_context::executor_type> strand_;
  boost::beast::multi_buffer buffer_;
  TransportAdapterController* controller_;
};

}  // namespace transport_adapter
}  // namespace transport_manager

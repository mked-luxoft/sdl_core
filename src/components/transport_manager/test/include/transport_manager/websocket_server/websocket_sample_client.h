#ifndef SRC_COMPONENTS_TRANSPORT_MANAGER_TEST_INCLUDE_TRANSPORT_MANAGER_WEBSOCKET_SERVER_WEBSOCKET_SAMPLE_CLIENT
#define SRC_COMPONENTS_TRANSPORT_MANAGER_TEST_INCLUDE_TRANSPORT_MANAGER_WEBSOCKET_SERVER_WEBSOCKET_SAMPLE_CLIENT

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class WSSampleClient : public std::enable_shared_from_this<WSSampleClient> {
 public:
  WSSampleClient() : resolver_(ioc_), ws_(ioc_), ctx_(ssl::context::sslv23) {}

  void run() {
    const auto host = "127.0.0.1";
    const auto port = "2020";
    const auto target = "";
    boost::system::error_code ec;

    const auto results = resolver_.resolve(host, port, ec);
    if (ec) {
      std::string str_err = "ErrorMessage: " + ec.message();
    }

    std::cout << "KEK!!!!RESOLVED!!!!!" << std::endl;

    boost::asio::connect(ws_.next_layer(), results.begin(), results.end(), ec);
    if (ec) {
      std::string str_err = "ErrorMessage: " + ec.message();
    }
    std::cout << "KEK!!!!CONNECTED!!!!!" << std::endl;

    ws_.handshake(host, target, ec);
    if (ec) {
      std::string str_err = "ErrorMessage: " + ec.message();
    }
    std::cout << "KEK!!!!HANDSHAKE!!!!!" << std::endl;
    ws_.async_read(buffer_,
                   std::bind(&WSSampleClient::on_read,
                             this->shared_from_this(),
                             std::placeholders::_1,
                             std::placeholders::_2));
    std::cout << "KEK!!!!ASYNC_READ!!!!!" << std::endl;
    boost::asio::post(io_pool_, [&]() { ioc_.run(); });
    std::cout << "KEK!!!!POST!!!!!" << std::endl;
  }

  void on_write(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // Read a message into our buffer
    ws_.async_read(buffer_,
                   std::bind(&WSSampleClient::on_read,
                             this->shared_from_this(),
                             std::placeholders::_1,
                             std::placeholders::_2));
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // Close the WebSocket connection
    //    ws_.async_close(
    //        websocket::close_code::normal,
    //        beast::bind_executor(&session::on_close, shared_from_this()));
  }

  void on_close(beast::error_code ec) {
    //    if (ec)
    //      return fail(ec, "close");

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    //    std::cout << beast::make_printable(buffer_.data()) << std::endl;
  }

 private:
  asio::io_context ioc_;
  tcp::resolver resolver_;
  websocket::stream<tcp::socket> ws_;
  ssl::context ctx_;
  boost::asio::thread_pool io_pool_;
  beast::flat_buffer buffer_;
  std::string host_;
  std::string text_;
};

#endif  // SRC_COMPONENTS_TRANSPORT_MANAGER_TEST_INCLUDE_TRANSPORT_MANAGER_WEBSOCKET_SERVER_WEBSOCKET_SAMPLE_CLIENT

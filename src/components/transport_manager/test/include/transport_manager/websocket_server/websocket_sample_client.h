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

// class BaseSampleClient {
// public:
//  virtual void run() = 0;
//  virtual void stop() = 0;
//  virtual void set_security_settings() = 0;
//};

using WS = websocket::stream<tcp::socket>;
using WSS = websocket::stream<ssl::stream<tcp::socket> >;

template <typename Stream = WS>
class WSSampleClient
    : public std::enable_shared_from_this<WSSampleClient<Stream> > {
 public:
  WSSampleClient(const std::string& host, const std::string& target);
  ~WSSampleClient() {}

  void stop() {
    ioc_.stop();
    ws_.lowest_layer().close();

    io_pool_.stop();
    io_pool_.join();
  }

  bool run() {
    //    const std::string host = "127.0.0.1";
    const std::string port = "2020";
    //    const std::string path = secure_ ? "wss://" : "ws://";
    const std::string target = target_;
    boost::system::error_code ec;
    ctx_.set_verify_mode(ssl::verify_none);
    std::cout << "KEK!!!!TARGET: " << target_ << std::endl;

    auto results = resolver_.resolve(host_, port, ec);
    if (ec) {
      std::string str_err = "ErrorMessage: " + ec.message();
      std::cout << "KEK!!!!" << str_err << std::endl;
    }

    std::cout << "KEK!!!!RESOLVED!!!!!" << std::endl;

    if (!connect(results)) {
      return false;
    }

    std::cout << "KEK!!!!CONNECTED!!!!!" << std::endl;

    if (!handshake(host_, target_)) {
      return false;
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
    return true;
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
    std::cout << "KEK!!!!on_close" << std::endl;

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    //    std::cout << beast::make_printable(buffer_.data()) << std::endl;
  }

  bool connect(tcp::resolver::results_type& results);

  bool handshake(const std::string& host, const std::string& target);

  void set_security_settings(const std::string& ca_cert_path,
                             const std::string& client_cert_path,
                             const std::string& client_key_path) {
    ctx_.set_options(boost::asio::ssl::context::default_workarounds);
    //    ws_.next_layer().set_verify_mode(ssl::verify_peer);
    ctx_.load_verify_file(ca_cert_path);
    using context = boost::asio::ssl::context_base;
    ctx_.use_certificate_chain_file(client_cert_path);
    ctx_.use_private_key_file(client_key_path, context::pem);
  }

 private:
  asio::io_context ioc_;
  tcp::resolver resolver_;
  ssl::context ctx_;
  Stream ws_;
  //  websocket::stream<ssl::stream<tcp::socket> > wss_;
  boost::asio::thread_pool io_pool_;
  beast::flat_buffer buffer_;
  bool secure_;
  std::string host_;
  std::string target_;
  //  std::string host_;
  //  std::string text_;
};

template <>
WSSampleClient<WS>::WSSampleClient(const std::string& host,
                                   const std::string& target)
    : resolver_(ioc_)
    , ctx_(ssl::context::sslv23_client)
    , ws_(ioc_)
    , secure_(false)
    , host_(host)
    , target_(target) {}

template <>
bool WSSampleClient<WS>::connect(tcp::resolver::results_type& results) {
  boost::system::error_code ec;
  boost::asio::connect(ws_.next_layer(), results.begin(), results.end(), ec);
  if (ec) {
    std::string str_err = "ErrorMessage: " + ec.message();
    std::cout << "KEK!!!!" << str_err << std::endl;
    return false;
  }
  return true;
}

template <>
bool WSSampleClient<WSS>::connect(tcp::resolver::results_type& results) {
  boost::system::error_code ec;
  boost::asio::connect(ws_.lowest_layer(), results.begin(), results.end(), ec);
  if (ec) {
    std::string str_err = "ErrorMessage: " + ec.message();
    std::cout << "KEK!!!!" << str_err << std::endl;
    return false;
  }
  return true;
}

template <>
bool WSSampleClient<WS>::handshake(const std::string& host,
                                   const std::string& target) {
  boost::system::error_code ec;
  ws_.handshake(host, target, ec);
  if (ec) {
    std::string str_err = "ErrorMessage: " + ec.message();
    std::cout << "KEK!!!!" << str_err << std::endl;
    return false;
  }
  return true;
}

template <>
bool WSSampleClient<WSS>::handshake(const std::string& host,
                                    const std::string& target) {
  std::cout << "KEK!!!!SECUREHANDSAKE!!!" << std::endl;
  boost::system::error_code ec;

  ws_.next_layer().handshake(ssl::stream_base::client, ec);
  if (ec) {
    std::string str_err = "ErrorMessage: " + ec.message();
    std::cout << "KEK!!!!" << str_err << std::endl;
    return false;
  }

  ws_.handshake(host, target, ec);
  if (ec) {
    std::string str_err = "ErrorMessage: " + ec.message();
    std::cout << "KEK!!!!" << str_err << std::endl;
    return false;
  }

  return true;
}

template <>
WSSampleClient<websocket::stream<ssl::stream<tcp::socket> > >::WSSampleClient(
    const std::string& host, const std::string& target)
    : resolver_(ioc_)
    , ctx_(ssl::context::sslv23_client)
    , ws_(ioc_, ctx_)
    , io_pool_(1)
    , secure_(true)
    , host_(host)
    , target_(target) {}

// template class WSSampleClient<websocket::stream<tcp::socket> >;
// template class WSSampleClient<websocket::stream<ssl::stream<tcp::socket> > >;

#endif  // SRC_COMPONENTS_TRANSPORT_MANAGER_TEST_INCLUDE_TRANSPORT_MANAGER_WEBSOCKET_SERVER_WEBSOCKET_SAMPLE_CLIENT

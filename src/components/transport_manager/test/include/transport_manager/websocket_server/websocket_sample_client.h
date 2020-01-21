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

using WS = websocket::stream<tcp::socket>;
using WSS = websocket::stream<ssl::stream<tcp::socket> >;

struct SecurityParams {
  std::string ca_cert_;
  std::string client_cert_;
  std::string client_key_;
};

template <typename Stream = WS>
class WSSampleClient
    : public std::enable_shared_from_this<WSSampleClient<Stream> > {
 public:
  WSSampleClient(const std::string& host, const std::string& port);
  WSSampleClient(const std::string& host,
                 const std::string& port,
                 const SecurityParams& params);
  ~WSSampleClient() {}

  bool run() {
    boost::system::error_code ec;
    ctx_.set_verify_mode(ssl::verify_none);

    auto results = resolver_.resolve(host_, port_, ec);
    if (ec) {
      std::string str_err = "ErrorMessage: " + ec.message();
      std::cout << "KEK!!!!" << str_err << std::endl;
    }

    std::cout << "KEK!!!!RESOLVED!!!!!" << std::endl;

    if (!connect(results)) {
      return false;
    }

    std::cout << "KEK!!!!CONNECTED!!!!!" << std::endl;

    if (!handshake(host_, "/")) {
      return false;
    }

    std::cout << "KEK!!!!HANDSHAKE!!!!!" << std::endl;
    ws_->async_read(buffer_,
                    std::bind(&WSSampleClient::on_read,
                              this->shared_from_this(),
                              std::placeholders::_1,
                              std::placeholders::_2));
    std::cout << "KEK!!!!ASYNC_READ!!!!!" << std::endl;
    boost::asio::post(io_pool_, [&]() { ioc_.run(); });
    std::cout << "KEK!!!!POST!!!!!" << std::endl;
    return true;
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
  }

  bool connect(tcp::resolver::results_type& results);

  bool handshake(const std::string& host, const std::string& target);

  void on_handshake_timeout();

  bool is_handshake_successful() const;
  void stop();

 private:
  asio::io_context ioc_;
  tcp::resolver resolver_;
  ssl::context ctx_;
  std::unique_ptr<Stream> ws_;
  boost::asio::thread_pool io_pool_;
  beast::flat_buffer buffer_;
  bool secure_;
  std::string host_;
  std::string port_;
  std::atomic_bool handshake_successful_;
};

template <>
WSSampleClient<WS>::WSSampleClient(const std::string& host,
                                   const std::string& port)
    : resolver_(ioc_)
    , ctx_(ssl::context::sslv23_client)
    , ws_(new WS(ioc_))
    , secure_(false)
    , host_(host)
    , port_(port) {}

template <>
bool WSSampleClient<WS>::connect(tcp::resolver::results_type& results) {
  boost::system::error_code ec;
  boost::asio::connect(ws_->next_layer(), results.begin(), results.end(), ec);
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
  boost::asio::connect(ws_->lowest_layer(), results.begin(), results.end(), ec);
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
  ws_->handshake(host, target, ec);
  if (ec) {
    std::string str_err = "ErrorMessage: " + ec.message();
    std::cout << "KEK!!!!" << str_err << std::endl;
    return false;
  }
  return true;
}

template <>
void WSSampleClient<WS>::stop() {
  ioc_.stop();
  ws_->lowest_layer().close();

  io_pool_.stop();
  io_pool_.join();
}

template <>
bool WSSampleClient<WSS>::handshake(const std::string& host,
                                    const std::string& target) {
  std::cout << "KEK!!!!SECUREHANDSAKE!!!" << std::endl;
  boost::system::error_code ec;

  ws_->next_layer().handshake(ssl::stream_base::client, ec);
  if (ec) {
    std::string str_err = "ErrorMessage: " + ec.message();
    std::cout << "KEK!!!!" << str_err << std::endl;
    return false;
  }

  ws_->handshake(host, target, ec);
  if (ec) {
    std::string str_err = "ErrorMessage: " + ec.message();
    std::cout << "KEK!!!!" << str_err << std::endl;
    return false;
  }

  handshake_successful_ = true;
  return true;
}

template <>
void WSSampleClient<WSS>::stop() {
  ioc_.stop();
  ws_->next_layer().next_layer().shutdown(
      boost::asio::ip::tcp::socket::shutdown_both);
  ws_->lowest_layer().close();

  io_pool_.stop();
  io_pool_.join();
}

template <>
void WSSampleClient<WSS>::on_handshake_timeout() {
  std::cout << "KEK!!!HANDSHAKETIMEOUT!!!" << std::endl;

  if (!handshake_successful_) {
    stop();
  }
}

template <>
bool WSSampleClient<WSS>::is_handshake_successful() const {
  return handshake_successful_;
}

template <>
WSSampleClient<websocket::stream<ssl::stream<tcp::socket> > >::WSSampleClient(
    const std::string& host,
    const std::string& port,
    const SecurityParams& params)
    : resolver_(ioc_)
    , ctx_(ssl::context::sslv23_client)
    , ws_(nullptr)
    , io_pool_(1)
    , secure_(true)
    , host_(host)
    , port_(port)
    , handshake_successful_(false) {
  ctx_.set_verify_mode(ssl::context::verify_peer);
  ctx_.load_verify_file(params.ca_cert_);
  ctx_.use_certificate_chain_file(params.client_cert_);
  ctx_.use_private_key_file(params.client_key_, boost::asio::ssl::context::pem);

  ws_.reset(new WSS(ioc_, ctx_));
}

// template class WSSampleClient<websocket::stream<tcp::socket> >;
// template class WSSampleClient<websocket::stream<ssl::stream<tcp::socket> > >;

#endif  // SRC_COMPONENTS_TRANSPORT_MANAGER_TEST_INCLUDE_TRANSPORT_MANAGER_WEBSOCKET_SERVER_WEBSOCKET_SAMPLE_CLIENT

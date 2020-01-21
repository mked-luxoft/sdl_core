/*
 *
 * Copyright (c) 2019, Livio
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

#include "transport_manager/websocket/websocket_device.h"

#include "utils/logger.h"

namespace transport_manager {
namespace transport_adapter {
CREATE_LOGGERPTR_GLOBAL(logger_, "TransportManager")

WebSocketDevice::WebSocketDevice(const std::string& host,
                                 const std::string& port,
                                 const std::string& name)
    : Device(name, name), host_(host), port_(port) {}

bool WebSocketDevice::IsSameAs(const Device* other) const {
  LOG4CXX_TRACE(logger_, "enter. device: " << other);

  const WebSocketDevice* other_websocket_device =
      dynamic_cast<const WebSocketDevice*>(other);

  if (!other_websocket_device) {
    return false;
  }

  if (GetHost() != other_websocket_device->GetHost()) {
    return false;
  }

  if (GetPort() != other_websocket_device->GetPort()) {
    return false;
  }

  if (GetTarget() != other_websocket_device->GetTarget()) {
    return false;
  }

  return true;
}

ApplicationList WebSocketDevice::GetApplicationList() const {
  return ApplicationList{0};
}

const std::string& WebSocketDevice::GetHost() const {
  return host_;
}

const std::string& WebSocketDevice::GetPort() const {
  return port_;
}

const std::string WebSocketDevice::GetTarget() const {
  return host_ + port_ + name();
}

}  // namespace transport_adapter
}  // namespace transport_manager

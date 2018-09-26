/*
 Copyright (c) 2018, Ford Motor Company
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the
 distribution.

 Neither the name of the Ford Motor Company nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include "rc_rpc_plugin/rc_app_extension.h"
#include "smart_objects/smart_object.h"
#include "rc_rpc_plugin/rc_rpc_plugin.h"

CREATE_LOGGERPTR_GLOBAL(logger_, "RCAppExtension")

namespace rc_rpc_plugin {

namespace strings = application_manager::strings;

RCAppExtension::RCAppExtension(application_manager::AppExtensionUID uid,
                               RCRPCPlugin* plugin,
                               application_manager::Application& app)
    : AppExtension(uid), plugin_(plugin), app_(app) {}

void RCAppExtension::SubscribeToInteriorVehicleData(
    const std::string& module_type) {
  subscribed_interior_vehicle_data_.insert(module_type);
}

void RCAppExtension::UnsubscribeFromInteriorVehicleData(
    const std::string& module_type) {
  subscribed_interior_vehicle_data_.erase(module_type);
}

void RCAppExtension::UnsubscribeFromInteriorVehicleData() {
  subscribed_interior_vehicle_data_.clear();
}

bool RCAppExtension::IsSubscibedToInteriorVehicleData(
    const std::string& module_type) {
  std::set<std::string>::iterator it =
      subscribed_interior_vehicle_data_.find(module_type);

  return (it != subscribed_interior_vehicle_data_.end());
}

void RCAppExtension::SaveResumptionData(
    smart_objects::SmartObject& resumption_data) {
  const char* application_interior_data = "moduleData";
  resumption_data[application_interior_data] =
      smart_objects::SmartObject(smart_objects::SmartType_Array);
  int i = 0;
  for (const auto& module_type : subscribed_interior_vehicle_data_) {
    resumption_data[application_interior_data][i++] = module_type;
  }
}

void RCAppExtension::ProcessResumption(
    const smart_objects::SmartObject& saved_app,
    resumption::Subscriber subscriber) {
  if (!saved_app.keyExists(strings::application_subscriptions)) {
    LOG4CXX_DEBUG(logger_, "application_subscriptions section is not exists");
    return;
  }

  const smart_objects::SmartObject& resumption_data =
      saved_app[strings::application_subscriptions];

  const char* application_interior_data = "moduleData";
  if (resumption_data.keyExists(application_interior_data)) {
    const smart_objects::SmartObject& interior_data_subscriptions =
        resumption_data[application_interior_data];
    for (size_t i = 0; i < interior_data_subscriptions.length(); ++i) {
      const std::string module_type =
          static_cast<std::string>((resumption_data[i]).asString());
      SubscribeToInteriorVehicleData(module_type);
    }
    plugin_->ProcessResumptionSubscription(app_, *this, subscriber);
  }
}

void RCAppExtension::RevertResumption(
    const smart_objects::SmartObject& subscriptions) {}

std::set<std::string> RCAppExtension::Subscriptions() const {
  return subscribed_interior_vehicle_data_;
}

RCAppExtension::~RCAppExtension() {}
}  //  namespace rc_rpc_plugin

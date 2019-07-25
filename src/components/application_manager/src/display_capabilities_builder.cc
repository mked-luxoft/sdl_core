#include "application_manager/display_capabilities_builder.h"
#include "application_manager/message_helper.h"
#include "application_manager/smart_object_keys.h"
namespace application_manager {
CREATE_LOGGERPTR_GLOBAL(logger_, "DisplayCapabilitiesBuilder")
DisplayCapabilitiesBuilder::DisplayCapabilitiesBuilder() {
  LOG4CXX_AUTO_TRACE(logger_);
  display_capabilities_ = std::make_shared<smart_objects::SmartObject>();
}

DisplayCapabilitiesBuilder::DisplayCapabilitiesBuilder(
    const smart_objects::SmartObject& display_capabilities) {
  LOG4CXX_AUTO_TRACE(logger_);
  display_capabilities_ =
      std::make_shared<smart_objects::SmartObject>(display_capabilities);
}
void DisplayCapabilitiesBuilder::UpdateDisplayCapabilities(
    const smart_objects::SmartObject& incoming_display_capabilities) {
  LOG4CXX_AUTO_TRACE(logger_);
  smart_objects::SmartObject current_window_capabilities =
      (*display_capabilities_)["windowCapabilities"];
  DCHECK(current_window_capabilities.getType() ==
         smart_objects::SmartType_Array);
  const auto incoming_window_capabilities =
      incoming_display_capabilities["windowCapabilities"];
  for (size_t i; i < incoming_window_capabilities.length(); ++i) {
    current_window_capabilities[current_window_capabilities.length()] =
        incoming_window_capabilities[i];
  }

  *display_capabilities_ = incoming_display_capabilities;
  (*display_capabilities_)["windowCapabilities"] = current_window_capabilities;

  MessageHelper::PrintSmartObject(*display_capabilities_);
}

const smart_objects::SmartObjectSPtr
DisplayCapabilitiesBuilder::display_capabilities() const {
  LOG4CXX_AUTO_TRACE(logger_);
  return display_capabilities_;
}

void DisplayCapabilitiesBuilder::Clear() {
  LOG4CXX_AUTO_TRACE(logger_);
  display_capabilities_.reset();
}
}  // namespace application_manager

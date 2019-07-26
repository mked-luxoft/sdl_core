#include "application_manager/display_capabilities_builder.h"
#include "application_manager/message_helper.h"
#include "application_manager/smart_object_keys.h"
namespace application_manager {
CREATE_LOGGERPTR_GLOBAL(logger_, "DisplayCapabilitiesBuilder")
DisplayCapabilitiesBuilder::DisplayCapabilitiesBuilder(Application& application)
    : owner_(application) {
  LOG4CXX_AUTO_TRACE(logger_);
  display_capabilities_ = std::make_shared<smart_objects::SmartObject>(
      smart_objects::SmartType_Map);
}

void DisplayCapabilitiesBuilder::InitBuilder(
    DisplayCapabilitiesBuilder::ResumeCallback resume_callback,
    const smart_objects::SmartObject& windows_info) {
  LOG4CXX_AUTO_TRACE(logger_);
  resume_callback_ = resume_callback;
  for (size_t i = 0; i < windows_info.length(); ++i) {
    auto window_id = windows_info[i][strings::window_id].asInt();
    LOG4CXX_DEBUG(logger_,
                  "Inserting " << window_id << " to waiting container");
    window_ids_to_resume_.insert(window_id);
  }
}

void DisplayCapabilitiesBuilder::UpdateDisplayCapabilities(
    const smart_objects::SmartObject& incoming_display_capabilities) {
  LOG4CXX_AUTO_TRACE(logger_);

  if (display_capabilities_->empty()) {
    LOG4CXX_DEBUG(logger_,
                  "Current display capability is empty, taking incoming");
    for (size_t i = 0;
         i < incoming_display_capabilities[0][strings::window_capabilities]
                 .length();
         ++i) {
      auto window_id =
          incoming_display_capabilities[0][strings::window_capabilities][i]
                                       [strings::window_id]
                                           .asInt();
      if (window_ids_to_resume_.end() !=
          window_ids_to_resume_.find(window_id)) {
        LOG4CXX_DEBUG(logger_, "STOP WAITING FOR: " << window_id);
        window_ids_to_resume_.erase(window_id);
      }
    }
    *display_capabilities_ = incoming_display_capabilities;
    //    MessageHelper::PrintSmartObject(*display_capabilities_);
    return;
  }

  auto lol = (*display_capabilities_)[0][strings::window_capabilities];
  const auto& kek =
      incoming_display_capabilities[0][strings::window_capabilities];
  *display_capabilities_ = incoming_display_capabilities;

  //  LOG4CXX_DEBUG(logger_, "BEFORE KEK");
  //  MessageHelper::PrintSmartObject(*display_capabilities_);

  auto is_waiting_for_window_id = [&lol, this](const WindowID window_id) {
    for (size_t i = 0; i < lol.length(); ++i) {
      if (window_ids_to_resume_.end() !=
          window_ids_to_resume_.find(window_id)) {
        return true;
      }
    }
    return false;
  };

  for (size_t i = 0; i < kek.length(); ++i) {
    const auto& window_id = kek[i][strings::window_id].asInt();
    if (is_waiting_for_window_id(window_id)) {
      lol[lol.length()] = kek[i];
      LOG4CXX_DEBUG(logger_, "STOP WAITING FOR: " << window_id);
      window_ids_to_resume_.erase(window_id);
    }
  }

  (*display_capabilities_)[0][strings::window_capabilities] = lol;

  if (window_ids_to_resume_.empty()) {
    LOG4CXX_DEBUG(logger_, "TRIGERRING NOTIFICATION");
    resume_callback_(owner_, *display_capabilities_);
  }

  //  LOG4CXX_DEBUG(logger_, "AFTER KEK, BUT BEFORE ASSIGNMENT");
  //  MessageHelper::PrintSmartObject(*display_capabilities_);
  //  LOG4CXX_DEBUG(logger_, "PRINT LOL");
  //  MessageHelper::PrintSmartObject(lol);

  //  LOG4CXX_DEBUG(logger_, "AFTER KEK");
  //  MessageHelper::PrintSmartObject(*display_capabilities_);

  //  MessageHelper::PrintSmartObject(incoming_display_capabilities);
  //  MessageHelper::PrintSmartObject(*display_capabilities_);
  //  smart_objects::SmartObject current_window_capabilities =
  //      (*display_capabilities_)["windowCapabilities"];
  //  LOG4CXX_DEBUG(logger_, "LOLKEK1");
  //  LOG4CXX_DEBUG(
  //      logger_,
  //      "incoming_widow_capabilities type: "
  //          << incoming_display_capabilities["windowCapabilities"].getType());
  //  auto& incoming_window_capabilities =
  //      incoming_display_capabilities["windowCapabilities"];
  //  LOG4CXX_DEBUG(logger_, "LOLKEK2");
  //  for (size_t i; i < incoming_window_capabilities.length(); ++i) {
  //    LOG4CXX_DEBUG(logger_, "LOLKEK!!!");
  //    LOG4CXX_DEBUG(logger_,
  //                  "WINDOW CAP TYPE: " <<
  //                  current_window_capabilities.getType());
  //    current_window_capabilities[current_window_capabilities.length()] =
  //        incoming_window_capabilities[i];
  //}

  //  (*display_capabilities_) = incoming_display_capabilities;
  //  LOG4CXX_DEBUG(logger_, "LOLKEK3");
  //  (*display_capabilities_)["windowCapabilities"] =
  //  current_window_capabilities; LOG4CXX_DEBUG(logger_, "LOLKEK4");

  //  MessageHelper::PrintSmartObject(*display_capabilities_);
}  // namespace application_manager

const smart_objects::SmartObjectSPtr
DisplayCapabilitiesBuilder::display_capabilities() const {
  LOG4CXX_AUTO_TRACE(logger_);
  return display_capabilities_;
}

void DisplayCapabilitiesBuilder::ResetSDisplayCapabilities() {
  LOG4CXX_AUTO_TRACE(logger_);
  display_capabilities_.reset();
  display_capabilities_ = std::make_shared<smart_objects::SmartObject>(
      smart_objects::SmartType_Map);
}

void DisplayCapabilitiesBuilder::AddWindowIDToResume(const WindowID window_id) {
  LOG4CXX_AUTO_TRACE(logger_);

  window_ids_to_resume_.insert(window_id);
}
}  // namespace application_manager

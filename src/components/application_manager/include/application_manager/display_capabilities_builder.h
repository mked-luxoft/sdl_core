#ifndef SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_APPLICATION_MANAGER_DISPLAY_CAPABILITIES_BUILDER_H_
#define SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_APPLICATION_MANAGER_DISPLAY_CAPABILITIES_BUILDER_H_
#include "application_manager/application.h"

namespace application_manager {
class DisplayCapabilitiesBuilder {
  typedef std::function<void(Application&, const smart_objects::SmartObject)>
      ResumeCallback;

 public:
  DisplayCapabilitiesBuilder(Application& application);
  void InitBuilder(ResumeCallback resume_callback_,
                   const smart_objects::SmartObject& windows_info);
  void UpdateDisplayCapabilities(
      const smart_objects::SmartObject& display_capabilities);

  void ResetSDisplayCapabilities();
  void AddWindowIDToResume(const WindowID window_id);

  const smart_objects::SmartObjectSPtr display_capabilities() const;

 private:
  smart_objects::SmartObjectSPtr display_capabilities_;
  typedef std::set<WindowID> WindowIDsToResume;
  WindowIDsToResume window_ids_to_resume_;
  Application& owner_;

  ResumeCallback resume_callback_;
};
}  // namespace application_manager
#endif

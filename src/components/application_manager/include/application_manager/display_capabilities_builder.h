#ifndef SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_APPLICATION_MANAGER_DISPLAY_CAPABILITIES_BUILDER_H_
#define SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_APPLICATION_MANAGER_DISPLAY_CAPABILITIES_BUILDER_H_
#include "application_manager/application.h"

namespace application_manager {
class DisplayCapabilitiesBuilder {
 public:
  DisplayCapabilitiesBuilder();
  DisplayCapabilitiesBuilder(
      const smart_objects::SmartObject& display_capabilities);
  void UpdateDisplayCapabilities(
      const smart_objects::SmartObject& display_capabilities);

  void Clear();

  const smart_objects::SmartObjectSPtr display_capabilities() const;

 private:
  smart_objects::SmartObjectSPtr display_capabilities_;
};
}  // namespace application_manager
#endif

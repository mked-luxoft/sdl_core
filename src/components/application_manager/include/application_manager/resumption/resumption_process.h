#ifndef SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_APPLICATION_MANAGER_RESUMPTION_RESUMPTION_PROCESS_
#define SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_APPLICATION_MANAGER_RESUMPTION_RESUMPTION_PROCESS_

#include <map>
#include <vector>
#include <functional>

#include "smart_objects/smart_object.h"
#include "application_manager/application.h"
#include "application_manager/event_engine/event_observer.h"

namespace resumption {

namespace app_mngr = application_manager;

struct ResumptionRequest {
  hmi_apis::FunctionID::eType function_id;
  int32_t correlation_id;
  smart_objects::SmartObject message;
};

struct ResumptionStatus {
  std::vector<ResumptionRequest> list_of_sent_requests;
  std::vector<ResumptionRequest> erro_requests;
  std::vector<ResumptionRequest> successful_requests;
};

class ResumptionProcess : public app_mngr::event_engine::EventObserver {
 public:
  explicit ResumptionProcess(
      application_manager::ApplicationManager& application_manager);
  ~ResumptionProcess();

  void Restore(app_mngr::ApplicationSharedPtr application,
               smart_objects::SmartObject saved_app);

  /**
   * @brief Event, that raised if application get resumption response from HMI
   * @param event : event object, that contains smart_object with HMI message
   */
  void on_event(const app_mngr::event_engine::Event& event) OVERRIDE;

 private:
  void RevertRestoredData(const int32_t app_id);

  void WaitForResponse(const int32_t app_id, const ResumptionRequest request);

  /**
   * @brief Process specified HMI request
   * @param request Request to process
   * @param use_events Process request events or not flag
   * @return TRUE on success, otherwise FALSE
   */
  bool ProcessHMIRequest(smart_objects::SmartObjectSPtr request = NULL,
                         bool use_events = false);

  /**
   * @brief Process list of HMI requests using ProcessHMIRequest method
   * @param requests List of requests to process
   */
  void ProcessHMIRequests(const smart_objects::SmartObjectList& requests);

  /**
   * @brief AddFiles allows to add files for the application
   * which should be resumed
   * @param application application which will be resumed
   * @param saved_app application specific section from backup file
   */
  void AddFiles(app_mngr::ApplicationSharedPtr application,
                const smart_objects::SmartObject& saved_app);

  void DeleteFiles(const int32_t app_id);

  /**
  * @brief AddSubmenues allows to add sub menues for the application
  * which should be resumed
  * @param application application which will be resumed
  * @param saved_app application specific section from backup file
  */
  void AddSubmenues(app_mngr::ApplicationSharedPtr application,
                    const smart_objects::SmartObject& saved_app);

  void DeleteSubmenues(const int32_t app_id);

  /**
  * @brief AddCommands allows to add commands for the application
  * which should be resumed
  * @param application application which will be resumed
  * @param saved_app application specific section from backup file
  */
  void AddCommands(app_mngr::ApplicationSharedPtr application,
                   const smart_objects::SmartObject& saved_app);

  void DeleteCommands(const int32_t app_id);
  void DeleteUICommands(const ResumptionRequest& request);
  void DeleteVRCommands(const ResumptionRequest& request);


  /**
  * @brief AddChoicesets allows to add choice sets for the application
  * which should be resumed
  * @param application application which will be resumed
  * @param saved_app application specific section from backup file
  */
  void AddChoicesets(app_mngr::ApplicationSharedPtr application,
                     const smart_objects::SmartObject& saved_app);

  void DeleteChoicesets(const int32_t app_id);

  /**
  * @brief SetGlobalProperties allows to restore global properties.
  * @param application application which will be resumed
  * @param saved_app application specific section from backup file
  */
  void SetGlobalProperties(app_mngr::ApplicationSharedPtr application,
                           const smart_objects::SmartObject& saved_app);

  void DeleteGlobalProperties(const int32_t app_id);

  /**
  * @brief AddSubscriptions allows to restore subscriptions
  * @param application application which will be resumed
  * @param saved_app application specific section from backup file
  */
  void AddSubscriptions(app_mngr::ApplicationSharedPtr application,
                        const smart_objects::SmartObject& saved_app);

  void DeleteSubscriptions(const int32_t app_id);

  /**
  * @brief AddWayPointsSubscription allows to restore subscription
  * for WayPoints
  * @param application application which will be resumed
  * @param saved_app application specific section from backup file
  */
  void AddWayPointsSubscription(app_mngr::ApplicationSharedPtr application,
                                const smart_objects::SmartObject& saved_app);

  void DeleteWayPointsSubscription(const int32_t app_id);

  app_mngr::ApplicationManager& application_manager_;
  std::map<std::int32_t, ResumptionStatus> resumption_status_;
};

}  // namespace resumption

#endif  // SRC_COMPONENTS_APPLICATION_MANAGER_INCLUDE_APPLICATION_MANAGER_RESUMPTION_RESUMPTION_PROCESS_

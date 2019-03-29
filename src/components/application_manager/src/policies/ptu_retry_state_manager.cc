#include "ptu_retry_state_manager.h"

CREATE_LOGGERPTR_GLOBAL(logger_, "PTURetryStateManager");

PTURetryStateManager::PTURetryStateManager(PolicyManager& policy_manager)
    : policy_manager_(policy_manager)
    , ptu_retry_state(kStateNone)
    , current_ptu_retry_index_(0) {}

bool PTURetryStateManager::IsAllowedRetryCountExceeded() const {
  LOG4CXX_AUTO_TRACE(logger_);

  return current_ptu_retry_index_ > policy_manager_.allowed_ptu_retries_count();
}

void PTURetryStateManager::ProcessCurrentState() {
  LOG4CXX_AUTO_TRACE(logger_);

  if (IsAllowedPTURetryCountExceeded()) {
    current_ptu_retry_state_ = kStateExceededRetryCount;
    current_ptu_retry_index_ = 0;
    policy_manager_.ResetRetrySequence(true);
  }
}

void PTURetryStateManager::OnRetrySequenceStarted() {
  LOG4CXX_AUTO_TRACE(logger_);
  ++current_ptu_retry_index_;
  current_ptu_retry_state_ = kStateUpdating;
}

void PTURetryStateManager::OnSuccessfulPTU() {
  LOG4CXX_AUTO_TRACE(logger_);
  current_ptu_retry_state_ = kStateUpToDate;
  current_ptu_retry_index_ = 0;
}

void PTURetryStateManager::OnInvalidPTReceiced() {
  LOG4CXX_AUTO_TRACE(logger_);

  current_ptu_retry_state_ = kStateWrongUpdateReceived;
}

void PTURetryStateManager::OnHMITimerTimedOut() {
  LOG4CXX_AUTO_TRACE(logger_);
}

void PTURetryStateManager::OnMobileTimerTimedOut() {
  LOG4CXX_AUTO_TRACE(logger_);
}

const PTURetryState PTURetryStateManager::current_state() const {
  LOG4CXX_AUTO_TRACE(logger_);

  return current_ptu_retry_state_;
}
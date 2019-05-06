#include "policy/ptu_retry_handler.h"

namespace policy {

class MockPTURetryHandler : public PTURetryHandler {
 public:
  MOCK_CONST_METHOD0(IsAllowedRetryCountExceeded, bool());
  MOCK_METHOD0(OnSystemRequestReceived, void());
  MOCK_METHOD0(RetrySequenceFailed, void());
};
}
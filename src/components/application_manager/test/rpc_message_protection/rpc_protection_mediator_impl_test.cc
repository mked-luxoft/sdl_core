#include "gtest/gtest.h"
#include "application_manager/rpc_protection_mediator_impl.h"
#include "application_manager/mock_application_manager.h"
#include "application_manager/policies/mock_policy_handler_interface.h"
#include "application_manager/mock_application.h"
#include "application_manager/policies/mock_rpc_encryption_manager.h"
#include "policy/policy_table/types.h"
#include "policy/policy_table/enums.h"
#include "policy/mock_policy_manager.h"
#include "rpc_base/rpc_base.h"
#include "utils/optional.h"

namespace policy_type = rpc::policy_table_interface_base;
namespace test {
namespace components {
namespace rpc_protection_mediator_test {

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;

const uint32_t kRAIFid =
    static_cast<uint32_t>(policy_table::FunctionID::RegisterAppInterfaceID);

const std::string kRAIPolicyFid = "RegisterAppInterface";
const std::string kRPCPolicyFid1 = "kRPCPolicyFid1";
const std::string kRPCPolicyFid2 = "kRPCPolicyFid2";
const std::string kRPCPolicyFid3 = "kRPCPolicyFid3";
const std::string kRPCPolicyFid4 = "kRPCPolicyFid4";
const std::string kRPCPolicyFid5 = "kRPCPolicyFid5";
const std::string kRPCPolicyFid6 = "kRPCPolicyFid6";

const std::string kProtectedFunctionGroupWithException =
    "kProtectedFunctionGroupWithException";
const std::string kProtectedFunctionGroup = "kProtectedFunctionGroup";
const std::string kUnprotectedFunctionGroup1 = "kUnrotectedFunctionGroup1";
const std::string kUnprotectedFunctionGroup2 = "kUnrotectedFunctionGroup2";

const bool kRPCServiceSecure = true;
const bool kAppNeedsEncryption = true;

typedef std::vector<std::string> FunctionGroup;
typedef std::map<std::string, FunctionGroup> FunctionGroups;

FunctionGroups function_groups;

enum ApplicationStubType {
  kProtectedAppProtectedRPC,
  kProtectedAppUnProtectedRPC,
  kProtectedAppExceptionRPC,
  kProtectedAppNotExceptionRPC,
  kUnProtectedAppProtectedRPC,
  kUnProtectedAppUnProtectedRPC,
  kProtectedAppWithMultipleGroups,
  kProtectedAppWithSameRPCInProtectedAndUnprotectedGroups
};

struct ApplicationStub {
  bool is_protected_;
  policy_type::Strings groups_;
  application_manager_test::MockApplication* mock_application_;

  ApplicationStub(const bool is_protected, const policy_type::Strings& groups)
      : is_protected_(is_protected), groups_(groups) {
    mock_application_ = new application_manager_test::MockApplication();
  }
};

ApplicationStub GetApplicationStub(ApplicationStubType type) {
  switch (type) {
    case ApplicationStubType::kProtectedAppProtectedRPC: {
      policy_type::Strings groups;
      groups.push_back(kProtectedFunctionGroup);
      return ApplicationStub(kAppNeedsEncryption, groups);
    }
    case ApplicationStubType::kProtectedAppUnProtectedRPC: {
      policy_type::Strings groups;
      groups.push_back(kUnprotectedFunctionGroup1);
      return ApplicationStub(kAppNeedsEncryption, groups);
    }
    case ApplicationStubType::kProtectedAppExceptionRPC: {
      policy_type::Strings groups;
      groups.push_back(kProtectedFunctionGroupWithException);
      return ApplicationStub(kAppNeedsEncryption, groups);
    }
    case ApplicationStubType::kProtectedAppNotExceptionRPC: {
      policy_type::Strings groups;
      groups.push_back(kProtectedFunctionGroup);
      return ApplicationStub(kAppNeedsEncryption, groups);
    }
    case ApplicationStubType::kUnProtectedAppProtectedRPC: {
      policy_type::Strings groups;
      groups.push_back(kProtectedFunctionGroup);
      return ApplicationStub(!kAppNeedsEncryption, groups);
    }
    case ApplicationStubType::kUnProtectedAppUnProtectedRPC: {
      policy_type::Strings groups;
      groups.push_back(kUnprotectedFunctionGroup1);
      return ApplicationStub(!kAppNeedsEncryption, groups);
    }
    case ApplicationStubType::
        kProtectedAppWithSameRPCInProtectedAndUnprotectedGroups: {
      policy_type::Strings groups;
      groups.push_back(kProtectedFunctionGroupWithException);
      groups.push_back(kUnprotectedFunctionGroup1);
      return ApplicationStub(kAppNeedsEncryption, groups);
    }
    case ApplicationStubType::kProtectedAppWithMultipleGroups: {
      policy_type::Strings groups;
      groups.push_back(kProtectedFunctionGroupWithException);
      groups.push_back(kProtectedFunctionGroup);
      return ApplicationStub(kAppNeedsEncryption, groups);
    }
    default:
      return ApplicationStub(!kAppNeedsEncryption, policy_type::Strings{});
  }
}

struct RPCProtectionMediatorImplTestHelper {
  utils::Optional<const uint32_t> function_id_;
  std::string policy_function_id_;
  ApplicationStub application_stub_;
  bool is_service_secure_;
  bool expected_does_rpc_need_encryption_;

  RPCProtectionMediatorImplTestHelper(
      const std::string policy_function_id,
      const bool is_service_secure,
      const ApplicationStub& application_stub,
      const bool expected_does_rpc_need_encryption)
      : function_id_(utils::Optional<uint32_t>::OptionalEmpty::EMPTY)
      , policy_function_id_(policy_function_id)
      , application_stub_(application_stub)
      , is_service_secure_(is_service_secure)
      , expected_does_rpc_need_encryption_(expected_does_rpc_need_encryption) {}

  RPCProtectionMediatorImplTestHelper(
      const uint32_t& function_id,
      const std::string policy_function_id,
      const bool is_service_secure,
      const ApplicationStub& application_stub,
      const bool expected_does_rpc_need_encryption)
      : function_id_(function_id)
      , policy_function_id_(policy_function_id)
      , application_stub_(application_stub)
      , is_service_secure_(is_service_secure)
      , expected_does_rpc_need_encryption_(expected_does_rpc_need_encryption) {}
};

class RPCProtectionMediatorImplTest
    : public ::testing::TestWithParam<RPCProtectionMediatorImplTestHelper> {
 protected:
  policy_test::MockPolicyHandlerInterface mock_policy_handler_;
  application_manager_test::MockRPCEncryptionManagerInterface
      rpc_encryption_manager_;

  std::shared_ptr<application_manager::RPCProtectionMediator>
      rpc_protection_mediator_;

  RPCProtectionMediatorImplTest() {
    rpc_protection_mediator_ =
        std::make_shared<application_manager::RPCProtectionMediatorImpl>(
            mock_policy_handler_);
  }

  void SetUp() OVERRIDE {
    function_groups[kProtectedFunctionGroupWithException] = {kRAIPolicyFid,
                                                             kRPCPolicyFid1};
    function_groups[kUnprotectedFunctionGroup1] = {kRPCPolicyFid1,
                                                   kRPCPolicyFid2};
    function_groups[kUnprotectedFunctionGroup2] = {kRPCPolicyFid3,
                                                   kRPCPolicyFid4};
    function_groups[kProtectedFunctionGroup] = {kRPCPolicyFid5, kRPCPolicyFid6};

    ON_CALL(rpc_encryption_manager_, GetPolicyFunctionName(kRAIFid))
        .WillByDefault(Return(kRAIPolicyFid));
    ON_CALL(mock_policy_handler_, RPCEncryptionManager())
        .WillByDefault(ReturnRef(rpc_encryption_manager_));
    ON_CALL(rpc_encryption_manager_,
            GetRPCsForGroup(kProtectedFunctionGroupWithException))
        .WillByDefault(
            Return(function_groups[kProtectedFunctionGroupWithException]));
    ON_CALL(rpc_encryption_manager_, GetRPCsForGroup(kProtectedFunctionGroup))
        .WillByDefault(Return(function_groups[kProtectedFunctionGroup]));
    ON_CALL(rpc_encryption_manager_,
            GetRPCsForGroup(kUnprotectedFunctionGroup1))
        .WillByDefault(Return(function_groups[kUnprotectedFunctionGroup1]));
    ON_CALL(rpc_encryption_manager_,
            GetRPCsForGroup(kUnprotectedFunctionGroup2))
        .WillByDefault(Return(function_groups[kUnprotectedFunctionGroup2]));
    ON_CALL(rpc_encryption_manager_,
            GroupNeedEncryption(kProtectedFunctionGroup))
        .WillByDefault(Return(true));
    ON_CALL(rpc_encryption_manager_,
            GroupNeedEncryption(kUnprotectedFunctionGroup1))
        .WillByDefault(Return(false));
    ON_CALL(rpc_encryption_manager_,
            GroupNeedEncryption(kUnprotectedFunctionGroup2))
        .WillByDefault(Return(false));
    ON_CALL(rpc_encryption_manager_,
            GroupNeedEncryption(kProtectedFunctionGroupWithException))
        .WillByDefault(Return(true));
  }
};

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionUnprotectedFidSecureService_ExpectFalse,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid3,
        kRPCServiceSecure,
        GetApplicationStub(ApplicationStubType::kProtectedAppUnProtectedRPC),
        false)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionProtectedFidSecureService_ExpectTrue,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid5,
        kRPCServiceSecure,
        GetApplicationStub(ApplicationStubType::kProtectedAppProtectedRPC),
        true)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionUnprotectedFidUnsecureService_ExpectFalse,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid3,
        !kRPCServiceSecure,
        GetApplicationStub(ApplicationStubType::kProtectedAppUnProtectedRPC),
        false)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionProtectedFidUnsecureService_ExpectFalse,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid1,
        !kRPCServiceSecure,
        GetApplicationStub(ApplicationStubType::kProtectedAppProtectedRPC),
        false)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionUnprotectedAppProtectedGroup_ExtectFalse,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid1,
        kRPCServiceSecure,
        GetApplicationStub(ApplicationStubType::kUnProtectedAppProtectedRPC),
        false)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionUnprotectedAppUnprotectedGroup_ExpectFalse,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid1,
        kRPCServiceSecure,
        GetApplicationStub(ApplicationStubType::kUnProtectedAppUnProtectedRPC),
        false)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionExceptionRPCUnsecureService_ExpectFalse,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRAIFid,
        kRAIPolicyFid,
        !kRPCServiceSecure,
        GetApplicationStub(ApplicationStubType::kProtectedAppExceptionRPC),
        false)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionNotExceptionUnprotecedRPCUnsecureService_ExpectFalse,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid3,
        !kRPCServiceSecure,
        GetApplicationStub(ApplicationStubType::kProtectedAppUnProtectedRPC),
        false)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionProtectedAppProtectedRPCIn1Of2Groups_ExpectTrue,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid1,
        kRPCServiceSecure,
        GetApplicationStub(
            ApplicationStubType::
                kProtectedAppWithSameRPCInProtectedAndUnprotectedGroups),
        true)));

INSTANTIATE_TEST_CASE_P(
    DoesRPCNeedEncryptionProtectedAppProtectedRPCMultipleGroups_ExpectTrue,
    RPCProtectionMediatorImplTest,
    ::testing::Values(RPCProtectionMediatorImplTestHelper(
        kRPCPolicyFid5,
        kRPCServiceSecure,
        GetApplicationStub(
            ApplicationStubType::kProtectedAppWithMultipleGroups),
        true)));

TEST_P(RPCProtectionMediatorImplTest, DoesRPCNeedEncryption) {
  const auto& function_id =
      GetParam().function_id_ ? *(GetParam().function_id_) : 0;
  const auto& policy_function_id = GetParam().policy_function_id_;
  const auto& is_service_secure = GetParam().is_service_secure_;
  const auto& app_stub = GetParam().application_stub_;
  auto mock_application =
      std::shared_ptr<application_manager_test::MockApplication>(
          app_stub.mock_application_);
  const auto& expected_does_rpc_need_encryption =
      GetParam().expected_does_rpc_need_encryption_;

  ON_CALL(rpc_encryption_manager_, GetPolicyFunctionName(_))
      .WillByDefault(Return(policy_function_id));

  ON_CALL(*mock_application, policy_app_id()).WillByDefault(Return(""));

  ON_CALL(rpc_encryption_manager_, AppNeedEncryption(_))
      .WillByDefault(Return(app_stub.is_protected_));

  ON_CALL(rpc_encryption_manager_, GetGroupsForApp(_))
      .WillByDefault(ReturnRef(app_stub.groups_));

  auto does_rpc_need_encryption =
      rpc_protection_mediator_->DoesRPCNeedEncryption(
          function_id, mock_application, 0, is_service_secure);

  EXPECT_EQ(expected_does_rpc_need_encryption, does_rpc_need_encryption);
}
}
}
}

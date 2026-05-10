/**
 *
 *  @file test_cli_result.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/kordexjs/cli
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the LICENSE file.
 *
 *  Kordex CLI
 *
 */

#include <string_view>
#include <kordex/cli/CliResult.hpp>

namespace
{
  [[nodiscard]] bool expect_true(
      bool condition,
      const char *message)
  {
    if (!condition)
    {
      (void)message;
      return false;
    }

    return true;
  }

  [[nodiscard]] bool test_success_result()
  {
    const auto result = kordex::cli::CliResult::success("done");

    return expect_true(
               result.succeeded(),
               "success result should succeed") &&
           expect_true(
               !result.failed(),
               "success result should not fail") &&
           expect_true(
               result.status == kordex::cli::CliExitStatus::Success,
               "success status should match") &&
           expect_true(
               result.exit_code == 0,
               "success exit code should be zero") &&
           expect_true(
               !result.has_error(),
               "success result should not contain error") &&
           expect_true(
               result.has_output(),
               "success result should contain output") &&
           expect_true(
               ::std::string_view(result.output) == "done",
               "success output should match");
  }

  [[nodiscard]] bool test_success_empty_output()
  {
    const auto result = kordex::cli::CliResult::success();

    return expect_true(
               result.succeeded(),
               "empty success should succeed") &&
           expect_true(
               !result.has_output(),
               "empty success should not have output") &&
           expect_true(
               !result.has_error_output(),
               "empty success should not have error output");
  }

  [[nodiscard]] bool test_failure_result()
  {
    const auto error = kordex::cli::make_cli_error(
        kordex::cli::CliErrorCode::CommandNotFound,
        "command missing");

    const auto result = kordex::cli::CliResult::failure(error, 2);

    return expect_true(
               !result.succeeded(),
               "failure result should not succeed") &&
           expect_true(
               result.failed(),
               "failure result should fail") &&
           expect_true(
               result.status == kordex::cli::CliExitStatus::Failed,
               "failure status should match") &&
           expect_true(
               result.exit_code == 2,
               "failure exit code should match") &&
           expect_true(
               result.has_error(),
               "failure should contain error") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::NotFound,
               "failure error code should match") &&
           expect_true(
               ::std::string_view(result.error.message()) == "command missing",
               "failure error message should match");
  }

  [[nodiscard]] bool test_failure_normalizes_zero_exit_code()
  {
    const auto error = kordex::cli::make_cli_error(
        kordex::cli::CliErrorCode::InvalidArgument,
        "bad argument");

    const auto result = kordex::cli::CliResult::failure(error, 0);

    return expect_true(
               result.failed(),
               "failure should fail") &&
           expect_true(
               result.exit_code == 1,
               "zero failure exit code should normalize to one") &&
           expect_true(
               result.has_error(),
               "failure should contain error");
  }

  [[nodiscard]] bool test_failure_with_ok_error_creates_internal_error()
  {
    const auto result = kordex::cli::CliResult::failure(
        kordex::cli::ok(),
        1);

    return expect_true(
               result.failed(),
               "failure should fail") &&
           expect_true(
               result.has_error(),
               "failure with ok error should create an error") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::InternalError,
               "fallback error should be internal error");
  }

  [[nodiscard]] bool test_help_result()
  {
    const auto result = kordex::cli::CliResult::help("usage: kordex");

    return expect_true(
               result.help_requested(),
               "help result should mark help requested") &&
           expect_true(
               !result.succeeded(),
               "help result should not be command success") &&
           expect_true(
               !result.failed(),
               "help result should not fail") &&
           expect_true(
               result.exit_code == 0,
               "help exit code should be zero") &&
           expect_true(
               !result.has_error(),
               "help result should not contain error") &&
           expect_true(
               result.has_output(),
               "help result should contain output") &&
           expect_true(
               ::std::string_view(result.output) == "usage: kordex",
               "help output should match");
  }

  [[nodiscard]] bool test_version_result()
  {
    const auto result = kordex::cli::CliResult::version("0.1.0");

    return expect_true(
               result.version_requested(),
               "version result should mark version requested") &&
           expect_true(
               !result.succeeded(),
               "version result should not be command success") &&
           expect_true(
               !result.failed(),
               "version result should not fail") &&
           expect_true(
               result.exit_code == 0,
               "version exit code should be zero") &&
           expect_true(
               !result.has_error(),
               "version result should not contain error") &&
           expect_true(
               result.has_output(),
               "version result should contain output") &&
           expect_true(
               ::std::string_view(result.output) == "0.1.0",
               "version output should match");
  }

  [[nodiscard]] bool test_cancelled_result()
  {
    const auto result = kordex::cli::CliResult::cancelled();

    return expect_true(
               result.was_cancelled(),
               "cancelled result should mark cancellation") &&
           expect_true(
               !result.succeeded(),
               "cancelled result should not succeed") &&
           expect_true(
               !result.failed(),
               "cancelled result should not be normal failure") &&
           expect_true(
               result.exit_code == 130,
               "cancelled exit code should be 130") &&
           expect_true(
               result.has_error(),
               "cancelled result should contain error") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::InternalError,
               "cancelled error should map to internal error");
  }

  [[nodiscard]] bool test_has_error_output()
  {
    auto result = kordex::cli::CliResult::failure(
        kordex::cli::make_cli_error(
            kordex::cli::CliErrorCode::IoError,
            "write failed"),
        1);

    result.error_output = "stderr text";

    return expect_true(
               result.has_error_output(),
               "result should contain error output") &&
           expect_true(
               ::std::string_view(result.error_output) == "stderr text",
               "error output should match");
  }

  [[nodiscard]] bool test_normalize_success()
  {
    auto result = kordex::cli::CliResult::failure(
        kordex::cli::make_cli_error(
            kordex::cli::CliErrorCode::InternalError,
            "old error"),
        2);

    result.status = kordex::cli::CliExitStatus::Success;
    result.normalize();

    return expect_true(
               result.succeeded(),
               "normalized success should succeed") &&
           expect_true(
               result.exit_code == 0,
               "normalized success exit code should be zero") &&
           expect_true(
               !result.has_error(),
               "normalized success should clear error");
  }

  [[nodiscard]] bool test_normalize_failed()
  {
    auto result = kordex::cli::CliResult::success();

    result.status = kordex::cli::CliExitStatus::Failed;
    result.exit_code = 0;
    result.normalize();

    return expect_true(
               result.failed(),
               "normalized failure should fail") &&
           expect_true(
               result.exit_code == 1,
               "normalized failure exit code should be one") &&
           expect_true(
               result.has_error(),
               "normalized failure should contain error") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::InternalError,
               "normalized failure should create internal error");
  }

  [[nodiscard]] bool test_normalize_cancelled()
  {
    auto result = kordex::cli::CliResult::success();

    result.status = kordex::cli::CliExitStatus::Cancelled;
    result.exit_code = 0;
    result.normalize();

    return expect_true(
               result.was_cancelled(),
               "normalized cancelled should be cancelled") &&
           expect_true(
               result.exit_code == 130,
               "normalized cancelled exit code should be 130") &&
           expect_true(
               result.has_error(),
               "normalized cancelled should contain error");
  }

  [[nodiscard]] bool test_status_strings()
  {
    return expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliExitStatus::Success)) == "success",
               "success string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliExitStatus::Failed)) == "failed",
               "failed string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliExitStatus::Help)) == "help",
               "help string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliExitStatus::Version)) == "version",
               "version string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliExitStatus::Cancelled)) == "cancelled",
               "cancelled string should match");
  }
} // namespace

int main()
{
  const bool ok =
      test_success_result() &&
      test_success_empty_output() &&
      test_failure_result() &&
      test_failure_normalizes_zero_exit_code() &&
      test_failure_with_ok_error_creates_internal_error() &&
      test_help_result() &&
      test_version_result() &&
      test_cancelled_result() &&
      test_has_error_output() &&
      test_normalize_success() &&
      test_normalize_failed() &&
      test_normalize_cancelled() &&
      test_status_strings();

  return ok ? 0 : 1;
}

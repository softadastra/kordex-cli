/**
 *
 *  @file test_error.cpp
 *  @author Softadastra
 *
 *  Copyright 2026, Softadastra.
 *  All rights reserved.
 *  https://github.com/softadastra/kordex-cli
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the LICENSE file.
 *
 *  Kordex CLI
 *
 */

#include <string_view>
#include <kordex/cli/Error.hpp>

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

  [[nodiscard]] bool test_ok_error()
  {
    const auto error = kordex::cli::ok();

    return expect_true(
        !error.has_error(),
        "ok error should not contain an error");
  }

  [[nodiscard]] bool test_make_cli_error()
  {
    const auto error = kordex::cli::make_cli_error(
        kordex::cli::CliErrorCode::InvalidArgument,
        "invalid command argument");

    return expect_true(
               error.has_error(),
               "CLI error should contain an error") &&
           expect_true(
               error.code() == vix::error::ErrorCode::InvalidArgument,
               "invalid argument should map to invalid argument") &&
           expect_true(
               ::std::string_view(error.message()) ==
                   "invalid command argument",
               "CLI error message should match");
  }

  [[nodiscard]] bool test_make_cli_error_with_default_message()
  {
    const auto error = kordex::cli::make_cli_error(
        kordex::cli::CliErrorCode::CommandNotFound);

    return expect_true(
               error.has_error(),
               "CLI error should contain an error") &&
           expect_true(
               error.code() == vix::error::ErrorCode::NotFound,
               "command not found should map to not found") &&
           expect_true(
               ::std::string_view(error.message()) ==
                   "CLI command not found",
               "default command not found message should match");
  }

  [[nodiscard]] bool test_make_none_error()
  {
    const auto error = kordex::cli::make_cli_error(
        kordex::cli::CliErrorCode::None,
        "ignored");

    return expect_true(
        !error.has_error(),
        "none error should return ok");
  }

  [[nodiscard]] bool test_error_code_mapping()
  {
    return expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::None) ==
                   vix::error::ErrorCode::Ok,
               "none should map to ok") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::InvalidArgument) ==
                   vix::error::ErrorCode::InvalidArgument,
               "invalid argument should map to invalid argument") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::InvalidConfig) ==
                   vix::error::ErrorCode::ConfigError,
               "invalid config should map to config error") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::CommandNotFound) ==
                   vix::error::ErrorCode::NotFound,
               "command not found should map to not found") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::CommandAlreadyExists) ==
                   vix::error::ErrorCode::AlreadyExists,
               "command already exists should map to already exists") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::CommandDisabled) ==
                   vix::error::ErrorCode::PermissionDenied,
               "command disabled should map to permission denied") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::ParseError) ==
                   vix::error::ErrorCode::ParseError,
               "parse error should map to parse error") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::HelpRequested) ==
                   vix::error::ErrorCode::Ok,
               "help requested should map to ok") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::VersionRequested) ==
                   vix::error::ErrorCode::Ok,
               "version requested should map to ok") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::IoError) ==
                   vix::error::ErrorCode::IoError,
               "io error should map to io error") &&
           expect_true(
               kordex::cli::to_error_code(
                   kordex::cli::CliErrorCode::InternalError) ==
                   vix::error::ErrorCode::InternalError,
               "internal error should map to internal error");
  }

  [[nodiscard]] bool test_error_code_strings()
  {
    return expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliErrorCode::None)) == "none",
               "none string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliErrorCode::InvalidArgument)) ==
                   "invalid_argument",
               "invalid argument string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliErrorCode::InvalidConfig)) ==
                   "invalid_config",
               "invalid config string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliErrorCode::CommandNotFound)) ==
                   "command_not_found",
               "command not found string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliErrorCode::CommandAlreadyExists)) ==
                   "command_already_exists",
               "command already exists string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliErrorCode::CommandDisabled)) ==
                   "command_disabled",
               "command disabled string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliErrorCode::ParseError)) ==
                   "parse_error",
               "parse error string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliErrorCode::InternalError)) ==
                   "internal_error",
               "internal error string should match");
  }

  [[nodiscard]] bool test_default_messages()
  {
    return expect_true(
               kordex::cli::default_message(
                   kordex::cli::CliErrorCode::None) == "no error",
               "none default message should match") &&
           expect_true(
               kordex::cli::default_message(
                   kordex::cli::CliErrorCode::InvalidArgument) ==
                   "invalid CLI argument",
               "invalid argument default message should match") &&
           expect_true(
               kordex::cli::default_message(
                   kordex::cli::CliErrorCode::InvalidConfig) ==
                   "invalid CLI configuration",
               "invalid config default message should match") &&
           expect_true(
               kordex::cli::default_message(
                   kordex::cli::CliErrorCode::CommandNotFound) ==
                   "CLI command not found",
               "command not found default message should match") &&
           expect_true(
               kordex::cli::default_message(
                   kordex::cli::CliErrorCode::CommandAlreadyExists) ==
                   "CLI command already exists",
               "command already exists default message should match") &&
           expect_true(
               kordex::cli::default_message(
                   kordex::cli::CliErrorCode::CommandDisabled) ==
                   "CLI command is disabled",
               "command disabled default message should match") &&
           expect_true(
               kordex::cli::default_message(
                   kordex::cli::CliErrorCode::ParseError) ==
                   "failed to parse CLI arguments",
               "parse error default message should match") &&
           expect_true(
               kordex::cli::default_message(
                   kordex::cli::CliErrorCode::InternalError) ==
                   "internal CLI error",
               "internal error default message should match");
  }
} // namespace

int main()
{
  const bool ok =
      test_ok_error() &&
      test_make_cli_error() &&
      test_make_cli_error_with_default_message() &&
      test_make_none_error() &&
      test_error_code_mapping() &&
      test_error_code_strings() &&
      test_default_messages();

  return ok ? 0 : 1;
}

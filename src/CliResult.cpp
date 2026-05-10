/**
 *
 *  @file CliResult.cpp
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

#include <utility>
#include <kordex/cli/CliResult.hpp>

namespace kordex::cli
{
  CliResult CliResult::success(
      ::std::string output_value)
  {
    CliResult result;
    result.status = CliExitStatus::Success;
    result.exit_code = 0;
    result.error = ok();
    result.output = ::std::move(output_value);
    return result;
  }

  CliResult CliResult::failure(
      Error error_value,
      int exit_code_value)
  {
    CliResult result;
    result.status = CliExitStatus::Failed;
    result.exit_code = exit_code_value == 0 ? 1 : exit_code_value;
    result.error = ::std::move(error_value);

    if (!result.error.has_error())
    {
      result.error = make_cli_error(
          CliErrorCode::InternalError,
          "CLI command failed");
    }

    return result;
  }

  CliResult CliResult::help(
      ::std::string output_value)
  {
    CliResult result;
    result.status = CliExitStatus::Help;
    result.exit_code = 0;
    result.error = ok();
    result.output = ::std::move(output_value);
    return result;
  }

  CliResult CliResult::version(
      ::std::string output_value)
  {
    CliResult result;
    result.status = CliExitStatus::Version;
    result.exit_code = 0;
    result.error = ok();
    result.output = ::std::move(output_value);
    return result;
  }

  CliResult CliResult::cancelled(
      ::std::string message)
  {
    CliResult result;
    result.status = CliExitStatus::Cancelled;
    result.exit_code = 130;
    result.error = make_cli_error(
        CliErrorCode::InternalError,
        ::std::move(message));
    return result;
  }

  bool CliResult::succeeded() const noexcept
  {
    return status == CliExitStatus::Success &&
           exit_code == 0 &&
           !error.has_error();
  }

  bool CliResult::failed() const noexcept
  {
    return status == CliExitStatus::Failed;
  }

  bool CliResult::help_requested() const noexcept
  {
    return status == CliExitStatus::Help;
  }

  bool CliResult::version_requested() const noexcept
  {
    return status == CliExitStatus::Version;
  }

  bool CliResult::was_cancelled() const noexcept
  {
    return status == CliExitStatus::Cancelled;
  }

  bool CliResult::has_output() const noexcept
  {
    return !output.empty();
  }

  bool CliResult::has_error_output() const noexcept
  {
    return !error_output.empty();
  }

  bool CliResult::has_error() const noexcept
  {
    return error.has_error();
  }

  void CliResult::normalize() noexcept
  {
    switch (status)
    {
    case CliExitStatus::Success:
    case CliExitStatus::Help:
    case CliExitStatus::Version:
      exit_code = 0;
      error = ok();
      break;

    case CliExitStatus::Failed:
      if (exit_code == 0)
      {
        exit_code = 1;
      }

      if (!error.has_error())
      {
        error = make_cli_error(
            CliErrorCode::InternalError,
            "CLI command failed");
      }
      break;

    case CliExitStatus::Cancelled:
      if (exit_code == 0)
      {
        exit_code = 130;
      }

      if (!error.has_error())
      {
        error = make_cli_error(
            CliErrorCode::InternalError,
            "CLI operation was cancelled");
      }
      break;
    }
  }

  const char *to_string(
      CliExitStatus status) noexcept
  {
    switch (status)
    {
    case CliExitStatus::Success:
      return "success";
    case CliExitStatus::Failed:
      return "failed";
    case CliExitStatus::Help:
      return "help";
    case CliExitStatus::Version:
      return "version";
    case CliExitStatus::Cancelled:
      return "cancelled";
    }

    return "failed";
  }

} // namespace kordex::cli

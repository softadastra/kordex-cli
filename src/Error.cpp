/**
 *
 *  @file Error.cpp
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

#include <kordex/cli/Error.hpp>

namespace kordex::cli
{
  Error ok()
  {
    return Error{};
  }

  Error make_cli_error(
      CliErrorCode code,
      ::std::string message)
  {
    if (code == CliErrorCode::None)
    {
      return ok();
    }

    if (message.empty())
    {
      message = ::std::string(default_message(code));
    }

    return Error(
        to_error_code(code),
        ::std::move(message));
  }

  vix::error::ErrorCode to_error_code(
      CliErrorCode code) noexcept
  {
    switch (code)
    {
    case CliErrorCode::None:
      return vix::error::ErrorCode::Ok;

    case CliErrorCode::InvalidArgument:
      return vix::error::ErrorCode::InvalidArgument;

    case CliErrorCode::InvalidConfig:
      return vix::error::ErrorCode::ConfigError;

    case CliErrorCode::CommandNotFound:
      return vix::error::ErrorCode::NotFound;

    case CliErrorCode::CommandAlreadyExists:
      return vix::error::ErrorCode::AlreadyExists;

    case CliErrorCode::CommandDisabled:
      return vix::error::ErrorCode::PermissionDenied;

    case CliErrorCode::ParseError:
      return vix::error::ErrorCode::ParseError;

    case CliErrorCode::HelpRequested:
    case CliErrorCode::VersionRequested:
      return vix::error::ErrorCode::Ok;

    case CliErrorCode::IoError:
      return vix::error::ErrorCode::IoError;

    case CliErrorCode::RuntimeError:
    case CliErrorCode::BindingError:
    case CliErrorCode::StdError:
    case CliErrorCode::InternalError:
      return vix::error::ErrorCode::InternalError;
    }

    return vix::error::ErrorCode::InternalError;
  }

  const char *to_string(
      CliErrorCode code) noexcept
  {
    switch (code)
    {
    case CliErrorCode::None:
      return "none";
    case CliErrorCode::InvalidArgument:
      return "invalid_argument";
    case CliErrorCode::InvalidConfig:
      return "invalid_config";
    case CliErrorCode::CommandNotFound:
      return "command_not_found";
    case CliErrorCode::CommandAlreadyExists:
      return "command_already_exists";
    case CliErrorCode::CommandDisabled:
      return "command_disabled";
    case CliErrorCode::ParseError:
      return "parse_error";
    case CliErrorCode::HelpRequested:
      return "help_requested";
    case CliErrorCode::VersionRequested:
      return "version_requested";
    case CliErrorCode::RuntimeError:
      return "runtime_error";
    case CliErrorCode::BindingError:
      return "binding_error";
    case CliErrorCode::StdError:
      return "std_error";
    case CliErrorCode::IoError:
      return "io_error";
    case CliErrorCode::InternalError:
      return "internal_error";
    }

    return "internal_error";
  }

  ::std::string_view default_message(
      CliErrorCode code) noexcept
  {
    switch (code)
    {
    case CliErrorCode::None:
      return "no error";
    case CliErrorCode::InvalidArgument:
      return "invalid CLI argument";
    case CliErrorCode::InvalidConfig:
      return "invalid CLI configuration";
    case CliErrorCode::CommandNotFound:
      return "CLI command not found";
    case CliErrorCode::CommandAlreadyExists:
      return "CLI command already exists";
    case CliErrorCode::CommandDisabled:
      return "CLI command is disabled";
    case CliErrorCode::ParseError:
      return "failed to parse CLI arguments";
    case CliErrorCode::HelpRequested:
      return "help requested";
    case CliErrorCode::VersionRequested:
      return "version requested";
    case CliErrorCode::RuntimeError:
      return "runtime operation failed";
    case CliErrorCode::BindingError:
      return "bindings operation failed";
    case CliErrorCode::StdError:
      return "standard module operation failed";
    case CliErrorCode::IoError:
      return "CLI I/O operation failed";
    case CliErrorCode::InternalError:
      return "internal CLI error";
    }

    return "internal CLI error";
  }

} // namespace kordex::cli

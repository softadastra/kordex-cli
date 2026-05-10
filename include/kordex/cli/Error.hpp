/**
 *
 *  @file Error.hpp
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

#ifndef KORDEX_CLI_ERROR_HPP
#define KORDEX_CLI_ERROR_HPP

#include <string>
#include <string_view>

#include <vix/error/Error.hpp>
#include <vix/error/ErrorCode.hpp>

namespace kordex::cli
{
  /**
   * @enum CliErrorCode
   * @brief Error codes used by the Kordex CLI module.
   */
  enum class CliErrorCode
  {
    None,
    InvalidArgument,
    InvalidConfig,
    CommandNotFound,
    CommandAlreadyExists,
    CommandDisabled,
    ParseError,
    HelpRequested,
    VersionRequested,
    RuntimeError,
    BindingError,
    StdError,
    IoError,
    InternalError
  };

  /**
   * @brief Shared error type used by Kordex CLI.
   */
  using Error = vix::error::Error;

  /**
   * @brief Return a successful CLI error value.
   */
  [[nodiscard]] Error ok();

  /**
   * @brief Create a Kordex CLI error.
   */
  [[nodiscard]] Error make_cli_error(
      CliErrorCode code,
      ::std::string message = {});

  /**
   * @brief Convert CliErrorCode to the shared Vix error code.
   */
  [[nodiscard]] vix::error::ErrorCode to_error_code(
      CliErrorCode code) noexcept;

  /**
   * @brief Convert CliErrorCode to a stable string.
   */
  [[nodiscard]] const char *to_string(
      CliErrorCode code) noexcept;

  /**
   * @brief Return the default message for a CLI error code.
   */
  [[nodiscard]] ::std::string_view default_message(
      CliErrorCode code) noexcept;

} // namespace kordex::cli

#endif // KORDEX_CLI_ERROR_HPP

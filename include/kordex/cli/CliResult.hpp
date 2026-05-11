/**
 *
 *  @file CliResult.hpp
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

#ifndef KORDEX_CLI_CLI_RESULT_HPP
#define KORDEX_CLI_CLI_RESULT_HPP

#include <string>

#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @enum CliExitStatus
   * @brief High-level CLI execution status.
   */
  enum class CliExitStatus
  {
    Success,
    Failed,
    Help,
    Version,
    Cancelled
  };

  /**
   * @struct CliResult
   * @brief Result produced by a CLI command or CLI run.
   */
  struct CliResult
  {
    /**
     * @brief Final CLI status.
     */
    CliExitStatus status{CliExitStatus::Success};

    /**
     * @brief Process exit code.
     *
     * By convention:
     * - 0 means success
     * - non-zero means failure
     */
    int exit_code{0};

    /**
     * @brief Optional structured error.
     */
    Error error{};

    /**
     * @brief Optional standard output text.
     */
    ::std::string output{};

    /**
     * @brief Optional standard error text.
     */
    ::std::string error_output{};

    /**
     * @brief Build a successful CLI result.
     */
    [[nodiscard]] static CliResult success(
        ::std::string output = {});

    /**
     * @brief Build a failed CLI result.
     */
    [[nodiscard]] static CliResult failure(
        Error error,
        int exit_code = 1);

    /**
     * @brief Build a help result.
     */
    [[nodiscard]] static CliResult help(
        ::std::string output = {});

    /**
     * @brief Build a version result.
     */
    [[nodiscard]] static CliResult version(
        ::std::string output = {});

    /**
     * @brief Build a cancelled CLI result.
     */
    [[nodiscard]] static CliResult cancelled(
        ::std::string message = "CLI operation was cancelled");

    /**
     * @brief Return true when the result succeeded.
     */
    [[nodiscard]] bool succeeded() const noexcept;

    /**
     * @brief Return true when the result failed.
     */
    [[nodiscard]] bool failed() const noexcept;

    /**
     * @brief Return true when the result represents help output.
     */
    [[nodiscard]] bool help_requested() const noexcept;

    /**
     * @brief Return true when the result represents version output.
     */
    [[nodiscard]] bool version_requested() const noexcept;

    /**
     * @brief Return true when the result was cancelled.
     */
    [[nodiscard]] bool was_cancelled() const noexcept;

    /**
     * @brief Return true if stdout output is present.
     */
    [[nodiscard]] bool has_output() const noexcept;

    /**
     * @brief Return true if stderr output is present.
     */
    [[nodiscard]] bool has_error_output() const noexcept;

    /**
     * @brief Return true if a structured error is present.
     */
    [[nodiscard]] bool has_error() const noexcept;

    /**
     * @brief Normalize exit code and error state.
     */
    void normalize() noexcept;
  };

  /**
   * @brief Result type used by operations returning CliResult.
   */
  using CliExecutionResult = Result<CliResult>;

  /**
   * @brief Convert CliExitStatus to a stable string.
   */
  [[nodiscard]] const char *to_string(
      CliExitStatus status) noexcept;

} // namespace kordex::cli

#endif // KORDEX_CLI_CLI_RESULT_HPP

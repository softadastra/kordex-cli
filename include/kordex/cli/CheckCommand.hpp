/**
 *
 *  @file CheckCommand.hpp
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

#ifndef KORDEX_CLI_CHECK_COMMAND_HPP
#define KORDEX_CLI_CHECK_COMMAND_HPP

#include <string>
#include <vector>

#include <kordex/runtime/SourceFile.hpp>

#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @struct CheckCommandOptions
   * @brief Options used by the check command.
   */
  struct CheckCommandOptions
  {
    /**
     * @brief Source file to check.
     */
    ::std::string file{};

    /**
     * @brief Whether warnings should be reported.
     */
    bool warnings{true};

    /**
     * @brief Whether the command should print detailed information.
     */
    bool details{false};

    /**
     * @brief Whether JSON-like output should be produced by the command.
     */
    bool json{false};

    /**
     * @brief Return true if a source file was provided.
     */
    [[nodiscard]] bool has_file() const noexcept;
  };

  /**
   * @struct CheckDiagnostic
   * @brief Diagnostic produced by the check command.
   */
  struct CheckDiagnostic
  {
    /**
     * @brief Diagnostic severity.
     */
    ::std::string level{"info"};

    /**
     * @brief Diagnostic message.
     */
    ::std::string message{};

    /**
     * @brief Optional source path.
     */
    ::std::string path{};

    /**
     * @brief Optional source line.
     */
    ::std::size_t line{0};

    /**
     * @brief Optional source column.
     */
    ::std::size_t column{0};

    /**
     * @brief Return true if the diagnostic has a message.
     */
    [[nodiscard]] bool has_message() const noexcept;

    /**
     * @brief Return true if source location is available.
     */
    [[nodiscard]] bool has_location() const noexcept;
  };

  /**
   * @struct CheckReport
   * @brief Report produced after checking a source file.
   */
  struct CheckReport
  {
    /**
     * @brief Checked source file path.
     */
    ::std::string file{};

    /**
     * @brief Detected source type.
     */
    kordex::runtime::SourceType source_type{
        kordex::runtime::SourceType::Unknown};

    /**
     * @brief Source size in bytes.
     */
    ::std::size_t size{0};

    /**
     * @brief Number of source lines.
     */
    ::std::size_t lines{0};

    /**
     * @brief Whether the file can be executed by the runtime.
     */
    bool executable{false};

    /**
     * @brief Whether the check succeeded.
     */
    bool ok{false};

    /**
     * @brief Diagnostics produced by the check.
     */
    ::std::vector<CheckDiagnostic> diagnostics{};

    /**
     * @brief Return true if diagnostics exist.
     */
    [[nodiscard]] bool has_diagnostics() const noexcept;

    /**
     * @brief Return number of error diagnostics.
     */
    [[nodiscard]] ::std::size_t error_count() const noexcept;

    /**
     * @brief Return number of warning diagnostics.
     */
    [[nodiscard]] ::std::size_t warning_count() const noexcept;
  };

  /**
   * @brief Parse check command arguments.
   */
  [[nodiscard]] Result<CheckCommandOptions> parse_check_options(
      const CommandContext &context);

  /**
   * @brief Validate check command options.
   */
  [[nodiscard]] Error validate_check_options(
      const CheckCommandOptions &options);

  /**
   * @brief Check a source file and produce a report.
   */
  [[nodiscard]] Result<CheckReport> check_source_file(
      const CheckCommandOptions &options);

  /**
   * @brief Render check report as text.
   */
  [[nodiscard]] ::std::string render_check_report(
      const CheckReport &report,
      const CheckCommandOptions &options);

  /**
   * @brief Render check report as JSON.
   */
  [[nodiscard]] ::std::string render_check_report_json(
      const CheckReport &report);

  /**
   * @brief Run the check command.
   */
  [[nodiscard]] CliResult run_check_command(
      const CommandContext &context);

  /**
   * @brief Create the check command.
   */
  [[nodiscard]] Result<Command> create_check_command();

} // namespace kordex::cli

#endif // KORDEX_CLI_CHECK_COMMAND_HPP

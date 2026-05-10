/**
 *
 *  @file CliOptions.hpp
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

#ifndef KORDEX_CLI_CLI_OPTIONS_HPP
#define KORDEX_CLI_CLI_OPTIONS_HPP

#include <string>
#include <vector>

namespace kordex::cli
{
  /**
   * @enum CliMode
   * @brief High-level CLI execution mode.
   */
  enum class CliMode
  {
    Normal,
    Development,
    Test
  };

  /**
   * @enum OutputMode
   * @brief CLI output rendering mode.
   */
  enum class OutputMode
  {
    Text,
    Json,
    Quiet
  };

  /**
   * @struct CliOptions
   * @brief User-facing options used to configure the Kordex CLI.
   */
  struct CliOptions
  {
    /**
     * @brief CLI mode.
     */
    CliMode mode{CliMode::Normal};

    /**
     * @brief Output rendering mode.
     */
    OutputMode output_mode{OutputMode::Text};

    /**
     * @brief Executable name used in help text.
     */
    ::std::string executable_name{"kordex"};

    /**
     * @brief Working directory used by commands.
     *
     * Empty means current process directory.
     */
    ::std::string working_directory{};

    /**
     * @brief Optional config file path.
     */
    ::std::string config_path{};

    /**
     * @brief Raw command-line arguments.
     */
    ::std::vector<::std::string> args{};

    /**
     * @brief Whether colored output is enabled.
     */
    bool color{true};

    /**
     * @brief Whether verbose output is enabled.
     */
    bool verbose{false};

    /**
     * @brief Whether debug output is enabled.
     */
    bool debug{false};

    /**
     * @brief Whether command execution should be skipped.
     */
    bool dry_run{false};

    /**
     * @brief Whether interactive prompts are allowed.
     */
    bool interactive{true};

    /**
     * @brief Whether command aliases are enabled.
     */
    bool enable_aliases{true};

    /**
     * @brief Whether unknown arguments should be preserved.
     */
    bool allow_unknown_args{false};

    /**
     * @brief Whether help command support is enabled.
     */
    bool enable_help{true};

    /**
     * @brief Whether init command support is enabled.
     */
    bool enable_init{true};

    /**
     * @brief Whether run command support is enabled.
     */
    bool enable_run{true};

    /**
     * @brief Whether check command support is enabled.
     */
    bool enable_check{true};

    /**
     * @brief Whether build command support is enabled.
     */
    bool enable_build{true};

    /**
     * @brief Whether repl command support is enabled.
     */
    bool enable_repl{true};

    /**
     * @brief Whether version command support is enabled.
     */
    bool enable_version{true};

    /**
     * @brief Return default CLI options.
     */
    [[nodiscard]] static CliOptions defaults();

    /**
     * @brief Return development CLI options.
     */
    [[nodiscard]] static CliOptions development();

    /**
     * @brief Return test CLI options.
     */
    [[nodiscard]] static CliOptions test();

    /**
     * @brief Return quiet CLI options.
     */
    [[nodiscard]] static CliOptions quiet();

    /**
     * @brief Return JSON-output CLI options.
     */
    [[nodiscard]] static CliOptions json();

    /**
     * @brief Return true if executable name is configured.
     */
    [[nodiscard]] bool has_executable_name() const noexcept;

    /**
     * @brief Return true if working directory is configured.
     */
    [[nodiscard]] bool has_working_directory() const noexcept;

    /**
     * @brief Return true if config path is configured.
     */
    [[nodiscard]] bool has_config_path() const noexcept;

    /**
     * @brief Return true if raw arguments are present.
     */
    [[nodiscard]] bool has_args() const noexcept;

    /**
     * @brief Return true if text output is selected.
     */
    [[nodiscard]] bool text_output() const noexcept;

    /**
     * @brief Return true if JSON output is selected.
     */
    [[nodiscard]] bool json_output() const noexcept;

    /**
     * @brief Return true if quiet output is selected.
     */
    [[nodiscard]] bool quiet_output() const noexcept;

    /**
     * @brief Return true if at least one command is enabled.
     */
    [[nodiscard]] bool has_enabled_commands() const noexcept;
  };

  /**
   * @brief Convert CliMode to a stable string.
   */
  [[nodiscard]] const char *to_string(
      CliMode mode) noexcept;

  /**
   * @brief Convert OutputMode to a stable string.
   */
  [[nodiscard]] const char *to_string(
      OutputMode mode) noexcept;

} // namespace kordex::cli

#endif // KORDEX_CLI_CLI_OPTIONS_HPP

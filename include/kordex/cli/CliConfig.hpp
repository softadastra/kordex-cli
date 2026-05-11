/**
 *
 *  @file CliConfig.hpp
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

#ifndef KORDEX_CLI_CLI_CONFIG_HPP
#define KORDEX_CLI_CLI_CONFIG_HPP

#include <string>
#include <vector>

#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>
#include <kordex/cli/CliOptions.hpp>

namespace kordex::cli
{
  /**
   * @struct CliConfig
   * @brief Final normalized configuration used by the Kordex CLI.
   */
  struct CliConfig
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
     * @brief Executable name used by help and diagnostics.
     */
    ::std::string executable_name{"kordex"};

    /**
     * @brief Working directory used by commands.
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

    bool color{true};
    bool verbose{false};
    bool debug{false};
    bool dry_run{false};
    bool interactive{true};
    bool enable_aliases{true};
    bool allow_unknown_args{false};

    bool enable_help{true};
    bool enable_init{true};
    bool enable_run{true};
    bool enable_check{true};
    bool enable_build{true};
    bool enable_repl{true};
    bool enable_version{true};

    /**
     * @brief Build a normalized config from options.
     */
    [[nodiscard]] static Result<CliConfig> from_options(
        const CliOptions &options);

    /**
     * @brief Build a normalized config from environment.
     *
     * Environment support is intentionally minimal for now.
     */
    [[nodiscard]] static Result<CliConfig> from_environment(
        const CliOptions &base = CliOptions::defaults());

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

    /**
     * @brief Return true if a command is enabled by name.
     */
    [[nodiscard]] bool command_enabled(
        const ::std::string &name) const noexcept;

    /**
     * @brief Return the number of enabled commands.
     */
    [[nodiscard]] ::std::size_t enabled_command_count() const noexcept;

    /**
     * @brief Validate the final CLI configuration.
     */
    [[nodiscard]] Error validate() const;
  };

  /**
   * @brief Parse CLI mode from string.
   */
  [[nodiscard]] Result<CliMode> parse_cli_mode(
      const ::std::string &value);

  /**
   * @brief Parse output mode from string.
   */
  [[nodiscard]] Result<OutputMode> parse_output_mode(
      const ::std::string &value);

} // namespace kordex::cli

#endif // KORDEX_CLI_CLI_CONFIG_HPP

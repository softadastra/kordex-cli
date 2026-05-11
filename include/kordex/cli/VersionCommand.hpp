/**
 *
 *  @file VersionCommand.hpp
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

#ifndef KORDEX_CLI_VERSION_COMMAND_HPP
#define KORDEX_CLI_VERSION_COMMAND_HPP

#include <string>

#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @struct VersionCommandOptions
   * @brief Options used by the version command.
   */
  struct VersionCommandOptions
  {
    /**
     * @brief Whether detailed version information should be shown.
     */
    bool details{false};

    /**
     * @brief Whether JSON output should be produced.
     */
    bool json{false};
  };

  /**
   * @struct VersionInfo
   * @brief Version metadata rendered by the version command.
   */
  struct VersionInfo
  {
    /**
     * @brief Product name.
     */
    ::std::string product{"Kordex"};

    /**
     * @brief CLI version.
     */
    ::std::string cli_version{};

    /**
     * @brief Runtime version.
     */
    ::std::string runtime_version{};

    /**
     * @brief Bindings version.
     */
    ::std::string bindings_version{};

    /**
     * @brief Std version.
     */
    ::std::string std_version{};

    /**
     * @brief Return true if CLI version exists.
     */
    [[nodiscard]] bool has_cli_version() const noexcept;

    /**
     * @brief Return true if runtime version exists.
     */
    [[nodiscard]] bool has_runtime_version() const noexcept;

    /**
     * @brief Return true if bindings version exists.
     */
    [[nodiscard]] bool has_bindings_version() const noexcept;

    /**
     * @brief Return true if std version exists.
     */
    [[nodiscard]] bool has_std_version() const noexcept;
  };

  /**
   * @brief Parse version command arguments.
   */
  [[nodiscard]] Result<VersionCommandOptions> parse_version_options(
      const CommandContext &context);

  /**
   * @brief Validate version command options.
   */
  [[nodiscard]] Error validate_version_options(
      const VersionCommandOptions &options);

  /**
   * @brief Collect Kordex version metadata.
   */
  [[nodiscard]] VersionInfo collect_version_info();

  /**
   * @brief Render version information as text.
   */
  [[nodiscard]] ::std::string render_version_info(
      const VersionInfo &info,
      const VersionCommandOptions &options);

  /**
   * @brief Render version information as JSON.
   */
  [[nodiscard]] ::std::string render_version_info_json(
      const VersionInfo &info);

  /**
   * @brief Run the version command.
   */
  [[nodiscard]] CliResult run_version_command(
      const CommandContext &context);

  /**
   * @brief Create the version command.
   */
  [[nodiscard]] Result<Command> create_version_command();

} // namespace kordex::cli

#endif // KORDEX_CLI_VERSION_COMMAND_HPP

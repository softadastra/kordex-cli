/**
 *
 *  @file InitCommand.hpp
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

#ifndef KORDEX_CLI_INIT_COMMAND_HPP
#define KORDEX_CLI_INIT_COMMAND_HPP

#include <string>

#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @struct InitProjectOptions
   * @brief Options used to create a new Kordex project.
   */
  struct InitProjectOptions
  {
    /**
     * @brief Project name.
     */
    ::std::string name{"app"};

    /**
     * @brief Target directory where the project will be created.
     */
    ::std::string directory{};

    /**
     * @brief Whether existing files may be overwritten.
     */
    bool force{false};

    /**
     * @brief Whether package.json should be generated.
     */
    bool package_json{true};

    /**
     * @brief Whether kordex.json should be generated.
     */
    bool kordex_json{true};

    /**
     * @brief Whether src/main.js should be generated.
     */
    bool main_file{true};

    /**
     * @brief Return true if project name exists.
     */
    [[nodiscard]] bool has_name() const noexcept;

    /**
     * @brief Return true if directory exists.
     */
    [[nodiscard]] bool has_directory() const noexcept;
  };

  /**
   * @brief Parse init command arguments.
   */
  [[nodiscard]] Result<InitProjectOptions> parse_init_options(
      const CommandContext &context);

  /**
   * @brief Validate init project options.
   */
  [[nodiscard]] Error validate_init_options(
      const InitProjectOptions &options);

  /**
   * @brief Create a new Kordex project.
   */
  [[nodiscard]] CliResult run_init_command(
      const CommandContext &context);

  /**
   * @brief Create the init command.
   */
  [[nodiscard]] Result<Command> create_init_command();

} // namespace kordex::cli

#endif // KORDEX_CLI_INIT_COMMAND_HPP

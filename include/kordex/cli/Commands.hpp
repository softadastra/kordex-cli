/**
 *
 *  @file Commands.hpp
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

#ifndef KORDEX_CLI_COMMANDS_HPP
#define KORDEX_CLI_COMMANDS_HPP

#include <string>
#include <vector>

#include <kordex/cli/CliConfig.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/CommandRegistry.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/HelpFormatter.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @brief Create the built-in help command.
   */
  [[nodiscard]] Result<Command> create_help_command();

  /**
   * @brief Create the init command.
   *
   * Implemented by InitCommand.cpp.
   */
  [[nodiscard]] Result<Command> create_init_command();

  /**
   * @brief Create the run command.
   *
   * Implemented by RunCommand.cpp.
   */
  [[nodiscard]] Result<Command> create_run_command();

  /**
   * @brief Create the check command.
   *
   * Implemented by CheckCommand.cpp.
   */
  [[nodiscard]] Result<Command> create_check_command();

  /**
   * @brief Create the build command.
   *
   * Implemented by BuildCommand.cpp.
   */
  [[nodiscard]] Result<Command> create_build_command();

  /**
   * @brief Create the repl command.
   *
   * Implemented by ReplCommand.cpp.
   */
  [[nodiscard]] Result<Command> create_repl_command();

  /**
   * @brief Create the version command.
   *
   * Implemented by VersionCommand.cpp.
   */
  [[nodiscard]] Result<Command> create_version_command();

  /**
   * @brief Return the canonical built-in command names.
   */
  [[nodiscard]] ::std::vector<::std::string> builtin_command_names();

  /**
   * @brief Return true if the command name is a known built-in command.
   */
  [[nodiscard]] bool is_builtin_command(
      const ::std::string &name) noexcept;

  /**
   * @brief Return true if the command is enabled by config.
   */
  [[nodiscard]] bool command_enabled_by_config(
      const CliConfig &config,
      const ::std::string &name) noexcept;

  /**
   * @brief Create one built-in command by name.
   */
  [[nodiscard]] Result<Command> create_builtin_command(
      const ::std::string &name);

  /**
   * @brief Register one built-in command by name.
   */
  [[nodiscard]] Error register_builtin_command(
      CommandRegistry &registry,
      const ::std::string &name);

  /**
   * @brief Register all enabled built-in commands.
   */
  [[nodiscard]] Error register_default_commands(
      CommandRegistry &registry,
      const CliConfig &config);

  /**
   * @brief Create a registry containing all enabled built-in commands.
   */
  [[nodiscard]] Result<CommandRegistry> create_default_command_registry(
      const CliConfig &config);

  /**
   * @brief Create a registry from default CLI options.
   */
  [[nodiscard]] Result<CommandRegistry> create_default_command_registry();

  [[nodiscard]] Result<Command> create_install_command();

  [[nodiscard]] Result<Command> create_update_command();

} // namespace kordex::cli

#endif // KORDEX_CLI_COMMANDS_HPP

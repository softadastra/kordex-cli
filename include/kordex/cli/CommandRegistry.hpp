/**
 *
 *  @file CommandRegistry.hpp
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

#ifndef KORDEX_CLI_COMMAND_REGISTRY_HPP
#define KORDEX_CLI_COMMAND_REGISTRY_HPP

#include <string>
#include <vector>

#include <kordex/cli/Command.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @class CommandRegistry
   * @brief Registry owning all available CLI commands.
   *
   * CommandRegistry stores commands, resolves aliases, and executes commands
   * by name.
   */
  class CommandRegistry
  {
  public:
    /**
     * @brief Construct an empty command registry.
     */
    CommandRegistry() = default;

    /**
     * @brief Register a command.
     */
    [[nodiscard]] Error register_command(
        Command command);

    /**
     * @brief Remove all registered commands.
     */
    void clear();

    /**
     * @brief Return true if no command is registered.
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Return the number of registered commands.
     */
    [[nodiscard]] ::std::size_t size() const noexcept;

    /**
     * @brief Return all registered commands.
     */
    [[nodiscard]] const ::std::vector<Command> &commands() const noexcept;

    /**
     * @brief Return only visible commands.
     */
    [[nodiscard]] ::std::vector<Command> visible_commands() const;

    /**
     * @brief Return true if a command or alias exists.
     */
    [[nodiscard]] bool has_command(
        const ::std::string &name) const noexcept;

    /**
     * @brief Find a command by name or alias.
     */
    [[nodiscard]] Result<Command> find(
        const ::std::string &name) const;

    /**
     * @brief Return the first command name from argv-style arguments.
     */
    [[nodiscard]] Result<::std::string> resolve_command_name(
        const ::std::vector<::std::string> &args) const;

    /**
     * @brief Return command arguments after the command name.
     */
    [[nodiscard]] CommandArguments resolve_command_args(
        const ::std::vector<::std::string> &args) const;

    /**
     * @brief Execute a command by name.
     */
    [[nodiscard]] CliResult run(
        const ::std::string &name,
        const CliConfig &config,
        CommandArguments args = {}) const;

    /**
     * @brief Execute a command from argv-style arguments.
     */
    [[nodiscard]] CliResult run(
        const CliConfig &config,
        const ::std::vector<::std::string> &args) const;

  private:
    [[nodiscard]] bool conflicts_with_existing_command(
        const Command &command) const noexcept;

    ::std::vector<Command> commands_{};
  };

} // namespace kordex::cli

#endif // KORDEX_CLI_COMMAND_REGISTRY_HPP

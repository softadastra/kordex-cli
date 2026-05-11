/**
 *
 *  @file Command.hpp
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

#ifndef KORDEX_CLI_COMMAND_HPP
#define KORDEX_CLI_COMMAND_HPP

#include <functional>
#include <string>
#include <vector>

#include <kordex/cli/CliConfig.hpp>
#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @brief Command argument list.
   */
  using CommandArguments = ::std::vector<::std::string>;

  /**
   * @struct CommandContext
   * @brief Runtime context passed to a command handler.
   */
  struct CommandContext
  {
    /**
     * @brief Normalized CLI configuration.
     */
    CliConfig config{};

    /**
     * @brief Command name used for execution.
     */
    ::std::string command_name{};

    /**
     * @brief Positional command arguments.
     */
    CommandArguments args{};

    /**
     * @brief Return true if the context has arguments.
     */
    [[nodiscard]] bool has_args() const noexcept;

    /**
     * @brief Return number of command arguments.
     */
    [[nodiscard]] ::std::size_t arg_count() const noexcept;
  };

  /**
   * @brief Callable command handler.
   */
  using CommandHandler = ::std::function<CliResult(const CommandContext &)>;

  /**
   * @struct CommandInfo
   * @brief Static metadata describing a CLI command.
   */
  struct CommandInfo
  {
    /**
     * @brief Primary command name.
     */
    ::std::string name{};

    /**
     * @brief Command aliases.
     */
    ::std::vector<::std::string> aliases{};

    /**
     * @brief Short command summary.
     */
    ::std::string summary{};

    /**
     * @brief Longer command description.
     */
    ::std::string description{};

    /**
     * @brief Usage string shown in help output.
     */
    ::std::string usage{};

    /**
     * @brief Whether the command should be hidden from normal help.
     */
    bool hidden{false};

    /**
     * @brief Whether the command can be executed.
     */
    bool enabled{true};

    /**
     * @brief Return true if the command has a primary name.
     */
    [[nodiscard]] bool has_name() const noexcept;

    /**
     * @brief Return true if the command has aliases.
     */
    [[nodiscard]] bool has_aliases() const noexcept;

    /**
     * @brief Return true if the command has a summary.
     */
    [[nodiscard]] bool has_summary() const noexcept;

    /**
     * @brief Return true if the command has a usage string.
     */
    [[nodiscard]] bool has_usage() const noexcept;
  };

  /**
   * @class Command
   * @brief Executable CLI command.
   *
   * Command owns command metadata and a handler callable.
   */
  class Command
  {
  public:
    /**
     * @brief Construct an empty invalid command.
     */
    Command() = default;

    /**
     * @brief Construct a command from metadata and handler.
     */
    Command(CommandInfo info, CommandHandler handler);

    /**
     * @brief Create and validate a command.
     */
    [[nodiscard]] static Result<Command> create(
        CommandInfo info,
        CommandHandler handler);

    /**
     * @brief Return command metadata.
     */
    [[nodiscard]] const CommandInfo &info() const noexcept;

    /**
     * @brief Return command name.
     */
    [[nodiscard]] const ::std::string &name() const noexcept;

    /**
     * @brief Return command aliases.
     */
    [[nodiscard]] const ::std::vector<::std::string> &aliases() const noexcept;

    /**
     * @brief Return command summary.
     */
    [[nodiscard]] const ::std::string &summary() const noexcept;

    /**
     * @brief Return command description.
     */
    [[nodiscard]] const ::std::string &description() const noexcept;

    /**
     * @brief Return command usage string.
     */
    [[nodiscard]] const ::std::string &usage() const noexcept;

    /**
     * @brief Return true if the command is valid.
     */
    [[nodiscard]] bool valid() const noexcept;

    /**
     * @brief Return true if the command is enabled.
     */
    [[nodiscard]] bool enabled() const noexcept;

    /**
     * @brief Return true if the command is hidden.
     */
    [[nodiscard]] bool hidden() const noexcept;

    /**
     * @brief Return true if a handler is configured.
     */
    [[nodiscard]] bool has_handler() const noexcept;

    /**
     * @brief Return true if name matches command name or alias.
     */
    [[nodiscard]] bool matches(
        const ::std::string &candidate) const noexcept;

    /**
     * @brief Return true if the command has an alias.
     */
    [[nodiscard]] bool has_alias(
        const ::std::string &alias) const noexcept;

    /**
     * @brief Validate command metadata and handler.
     */
    [[nodiscard]] Error validate() const;

    /**
     * @brief Run command with explicit context.
     */
    [[nodiscard]] CliResult run(
        const CommandContext &context) const;

    /**
     * @brief Run command with config and args.
     */
    [[nodiscard]] CliResult run(
        const CliConfig &config,
        CommandArguments args = {}) const;

  private:
    CommandInfo info_{};
    CommandHandler handler_{};
  };

} // namespace kordex::cli

#endif // KORDEX_CLI_COMMAND_HPP

/**
 *
 *  @file CommandRegistry.cpp
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

#include <utility>

#include <kordex/cli/CommandRegistry.hpp>

namespace kordex::cli
{
  Error CommandRegistry::register_command(
      Command command)
  {
    const auto validation = command.validate();
    if (validation)
    {
      return validation;
    }

    if (conflicts_with_existing_command(command))
    {
      return make_cli_error(
          CliErrorCode::CommandAlreadyExists,
          "CLI command already exists: " + command.name());
    }

    commands_.push_back(::std::move(command));

    return ok();
  }

  void CommandRegistry::clear()
  {
    commands_.clear();
  }

  bool CommandRegistry::empty() const noexcept
  {
    return commands_.empty();
  }

  ::std::size_t CommandRegistry::size() const noexcept
  {
    return commands_.size();
  }

  const ::std::vector<Command> &CommandRegistry::commands() const noexcept
  {
    return commands_;
  }

  ::std::vector<Command> CommandRegistry::visible_commands() const
  {
    ::std::vector<Command> result;

    for (const auto &command : commands_)
    {
      if (!command.hidden())
      {
        result.push_back(command);
      }
    }

    return result;
  }

  bool CommandRegistry::has_command(
      const ::std::string &name) const noexcept
  {
    for (const auto &command : commands_)
    {
      if (command.matches(name))
      {
        return true;
      }
    }

    return false;
  }

  Result<Command> CommandRegistry::find(
      const ::std::string &name) const
  {
    if (name.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "command name cannot be empty");
    }

    for (const auto &command : commands_)
    {
      if (command.matches(name))
      {
        return command;
      }
    }

    return make_cli_error(
        CliErrorCode::CommandNotFound,
        "CLI command not found: " + name);
  }

  Result<::std::string> CommandRegistry::resolve_command_name(
      const ::std::vector<::std::string> &args) const
  {
    if (args.empty())
    {
      if (has_command("help"))
      {
        return ::std::string("help");
      }

      return make_cli_error(
          CliErrorCode::CommandNotFound,
          "no CLI command provided");
    }

    const auto &first = args.front();

    if (first == "--help" || first == "-h")
    {
      return ::std::string("help");
    }

    if (first == "--version" || first == "-V")
    {
      return ::std::string("version");
    }

    return first;
  }

  CommandArguments CommandRegistry::resolve_command_args(
      const ::std::vector<::std::string> &args) const
  {
    if (args.empty())
    {
      return {};
    }

    const auto &first = args.front();

    if (first == "--help" ||
        first == "-h" ||
        first == "--version" ||
        first == "-V")
    {
      return {};
    }

    return CommandArguments(args.begin() + 1, args.end());
  }

  CliResult CommandRegistry::run(
      const ::std::string &name,
      const CliConfig &config,
      CommandArguments args) const
  {
    auto command = find(name);
    if (!command)
    {
      return CliResult::failure(command.error(), 1);
    }

    if (!config.command_enabled(command.value().name()) &&
        !config.command_enabled(name))
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::CommandDisabled,
              "CLI command is disabled: " + name),
          1);
    }

    CommandContext context;
    context.config = config;
    context.command_name = name;
    context.args = ::std::move(args);

    return command.value().run(context);
  }

  CliResult CommandRegistry::run(
      const CliConfig &config,
      const ::std::vector<::std::string> &args) const
  {
    auto command_name = resolve_command_name(args);
    if (!command_name)
    {
      return CliResult::failure(command_name.error(), 1);
    }

    return run(
        command_name.value(),
        config,
        resolve_command_args(args));
  }

  bool CommandRegistry::conflicts_with_existing_command(
      const Command &command) const noexcept
  {
    if (has_command(command.name()))
    {
      return true;
    }

    for (const auto &alias : command.aliases())
    {
      if (has_command(alias))
      {
        return true;
      }
    }

    for (const auto &existing : commands_)
    {
      if (command.matches(existing.name()))
      {
        return true;
      }

      for (const auto &alias : existing.aliases())
      {
        if (command.matches(alias))
        {
          return true;
        }
      }
    }

    return false;
  }

} // namespace kordex::cli

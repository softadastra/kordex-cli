/**
 *
 *  @file Command.cpp
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

#include <algorithm>
#include <exception>
#include <utility>

#include <kordex/cli/Command.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool contains_empty_alias(
        const ::std::vector<::std::string> &aliases) noexcept
    {
      return ::std::any_of(
          aliases.begin(),
          aliases.end(),
          [](const ::std::string &alias)
          {
            return alias.empty();
          });
    }
  } // namespace

  bool CommandContext::has_args() const noexcept
  {
    return !args.empty();
  }

  ::std::size_t CommandContext::arg_count() const noexcept
  {
    return args.size();
  }

  bool CommandInfo::has_name() const noexcept
  {
    return !name.empty();
  }

  bool CommandInfo::has_aliases() const noexcept
  {
    return !aliases.empty();
  }

  bool CommandInfo::has_summary() const noexcept
  {
    return !summary.empty();
  }

  bool CommandInfo::has_usage() const noexcept
  {
    return !usage.empty();
  }

  Command::Command(
      CommandInfo info,
      CommandHandler handler)
      : info_(::std::move(info)),
        handler_(::std::move(handler))
  {
  }

  Result<Command> Command::create(
      CommandInfo info,
      CommandHandler handler)
  {
    Command command(::std::move(info), ::std::move(handler));

    const auto validation = command.validate();
    if (validation)
    {
      return validation;
    }

    return command;
  }

  const CommandInfo &Command::info() const noexcept
  {
    return info_;
  }

  const ::std::string &Command::name() const noexcept
  {
    return info_.name;
  }

  const ::std::vector<::std::string> &Command::aliases() const noexcept
  {
    return info_.aliases;
  }

  const ::std::string &Command::summary() const noexcept
  {
    return info_.summary;
  }

  const ::std::string &Command::description() const noexcept
  {
    return info_.description;
  }

  const ::std::string &Command::usage() const noexcept
  {
    return info_.usage;
  }

  bool Command::valid() const noexcept
  {
    return info_.has_name() &&
           static_cast<bool>(handler_);
  }

  bool Command::enabled() const noexcept
  {
    return info_.enabled;
  }

  bool Command::hidden() const noexcept
  {
    return info_.hidden;
  }

  bool Command::has_handler() const noexcept
  {
    return static_cast<bool>(handler_);
  }

  bool Command::matches(
      const ::std::string &candidate) const noexcept
  {
    if (candidate == info_.name)
    {
      return true;
    }

    return has_alias(candidate);
  }

  bool Command::has_alias(
      const ::std::string &alias) const noexcept
  {
    return ::std::find(
               info_.aliases.begin(),
               info_.aliases.end(),
               alias) != info_.aliases.end();
  }

  Error Command::validate() const
  {
    if (info_.name.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "command name cannot be empty");
    }

    if (contains_empty_alias(info_.aliases))
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "command aliases cannot contain empty entries");
    }

    if (!handler_)
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "command handler cannot be empty");
    }

    return ok();
  }

  CliResult Command::run(
      const CommandContext &context) const
  {
    const auto validation = validate();
    if (validation)
    {
      return CliResult::failure(validation, 1);
    }

    if (!enabled())
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::CommandDisabled,
              "command is disabled: " + info_.name),
          1);
    }

    try
    {
      auto result = handler_(context);
      result.normalize();
      return result;
    }
    catch (const ::std::exception &exception)
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::InternalError,
              exception.what()),
          1);
    }
    catch (...)
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::InternalError,
              "unknown error while running command"),
          1);
    }
  }

  CliResult Command::run(
      const CliConfig &config,
      CommandArguments args) const
  {
    CommandContext context;
    context.config = config;
    context.command_name = info_.name;
    context.args = ::std::move(args);

    return run(context);
  }

} // namespace kordex::cli

/**
 *
 *  @file Commands.cpp
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

#include <sstream>
#include <utility>

#include <kordex/cli/Commands.hpp>
#include <kordex/cli/Version.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] CliResult help_result(
        const CommandContext &context)
    {
      HelpFormatter formatter = HelpFormatter::from_config(context.config);

      ::std::ostringstream stream;

      stream << formatter.options().product_name << '\n';

      if (formatter.options().has_description())
      {
        stream << formatter.options().description << "\n\n";
      }

      stream << formatter.format_usage() << "\n\n";
      stream << "Commands:\n";

      for (const auto &name : builtin_command_names())
      {
        if (!command_enabled_by_config(context.config, name))
        {
          continue;
        }

        if (name == "help")
        {
          stream << "  help     Show help\n";
        }
        else if (name == "init")
        {
          stream << "  init     Create a new Kordex project\n";
        }
        else if (name == "run")
        {
          stream << "  run      Run a JavaScript or TypeScript file\n";
        }
        else if (name == "check")
        {
          stream << "  check    Check a source file\n";
        }
        else if (name == "build")
        {
          stream << "  build    Build a source file or project\n";
        }
        else if (name == "repl")
        {
          stream << "  repl     Start an interactive Kordex session\n";
        }
        else if (name == "version")
        {
          stream << "  version  Show Kordex version\n";
        }
      }

      if (!context.config.quiet_output())
      {
        stream << '\n'
               << formatter.format_global_options();
      }

      return CliResult::help(stream.str());
    }

    [[nodiscard]] Result<Command> create_command_or_disabled(
        const ::std::string &name,
        Result<Command> command)
    {
      if (!command)
      {
        return command.error();
      }

      if (command.value().name() != name &&
          !command.value().matches(name))
      {
        return make_cli_error(
            CliErrorCode::InternalError,
            "built-in command factory returned unexpected command: " + name);
      }

      return command.value();
    }
  } // namespace

  Result<Command> create_help_command()
  {
    CommandInfo info;
    info.name = "help";
    info.aliases = {"h"};
    info.summary = "Show help";
    info.description = "Show top-level Kordex CLI help.";
    info.usage = "kordex help";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        ::std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return help_result(context);
        });
  }

  ::std::vector<::std::string> builtin_command_names()
  {
    return {
        "help",
        "init",
        "run",
        "check",
        "build",
        "repl",
        "version"};
  }

  bool is_builtin_command(
      const ::std::string &name) noexcept
  {
    return name == "help" ||
           name == "h" ||
           name == "--help" ||
           name == "-h" ||
           name == "init" ||
           name == "new" ||
           name == "run" ||
           name == "check" ||
           name == "build" ||
           name == "repl" ||
           name == "version" ||
           name == "--version" ||
           name == "-V";
  }

  bool command_enabled_by_config(
      const CliConfig &config,
      const ::std::string &name) noexcept
  {
    if (name == "help" || name == "h" || name == "--help" || name == "-h")
    {
      return config.enable_help;
    }

    if (name == "init" || name == "new")
    {
      return config.enable_init;
    }

    if (name == "run")
    {
      return config.enable_run;
    }

    if (name == "check")
    {
      return config.enable_check;
    }

    if (name == "build")
    {
      return config.enable_build;
    }

    if (name == "repl")
    {
      return config.enable_repl;
    }

    if (name == "version" || name == "--version" || name == "-V")
    {
      return config.enable_version;
    }

    return false;
  }

  Result<Command> create_builtin_command(
      const ::std::string &name)
  {
    if (name == "help" || name == "h" || name == "--help" || name == "-h")
    {
      return create_command_or_disabled("help", create_help_command());
    }

    if (name == "init" || name == "new")
    {
      return create_command_or_disabled("init", create_init_command());
    }

    if (name == "run")
    {
      return create_command_or_disabled("run", create_run_command());
    }

    if (name == "check")
    {
      return create_command_or_disabled("check", create_check_command());
    }

    if (name == "build")
    {
      return create_command_or_disabled("build", create_build_command());
    }

    if (name == "repl")
    {
      return create_command_or_disabled("repl", create_repl_command());
    }

    if (name == "version" || name == "--version" || name == "-V")
    {
      return create_command_or_disabled("version", create_version_command());
    }

    return make_cli_error(
        CliErrorCode::CommandNotFound,
        "unknown built-in command: " + name);
  }

  Error register_builtin_command(
      CommandRegistry &registry,
      const ::std::string &name)
  {
    auto command = create_builtin_command(name);
    if (!command)
    {
      return command.error();
    }

    return registry.register_command(::std::move(command.value()));
  }

  Error register_default_commands(
      CommandRegistry &registry,
      const CliConfig &config)
  {
    const auto validation = config.validate();
    if (validation)
    {
      return validation;
    }

    for (const auto &name : builtin_command_names())
    {
      if (!command_enabled_by_config(config, name))
      {
        continue;
      }

      const auto error = register_builtin_command(registry, name);
      if (error)
      {
        return error;
      }
    }

    if (registry.empty())
    {
      return make_cli_error(
          CliErrorCode::CommandDisabled,
          "no built-in CLI command was registered");
    }

    return ok();
  }

  Result<CommandRegistry> create_default_command_registry(
      const CliConfig &config)
  {
    CommandRegistry registry;

    const auto error = register_default_commands(registry, config);
    if (error)
    {
      return error;
    }

    return registry;
  }

  Result<CommandRegistry> create_default_command_registry()
  {
    auto config = CliConfig::from_options(CliOptions::defaults());
    if (!config)
    {
      return config.error();
    }

    return create_default_command_registry(config.value());
  }

} // namespace kordex::cli

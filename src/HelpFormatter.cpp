/**
 *
 *  @file HelpFormatter.cpp
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
#include <sstream>
#include <utility>

#include <kordex/cli/HelpFormatter.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] ::std::size_t max_command_name_width(
        const ::std::vector<Command> &commands) noexcept
    {
      ::std::size_t width = 0;

      for (const auto &command : commands)
      {
        width = (std::max)(width, command.name().size());
      }

      return width;
    }

    void append_padded(
        ::std::ostringstream &stream,
        const ::std::string &value,
        ::std::size_t width)
    {
      stream << value;

      if (value.size() < width)
      {
        stream << ::std::string(width - value.size(), ' ');
      }
    }
  } // namespace

  bool HelpFormatterOptions::has_executable_name() const noexcept
  {
    return !executable_name.empty();
  }

  bool HelpFormatterOptions::has_product_name() const noexcept
  {
    return !product_name.empty();
  }

  bool HelpFormatterOptions::has_description() const noexcept
  {
    return !description.empty();
  }

  HelpFormatter::HelpFormatter()
      : options_()
  {
  }

  HelpFormatter::HelpFormatter(
      HelpFormatterOptions options)
      : options_(::std::move(options))
  {
  }

  HelpFormatter HelpFormatter::from_config(
      const CliConfig &config)
  {
    HelpFormatterOptions options;
    options.executable_name = config.executable_name;
    options.show_hidden = config.debug;
    options.show_aliases = config.enable_aliases;
    options.show_global_options = true;
    options.show_examples = !config.quiet_output();

    return HelpFormatter(options);
  }

  const HelpFormatterOptions &HelpFormatter::options() const noexcept
  {
    return options_;
  }

  Error HelpFormatter::validate() const
  {
    if (options_.executable_name.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "help formatter executable name cannot be empty");
    }

    if (options_.product_name.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "help formatter product name cannot be empty");
    }

    return ok();
  }

  Result<::std::string> HelpFormatter::format(
      const CommandRegistry &registry) const
  {
    const auto validation = validate();
    if (validation)
    {
      return validation;
    }

    ::std::ostringstream stream;

    stream << options_.product_name << '\n';

    if (options_.has_description())
    {
      stream << options_.description << "\n\n";
    }
    else
    {
      stream << '\n';
    }

    stream << format_usage() << "\n\n";

    const auto commands = options_.show_hidden
                              ? registry.commands()
                              : registry.visible_commands();

    stream << format_commands(commands);

    if (options_.show_global_options)
    {
      stream << '\n'
             << format_global_options();
    }

    if (options_.show_examples)
    {
      stream << '\n'
             << format_examples();
    }

    return stream.str();
  }

  Result<::std::string> HelpFormatter::format_command(
      const Command &command) const
  {
    const auto validation = validate();
    if (validation)
    {
      return validation;
    }

    const auto command_validation = command.validate();
    if (command_validation)
    {
      return command_validation;
    }

    ::std::ostringstream stream;

    stream << command.name() << '\n';

    if (!command.summary().empty())
    {
      stream << command.summary() << "\n\n";
    }

    if (!command.description().empty())
    {
      stream << command.description() << "\n\n";
    }

    stream << format_command_usage(command);

    if (options_.show_aliases && !command.aliases().empty())
    {
      stream << "\n\nAliases:\n";
      stream << "  " << format_aliases(command.aliases()) << '\n';
    }

    return stream.str();
  }

  ::std::string HelpFormatter::format_usage() const
  {
    ::std::ostringstream stream;

    stream << "Usage:\n";
    stream << "  " << options_.executable_name << " <command> [options] [args]";

    return stream.str();
  }

  ::std::string HelpFormatter::format_command_usage(
      const Command &command) const
  {
    ::std::ostringstream stream;

    stream << "Usage:\n";

    if (!command.usage().empty())
    {
      stream << "  " << command.usage();
    }
    else
    {
      stream << "  " << options_.executable_name << ' ' << command.name();
    }

    return stream.str();
  }

  ::std::string HelpFormatter::format_commands(
      const ::std::vector<Command> &commands) const
  {
    ::std::ostringstream stream;

    stream << "Commands:\n";

    if (commands.empty())
    {
      stream << "  No commands available.\n";
      return stream.str();
    }

    const auto width = max_command_name_width(commands);

    for (const auto &command : commands)
    {
      stream << "  ";
      append_padded(stream, command.name(), width);
      stream << "  ";

      if (!command.summary().empty())
      {
        stream << command.summary();
      }

      if (options_.show_aliases && !command.aliases().empty())
      {
        stream << " (aliases: " << format_aliases(command.aliases()) << ')';
      }

      stream << '\n';
    }

    return stream.str();
  }

  ::std::string HelpFormatter::format_global_options() const
  {
    ::std::ostringstream stream;

    stream << "Global options:\n";
    stream << "  -h, --help       Show help\n";
    stream << "  -V, --version    Show version\n";
    stream << "  -v, --verbose    Enable verbose output\n";
    stream << "      --debug      Enable debug output\n";
    stream << "  -q, --quiet      Disable normal output\n";
    stream << "      --json       Render machine-readable JSON output\n";
    stream << "      --no-color   Disable colored output\n";
    stream << "      --dry-run    Show what would happen without executing\n";

    return stream.str();
  }

  ::std::string HelpFormatter::format_examples() const
  {
    ::std::ostringstream stream;

    stream << "Examples:\n";
    stream << "  " << options_.executable_name << " init app\n";
    stream << "  " << options_.executable_name << " run main.js\n";
    stream << "  " << options_.executable_name << " check main.js\n";
    stream << "  " << options_.executable_name << " build main.js\n";

    return stream.str();
  }

  ::std::string HelpFormatter::format_aliases(
      const ::std::vector<::std::string> &aliases)
  {
    ::std::ostringstream stream;

    for (::std::size_t index = 0; index < aliases.size(); ++index)
    {
      if (index > 0)
      {
        stream << ", ";
      }

      stream << aliases[index];
    }

    return stream.str();
  }

} // namespace kordex::cli

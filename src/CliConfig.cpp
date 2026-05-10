/**
 *
 *  @file CliConfig.cpp
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
#include <cctype>
#include <string>

#include <kordex/cli/CliConfig.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] ::std::string to_lower(
        ::std::string value)
    {
      ::std::transform(
          value.begin(),
          value.end(),
          value.begin(),
          [](unsigned char character)
          {
            return static_cast<char>(::std::tolower(character));
          });

      return value;
    }

    [[nodiscard]] bool contains_empty_argument(
        const ::std::vector<::std::string> &args) noexcept
    {
      return ::std::any_of(
          args.begin(),
          args.end(),
          [](const ::std::string &arg)
          {
            return arg.empty();
          });
    }
  } // namespace

  Result<CliConfig> CliConfig::from_options(
      const CliOptions &options)
  {
    CliConfig config;

    config.mode = options.mode;
    config.output_mode = options.output_mode;

    config.executable_name = options.executable_name;
    config.working_directory = options.working_directory;
    config.config_path = options.config_path;
    config.args = options.args;

    config.color = options.color;
    config.verbose = options.verbose;
    config.debug = options.debug;
    config.dry_run = options.dry_run;
    config.interactive = options.interactive;
    config.enable_aliases = options.enable_aliases;
    config.allow_unknown_args = options.allow_unknown_args;

    config.enable_help = options.enable_help;
    config.enable_init = options.enable_init;
    config.enable_run = options.enable_run;
    config.enable_check = options.enable_check;
    config.enable_build = options.enable_build;
    config.enable_repl = options.enable_repl;
    config.enable_version = options.enable_version;

    const auto validation = config.validate();
    if (validation)
    {
      return validation;
    }

    return config;
  }

  Result<CliConfig> CliConfig::from_environment(
      const CliOptions &base)
  {
    /*
     * Environment support will be expanded later.
     *
     * For now, this keeps the public API stable and returns the normalized
     * options.
     */
    return from_options(base);
  }

  bool CliConfig::has_executable_name() const noexcept
  {
    return !executable_name.empty();
  }

  bool CliConfig::has_working_directory() const noexcept
  {
    return !working_directory.empty();
  }

  bool CliConfig::has_config_path() const noexcept
  {
    return !config_path.empty();
  }

  bool CliConfig::has_args() const noexcept
  {
    return !args.empty();
  }

  bool CliConfig::text_output() const noexcept
  {
    return output_mode == OutputMode::Text;
  }

  bool CliConfig::json_output() const noexcept
  {
    return output_mode == OutputMode::Json;
  }

  bool CliConfig::quiet_output() const noexcept
  {
    return output_mode == OutputMode::Quiet;
  }

  bool CliConfig::has_enabled_commands() const noexcept
  {
    return enable_help ||
           enable_init ||
           enable_run ||
           enable_check ||
           enable_build ||
           enable_repl ||
           enable_version;
  }

  bool CliConfig::command_enabled(
      const ::std::string &name) const noexcept
  {
    if (name == "help" || name == "--help" || name == "-h")
    {
      return enable_help;
    }

    if (name == "init" || name == "new")
    {
      return enable_init;
    }

    if (name == "run")
    {
      return enable_run;
    }

    if (name == "check")
    {
      return enable_check;
    }

    if (name == "build")
    {
      return enable_build;
    }

    if (name == "repl")
    {
      return enable_repl;
    }

    if (name == "version" ||
        name == "--version" ||
        name == "-V")
    {
      return enable_version;
    }

    return false;
  }

  ::std::size_t CliConfig::enabled_command_count() const noexcept
  {
    ::std::size_t count = 0;

    if (enable_help)
    {
      ++count;
    }

    if (enable_init)
    {
      ++count;
    }

    if (enable_run)
    {
      ++count;
    }

    if (enable_check)
    {
      ++count;
    }

    if (enable_build)
    {
      ++count;
    }

    if (enable_repl)
    {
      ++count;
    }

    if (enable_version)
    {
      ++count;
    }

    return count;
  }

  Error CliConfig::validate() const
  {
    if (executable_name.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "CLI executable name cannot be empty");
    }

    if (!has_enabled_commands())
    {
      return make_cli_error(
          CliErrorCode::CommandDisabled,
          "at least one CLI command must be enabled");
    }

    if (contains_empty_argument(args))
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "CLI arguments cannot contain empty entries");
    }

    if (quiet_output() && verbose)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "quiet output cannot be combined with verbose mode");
    }

    if (quiet_output() && debug)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "quiet output cannot be combined with debug mode");
    }

    if (json_output() && color)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "JSON output cannot use colored text");
    }

    if (dry_run && interactive)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "dry-run mode cannot use interactive prompts");
    }

    return ok();
  }

  Result<CliMode> parse_cli_mode(
      const ::std::string &value)
  {
    const auto normalized = to_lower(value);

    if (normalized == "normal" || normalized == "default")
    {
      return CliMode::Normal;
    }

    if (normalized == "development" || normalized == "dev")
    {
      return CliMode::Development;
    }

    if (normalized == "test" || normalized == "testing")
    {
      return CliMode::Test;
    }

    return make_cli_error(
        CliErrorCode::InvalidArgument,
        "invalid CLI mode: " + value);
  }

  Result<OutputMode> parse_output_mode(
      const ::std::string &value)
  {
    const auto normalized = to_lower(value);

    if (normalized == "text" || normalized == "plain")
    {
      return OutputMode::Text;
    }

    if (normalized == "json")
    {
      return OutputMode::Json;
    }

    if (normalized == "quiet" || normalized == "silent")
    {
      return OutputMode::Quiet;
    }

    return make_cli_error(
        CliErrorCode::InvalidArgument,
        "invalid CLI output mode: " + value);
  }

} // namespace kordex::cli

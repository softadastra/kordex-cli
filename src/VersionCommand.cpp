/**
 *
 *  @file VersionCommand.cpp
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

#include <kordex/runtime/Version.hpp>
#include <kordex/bindings/Version.hpp>
#include <kordex/std/Version.hpp>

#include <kordex/cli/Output.hpp>
#include <kordex/cli/Version.hpp>
#include <kordex/cli/VersionCommand.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool is_flag(
        const ::std::string &value) noexcept
    {
      return value.size() > 1 && value.front() == '-';
    }
  } // namespace

  bool VersionInfo::has_cli_version() const noexcept
  {
    return !cli_version.empty();
  }

  bool VersionInfo::has_runtime_version() const noexcept
  {
    return !runtime_version.empty();
  }

  bool VersionInfo::has_bindings_version() const noexcept
  {
    return !bindings_version.empty();
  }

  bool VersionInfo::has_std_version() const noexcept
  {
    return !std_version.empty();
  }

  Result<VersionCommandOptions> parse_version_options(
      const CommandContext &context)
  {
    VersionCommandOptions options;
    options.json = context.config.json_output();

    for (const auto &arg : context.args)
    {
      if (arg.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "version arguments cannot contain empty entries");
      }

      if (arg == "--details" || arg == "--full")
      {
        options.details = true;
        continue;
      }

      if (arg == "--json")
      {
        options.json = true;
        continue;
      }

      if (is_flag(arg))
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "unknown version option: " + arg);
      }

      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "unexpected version argument: " + arg);
    }

    const auto validation = validate_version_options(options);
    if (validation)
    {
      return validation;
    }

    return options;
  }

  Error validate_version_options(
      const VersionCommandOptions &options)
  {
    (void)options;
    return ok();
  }

  VersionInfo collect_version_info()
  {
    VersionInfo info;

    info.product = "Kordex";
    info.cli_version = ::std::string(kordex::cli::version());
    info.runtime_version = ::std::string(kordex::runtime::version());
    info.bindings_version = ::std::string(kordex::bindings::version());
    info.std_version = ::std::string(kordex::standard::version());

    return info;
  }

  ::std::string render_version_info(
      const VersionInfo &info,
      const VersionCommandOptions &options)
  {
    if (options.json)
    {
      return render_version_info_json(info);
    }

    ::std::ostringstream stream;

    if (!options.details)
    {
      stream << info.product << ' ' << info.cli_version;
      return stream.str();
    }

    stream << info.product << '\n';
    stream << "cli      = " << info.cli_version << '\n';
    stream << "runtime  = " << info.runtime_version << '\n';
    stream << "bindings = " << info.bindings_version << '\n';
    stream << "std      = " << info.std_version;

    return stream.str();
  }

  ::std::string render_version_info_json(
      const VersionInfo &info)
  {
    ::std::ostringstream stream;

    stream << "{";
    stream << "\"product\":\"" << Output::json_escape(info.product) << "\",";
    stream << "\"cli\":\"" << Output::json_escape(info.cli_version) << "\",";
    stream << "\"runtime\":\"" << Output::json_escape(info.runtime_version) << "\",";
    stream << "\"bindings\":\"" << Output::json_escape(info.bindings_version) << "\",";
    stream << "\"std\":\"" << Output::json_escape(info.std_version) << "\"";
    stream << "}";

    return stream.str();
  }

  CliResult run_version_command(
      const CommandContext &context)
  {
    auto options = parse_version_options(context);
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    const auto info = collect_version_info();

    return CliResult::version(
        render_version_info(info, options.value()));
  }

  Result<Command> create_version_command()
  {
    CommandInfo info;
    info.name = "version";
    info.aliases = {"v"};
    info.summary = "Show Kordex version";
    info.description =
        "Show Kordex CLI version information.";
    info.usage = "kordex version [--details] [--json]";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        ::std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return run_version_command(context);
        });
  }

} // namespace kordex::cli

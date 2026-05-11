/**
 *
 *  @file ArgumentParser.cpp
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

#include <utility>
#include <kordex/cli/ArgumentParser.hpp>

namespace kordex::cli
{
  bool ParsedArguments::has_command() const noexcept
  {
    return !command.empty();
  }

  bool ParsedArguments::has_positional() const noexcept
  {
    return !positional.empty();
  }

  bool ParsedArguments::has_unknown() const noexcept
  {
    return !unknown.empty();
  }

  bool ParsedArguments::is_meta_request() const noexcept
  {
    return help || version;
  }

  ArgumentParser::ArgumentParser()
      : options_(CliOptions::defaults())
  {
  }

  ArgumentParser::ArgumentParser(
      CliOptions options)
      : options_(::std::move(options))
  {
  }

  const CliOptions &ArgumentParser::options() const noexcept
  {
    return options_;
  }

  Result<ParsedArguments> ArgumentParser::parse(
      int argc,
      char **argv) const
  {
    auto args = argv_to_vector(argc, argv);
    if (!args)
    {
      return args.error();
    }

    return parse(args.value());
  }

  Result<ParsedArguments> ArgumentParser::parse(
      const ::std::vector<::std::string> &args) const
  {
    return parse_tokens(args);
  }

  Result<CliOptions> ArgumentParser::parse_options(
      const ::std::vector<::std::string> &args) const
  {
    auto parsed = parse(args);
    if (!parsed)
    {
      return parsed.error();
    }

    return to_options(parsed.value());
  }

  Result<CliOptions> ArgumentParser::parse_options(
      int argc,
      char **argv) const
  {
    auto parsed = parse(argc, argv);
    if (!parsed)
    {
      return parsed.error();
    }

    return to_options(parsed.value());
  }

  Result<CliOptions> ArgumentParser::to_options(
      const ParsedArguments &parsed) const
  {
    CliOptions result = options_;

    result.executable_name = parsed.executable_name.empty()
                                 ? options_.executable_name
                                 : parsed.executable_name;

    result.output_mode = parsed.output_mode;
    result.color = !parsed.no_color && result.output_mode == OutputMode::Text;
    result.verbose = parsed.verbose;
    result.debug = parsed.debug;
    result.dry_run = parsed.dry_run;
    result.interactive = !parsed.dry_run &&
                         result.output_mode == OutputMode::Text &&
                         !parsed.quiet;

    result.args.clear();

    if (parsed.has_command())
    {
      result.args.push_back(parsed.command);
    }

    for (const auto &arg : parsed.positional)
    {
      result.args.push_back(arg);
    }

    for (const auto &arg : parsed.unknown)
    {
      result.args.push_back(arg);
    }

    auto config = CliConfig::from_options(result);
    if (!config)
    {
      return config.error();
    }

    return result;
  }

  Result<::std::vector<::std::string>> ArgumentParser::argv_to_vector(
      int argc,
      char **argv)
  {
    if (argc < 0)
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "argc cannot be negative");
    }

    if (argc > 0 && argv == nullptr)
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "argv cannot be null when argc is positive");
    }

    ::std::vector<::std::string> args;
    args.reserve(static_cast<::std::size_t>(argc));

    for (int index = 0; index < argc; ++index)
    {
      if (argv[index] == nullptr)
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "argv entry cannot be null");
      }

      args.emplace_back(argv[index]);
    }

    return args;
  }

  bool ArgumentParser::is_option(
      const ::std::string &arg) noexcept
  {
    return arg.size() > 1 && arg.front() == '-';
  }

  bool ArgumentParser::is_option_terminator(
      const ::std::string &arg) noexcept
  {
    return arg == "--";
  }

  bool ArgumentParser::is_known_global_option(
      const ::std::string &arg) noexcept
  {
    return arg == "--help" ||
           arg == "-h" ||
           arg == "--version" ||
           arg == "-V" ||
           arg == "--verbose" ||
           arg == "-v" ||
           arg == "--debug" ||
           arg == "--quiet" ||
           arg == "-q" ||
           arg == "--json" ||
           arg == "--no-color" ||
           arg == "--dry-run";
  }

  Result<ParsedArguments> ArgumentParser::parse_tokens(
      const ::std::vector<::std::string> &tokens) const
  {
    ParsedArguments parsed;
    parsed.executable_name = options_.executable_name;
    parsed.output_mode = options_.output_mode;
    parsed.verbose = options_.verbose;
    parsed.debug = options_.debug;
    parsed.quiet = options_.quiet_output();
    parsed.json = options_.json_output();
    parsed.no_color = !options_.color;
    parsed.dry_run = options_.dry_run;

    ::std::size_t index = 0;

    if (!tokens.empty())
    {
      parsed.executable_name = tokens.front().empty()
                                   ? options_.executable_name
                                   : tokens.front();
      index = 1;
    }

    bool stop_option_parsing = false;

    for (; index < tokens.size(); ++index)
    {
      const auto &token = tokens[index];

      if (token.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "CLI arguments cannot contain empty entries");
      }

      if (!stop_option_parsing && is_option_terminator(token))
      {
        stop_option_parsing = true;
        continue;
      }

      if (!stop_option_parsing && is_option(token))
      {
        if (token == "--help" || token == "-h")
        {
          parsed.help = true;
          if (!parsed.has_command())
          {
            parsed.command = "help";
          }
          continue;
        }

        if (token == "--version" || token == "-V")
        {
          parsed.version = true;
          if (!parsed.has_command())
          {
            parsed.command = "version";
          }
          continue;
        }

        if (token == "--verbose" || token == "-v")
        {
          parsed.verbose = true;
          continue;
        }

        if (token == "--debug")
        {
          parsed.debug = true;
          continue;
        }

        if (token == "--quiet" || token == "-q")
        {
          parsed.quiet = true;
          parsed.json = false;
          parsed.output_mode = OutputMode::Quiet;
          continue;
        }

        if (token == "--json")
        {
          parsed.json = true;
          parsed.quiet = false;
          parsed.no_color = true;
          parsed.output_mode = OutputMode::Json;
          continue;
        }

        if (token == "--no-color")
        {
          parsed.no_color = true;
          continue;
        }

        if (token == "--dry-run")
        {
          parsed.dry_run = true;
          continue;
        }

        if (!options_.allow_unknown_args)
        {
          return make_cli_error(
              CliErrorCode::ParseError,
              "unknown CLI option: " + token);
        }

        parsed.unknown.push_back(token);
        continue;
      }

      if (!parsed.has_command())
      {
        parsed.command = token;
        continue;
      }

      parsed.positional.push_back(token);
    }

    if (!parsed.has_command() && options_.enable_help)
    {
      parsed.command = "help";
      parsed.help = true;
    }

    if (parsed.quiet && parsed.verbose)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "quiet output cannot be combined with verbose mode");
    }

    if (parsed.quiet && parsed.debug)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "quiet output cannot be combined with debug mode");
    }

    return parsed;
  }

} // namespace kordex::cli

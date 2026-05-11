/**
 *
 *  @file test_argument_parser.cpp
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

#include <string_view>
#include <kordex/cli/ArgumentParser.hpp>

namespace
{
  [[nodiscard]] bool expect_true(
      bool condition,
      const char *message)
  {
    if (!condition)
    {
      (void)message;
      return false;
    }

    return true;
  }

  template <typename T>
  [[nodiscard]] bool result_ok(
      const kordex::cli::Result<T> &result)
  {
    return !(!result);
  }

  [[nodiscard]] bool test_parsed_arguments_helpers()
  {
    kordex::cli::ParsedArguments parsed;
    parsed.command = "run";
    parsed.positional = {"main.js"};
    parsed.unknown = {"--custom"};

    return expect_true(
               parsed.has_command(),
               "parsed arguments should have command") &&
           expect_true(
               parsed.has_positional(),
               "parsed arguments should have positional values") &&
           expect_true(
               parsed.has_unknown(),
               "parsed arguments should have unknown values") &&
           expect_true(
               !parsed.is_meta_request(),
               "parsed arguments should not be meta request");
  }

  [[nodiscard]] bool test_parse_run_command()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse(
        {"kordex", "run", "main.js", "--watch"});

    if (!expect_true(
            result_ok(result),
            "run arguments should parse"))
    {
      return false;
    }

    const auto &parsed = result.value();

    return expect_true(
               parsed.executable_name == "kordex",
               "executable name should match") &&
           expect_true(
               parsed.command == "run",
               "command should be run") &&
           expect_true(
               parsed.positional.size() == 2,
               "positional arg count should match") &&
           expect_true(
               parsed.positional[0] == "main.js",
               "first positional arg should match") &&
           expect_true(
               parsed.positional[1] == "--watch",
               "second positional arg should be preserved after command");
  }

  [[nodiscard]] bool test_parse_help()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse({"kordex", "--help"});

    if (!expect_true(
            result_ok(result),
            "help arguments should parse"))
    {
      return false;
    }

    const auto &parsed = result.value();

    return expect_true(
               parsed.help,
               "help flag should be true") &&
           expect_true(
               parsed.is_meta_request(),
               "help should be meta request") &&
           expect_true(
               parsed.command == "help",
               "help command should be selected") &&
           expect_true(
               parsed.positional.empty(),
               "help should not have positional args");
  }

  [[nodiscard]] bool test_parse_version()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse({"kordex", "-V"});

    if (!expect_true(
            result_ok(result),
            "version arguments should parse"))
    {
      return false;
    }

    const auto &parsed = result.value();

    return expect_true(
               parsed.version,
               "version flag should be true") &&
           expect_true(
               parsed.is_meta_request(),
               "version should be meta request") &&
           expect_true(
               parsed.command == "version",
               "version command should be selected");
  }

  [[nodiscard]] bool test_parse_global_flags()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse(
        {"kordex", "--verbose", "--debug", "--no-color", "--dry-run", "check"});

    if (!expect_true(
            result_ok(result),
            "global flags should parse"))
    {
      return false;
    }

    const auto &parsed = result.value();

    return expect_true(
               parsed.command == "check",
               "command should be check") &&
           expect_true(
               parsed.verbose,
               "verbose should be enabled") &&
           expect_true(
               parsed.debug,
               "debug should be enabled") &&
           expect_true(
               parsed.no_color,
               "no color should be enabled") &&
           expect_true(
               parsed.dry_run,
               "dry run should be enabled");
  }

  [[nodiscard]] bool test_parse_json_output()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse({"kordex", "--json", "check"});

    if (!expect_true(
            result_ok(result),
            "json flag should parse"))
    {
      return false;
    }

    const auto &parsed = result.value();

    return expect_true(
               parsed.command == "check",
               "command should be check") &&
           expect_true(
               parsed.json,
               "json flag should be true") &&
           expect_true(
               !parsed.quiet,
               "quiet should be false") &&
           expect_true(
               parsed.no_color,
               "json should disable color") &&
           expect_true(
               parsed.output_mode == kordex::cli::OutputMode::Json,
               "output mode should be json");
  }

  [[nodiscard]] bool test_parse_quiet_output()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse({"kordex", "--quiet", "check"});

    if (!expect_true(
            result_ok(result),
            "quiet flag should parse"))
    {
      return false;
    }

    const auto &parsed = result.value();

    return expect_true(
               parsed.command == "check",
               "command should be check") &&
           expect_true(
               parsed.quiet,
               "quiet flag should be true") &&
           expect_true(
               !parsed.json,
               "json should be false") &&
           expect_true(
               parsed.output_mode == kordex::cli::OutputMode::Quiet,
               "output mode should be quiet");
  }

  [[nodiscard]] bool test_option_terminator()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse(
        {"kordex", "run", "--", "--not-a-global-option", "main.js"});

    if (!expect_true(
            result_ok(result),
            "option terminator should parse"))
    {
      return false;
    }

    const auto &parsed = result.value();

    return expect_true(
               parsed.command == "run",
               "command should be run") &&
           expect_true(
               parsed.positional.size() == 2,
               "two positional args should remain") &&
           expect_true(
               parsed.positional[0] == "--not-a-global-option",
               "first arg after terminator should be positional") &&
           expect_true(
               parsed.positional[1] == "main.js",
               "second arg after terminator should be positional");
  }

  [[nodiscard]] bool test_empty_args_defaults_to_help()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse({"kordex"});

    if (!expect_true(
            result_ok(result),
            "empty user args should parse"))
    {
      return false;
    }

    return expect_true(
               result.value().command == "help",
               "empty args should default to help") &&
           expect_true(
               result.value().help,
               "empty args should request help");
  }

  [[nodiscard]] bool test_unknown_option_fails_by_default()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse({"kordex", "--unknown"});

    return expect_true(
               !result,
               "unknown option should fail by default") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ParseError,
               "unknown option should map to parse error");
  }

  [[nodiscard]] bool test_unknown_option_can_be_preserved()
  {
    auto options = kordex::cli::CliOptions::defaults();
    options.allow_unknown_args = true;

    kordex::cli::ArgumentParser parser(options);

    auto result = parser.parse({"kordex", "--unknown", "run"});

    if (!expect_true(
            result_ok(result),
            "unknown option should be preserved when allowed"))
    {
      return false;
    }

    return expect_true(
               result.value().unknown.size() == 1,
               "unknown args should contain one item") &&
           expect_true(
               result.value().unknown[0] == "--unknown",
               "unknown arg should match") &&
           expect_true(
               result.value().command == "run",
               "command should still parse");
  }

  [[nodiscard]] bool test_quiet_verbose_conflict()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse({"kordex", "--quiet", "--verbose", "check"});

    return expect_true(
               !result,
               "quiet verbose should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "quiet verbose should map to config error");
  }

  [[nodiscard]] bool test_quiet_debug_conflict()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse({"kordex", "--quiet", "--debug", "check"});

    return expect_true(
               !result,
               "quiet debug should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "quiet debug should map to config error");
  }

  [[nodiscard]] bool test_parse_options()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse_options(
        {"kordex", "--json", "run", "main.js"});

    if (!expect_true(
            result_ok(result),
            "parse_options should succeed"))
    {
      return false;
    }

    const auto &options = result.value();

    return expect_true(
               options.executable_name == "kordex",
               "options executable name should match") &&
           expect_true(
               options.output_mode == kordex::cli::OutputMode::Json,
               "options output mode should be json") &&
           expect_true(
               !options.color,
               "json options should disable color") &&
           expect_true(
               !options.interactive,
               "json options should disable interactive") &&
           expect_true(
               options.args.size() == 2,
               "options args should contain command and positional") &&
           expect_true(
               options.args[0] == "run",
               "options first arg should be command") &&
           expect_true(
               options.args[1] == "main.js",
               "options second arg should be positional");
  }

  [[nodiscard]] bool test_parse_options_dry_run()
  {
    kordex::cli::ArgumentParser parser;

    auto result = parser.parse_options(
        {"kordex", "--dry-run", "build"});

    if (!expect_true(
            result_ok(result),
            "dry run options should parse"))
    {
      return false;
    }

    return expect_true(
               result.value().dry_run,
               "dry run option should be enabled") &&
           expect_true(
               !result.value().interactive,
               "dry run should disable interactive mode");
  }

  [[nodiscard]] bool test_argv_to_vector()
  {
    const char *raw[] = {"kordex", "run", "main.js"};
    char **argv = const_cast<char **>(raw);

    auto result = kordex::cli::ArgumentParser::argv_to_vector(3, argv);

    if (!expect_true(
            result_ok(result),
            "argv_to_vector should succeed"))
    {
      return false;
    }

    return expect_true(
               result.value().size() == 3,
               "argv vector size should match") &&
           expect_true(
               result.value()[0] == "kordex",
               "argv first value should match") &&
           expect_true(
               result.value()[2] == "main.js",
               "argv third value should match");
  }

  [[nodiscard]] bool test_argv_to_vector_invalid()
  {
    auto negative = kordex::cli::ArgumentParser::argv_to_vector(-1, nullptr);
    auto null_argv = kordex::cli::ArgumentParser::argv_to_vector(1, nullptr);

    return expect_true(
               !negative,
               "negative argc should fail") &&
           expect_true(
               negative.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "negative argc should map to invalid argument") &&
           expect_true(
               !null_argv,
               "null argv should fail") &&
           expect_true(
               null_argv.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "null argv should map to invalid argument");
  }

  [[nodiscard]] bool test_static_helpers()
  {
    return expect_true(
               kordex::cli::ArgumentParser::is_option("--help"),
               "--help should be option") &&
           expect_true(
               kordex::cli::ArgumentParser::is_option("-v"),
               "-v should be option") &&
           expect_true(
               !kordex::cli::ArgumentParser::is_option("run"),
               "run should not be option") &&
           expect_true(
               kordex::cli::ArgumentParser::is_option_terminator("--"),
               "-- should be terminator") &&
           expect_true(
               kordex::cli::ArgumentParser::is_known_global_option("--json"),
               "--json should be known global") &&
           expect_true(
               !kordex::cli::ArgumentParser::is_known_global_option("--custom"),
               "--custom should not be known global");
  }
} // namespace

int main()
{
  const bool ok =
      test_parsed_arguments_helpers() &&
      test_parse_run_command() &&
      test_parse_help() &&
      test_parse_version() &&
      test_parse_global_flags() &&
      test_parse_json_output() &&
      test_parse_quiet_output() &&
      test_option_terminator() &&
      test_empty_args_defaults_to_help() &&
      test_unknown_option_fails_by_default() &&
      test_unknown_option_can_be_preserved() &&
      test_quiet_verbose_conflict() &&
      test_quiet_debug_conflict() &&
      test_parse_options() &&
      test_parse_options_dry_run() &&
      test_argv_to_vector() &&
      test_argv_to_vector_invalid() &&
      test_static_helpers();

  return ok ? 0 : 1;
}

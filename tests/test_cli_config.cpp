/**
 *
 *  @file test_cli_config.cpp
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
#include <kordex/cli/CliConfig.hpp>

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

  [[nodiscard]] bool test_from_default_options()
  {
    auto result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::defaults());

    if (!expect_true(
            result_ok(result),
            "default CLI config should be valid"))
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.mode == kordex::cli::CliMode::Normal,
               "default config mode should be normal") &&
           expect_true(
               config.output_mode == kordex::cli::OutputMode::Text,
               "default config output should be text") &&
           expect_true(
               ::std::string_view(config.executable_name) == "kordex",
               "default config executable name should match") &&
           expect_true(
               config.has_executable_name(),
               "default config should have executable name") &&
           expect_true(
               !config.has_working_directory(),
               "default config should not have working directory") &&
           expect_true(
               !config.has_config_path(),
               "default config should not have config path") &&
           expect_true(
               !config.has_args(),
               "default config should not have args") &&
           expect_true(
               config.text_output(),
               "default config should use text output") &&
           expect_true(
               config.has_enabled_commands(),
               "default config should have enabled commands") &&
           expect_true(
               config.enabled_command_count() == 7,
               "default config should enable seven commands");
  }

  [[nodiscard]] bool test_from_development_options()
  {
    auto result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::development());

    if (!expect_true(
            result_ok(result),
            "development CLI config should be valid"))
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.mode == kordex::cli::CliMode::Development,
               "development config mode should match") &&
           expect_true(
               config.verbose,
               "development config should be verbose") &&
           expect_true(
               config.debug,
               "development config should enable debug") &&
           expect_true(
               config.color,
               "development config should enable color") &&
           expect_true(
               config.interactive,
               "development config should be interactive");
  }

  [[nodiscard]] bool test_from_test_options()
  {
    auto result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(result),
            "test CLI config should be valid"))
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.mode == kordex::cli::CliMode::Test,
               "test config mode should match") &&
           expect_true(
               ::std::string_view(config.executable_name) == "kordex-test",
               "test config executable name should match") &&
           expect_true(
               !config.color,
               "test config should disable color") &&
           expect_true(
               config.debug,
               "test config should enable debug") &&
           expect_true(
               !config.interactive,
               "test config should disable interactive mode");
  }

  [[nodiscard]] bool test_from_json_options()
  {
    auto result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::json());

    if (!expect_true(
            result_ok(result),
            "json CLI config should be valid"))
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.json_output(),
               "json config should use json output") &&
           expect_true(
               !config.text_output(),
               "json config should not use text output") &&
           expect_true(
               !config.quiet_output(),
               "json config should not use quiet output") &&
           expect_true(
               !config.color,
               "json config should disable color") &&
           expect_true(
               !config.interactive,
               "json config should disable interactive mode");
  }

  [[nodiscard]] bool test_from_quiet_options()
  {
    auto result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::quiet());

    if (!expect_true(
            result_ok(result),
            "quiet CLI config should be valid"))
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.quiet_output(),
               "quiet config should use quiet output") &&
           expect_true(
               !config.text_output(),
               "quiet config should not use text output") &&
           expect_true(
               !config.json_output(),
               "quiet config should not use json output") &&
           expect_true(
               !config.color,
               "quiet config should disable color") &&
           expect_true(
               !config.verbose,
               "quiet config should disable verbose") &&
           expect_true(
               !config.debug,
               "quiet config should disable debug") &&
           expect_true(
               !config.interactive,
               "quiet config should disable interactive mode");
  }

  [[nodiscard]] bool test_from_environment()
  {
    auto options = kordex::cli::CliOptions::test();
    options.args.push_back("run");

    auto result = kordex::cli::CliConfig::from_environment(options);

    if (!expect_true(
            result_ok(result),
            "CLI config from environment should be valid"))
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.mode == kordex::cli::CliMode::Test,
               "environment config should preserve mode") &&
           expect_true(
               config.has_args(),
               "environment config should preserve args") &&
           expect_true(
               config.args.size() == 1,
               "environment config args size should match");
  }

  [[nodiscard]] bool test_helpers()
  {
    auto options = kordex::cli::CliOptions::defaults();
    options.working_directory = "/tmp/kordex";
    options.config_path = "kordex.json";
    options.args.push_back("run");

    auto result = kordex::cli::CliConfig::from_options(options);

    if (!expect_true(
            result_ok(result),
            "configured CLI config should be valid"))
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.has_executable_name(),
               "config should have executable name") &&
           expect_true(
               config.has_working_directory(),
               "config should have working directory") &&
           expect_true(
               config.has_config_path(),
               "config should have config path") &&
           expect_true(
               config.has_args(),
               "config should have args") &&
           expect_true(
               config.text_output(),
               "config should use text output") &&
           expect_true(
               config.has_enabled_commands(),
               "config should have enabled commands");
  }

  [[nodiscard]] bool test_command_enabled()
  {
    auto result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::defaults());

    if (!expect_true(
            result_ok(result),
            "default CLI config should be valid"))
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.command_enabled("help"),
               "help should be enabled") &&
           expect_true(
               config.command_enabled("--help"),
               "--help should be enabled") &&
           expect_true(
               config.command_enabled("-h"),
               "-h should be enabled") &&
           expect_true(
               config.command_enabled("init"),
               "init should be enabled") &&
           expect_true(
               config.command_enabled("new"),
               "new should alias init") &&
           expect_true(
               config.command_enabled("run"),
               "run should be enabled") &&
           expect_true(
               config.command_enabled("check"),
               "check should be enabled") &&
           expect_true(
               config.command_enabled("build"),
               "build should be enabled") &&
           expect_true(
               config.command_enabled("repl"),
               "repl should be enabled") &&
           expect_true(
               config.command_enabled("version"),
               "version should be enabled") &&
           expect_true(
               config.command_enabled("--version"),
               "--version should be enabled") &&
           expect_true(
               config.command_enabled("-V"),
               "-V should be enabled") &&
           expect_true(
               !config.command_enabled("unknown"),
               "unknown should not be enabled");
  }

  [[nodiscard]] bool test_parse_cli_mode()
  {
    auto normal = kordex::cli::parse_cli_mode("normal");
    auto dev = kordex::cli::parse_cli_mode("dev");
    auto development = kordex::cli::parse_cli_mode("development");
    auto test = kordex::cli::parse_cli_mode("test");
    auto invalid = kordex::cli::parse_cli_mode("invalid");

    return expect_true(
               result_ok(normal),
               "normal mode should parse") &&
           expect_true(
               result_ok(dev),
               "dev mode should parse") &&
           expect_true(
               result_ok(development),
               "development mode should parse") &&
           expect_true(
               result_ok(test),
               "test mode should parse") &&
           expect_true(
               normal.value() == kordex::cli::CliMode::Normal,
               "normal parsed value should match") &&
           expect_true(
               dev.value() == kordex::cli::CliMode::Development,
               "dev parsed value should match") &&
           expect_true(
               development.value() == kordex::cli::CliMode::Development,
               "development parsed value should match") &&
           expect_true(
               test.value() == kordex::cli::CliMode::Test,
               "test parsed value should match") &&
           expect_true(
               !invalid,
               "invalid mode should fail") &&
           expect_true(
               invalid.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "invalid mode should map to invalid argument");
  }

  [[nodiscard]] bool test_parse_output_mode()
  {
    auto text = kordex::cli::parse_output_mode("text");
    auto plain = kordex::cli::parse_output_mode("plain");
    auto json = kordex::cli::parse_output_mode("json");
    auto quiet = kordex::cli::parse_output_mode("quiet");
    auto silent = kordex::cli::parse_output_mode("silent");
    auto invalid = kordex::cli::parse_output_mode("xml");

    return expect_true(
               result_ok(text),
               "text output should parse") &&
           expect_true(
               result_ok(plain),
               "plain output should parse") &&
           expect_true(
               result_ok(json),
               "json output should parse") &&
           expect_true(
               result_ok(quiet),
               "quiet output should parse") &&
           expect_true(
               result_ok(silent),
               "silent output should parse") &&
           expect_true(
               text.value() == kordex::cli::OutputMode::Text,
               "text parsed value should match") &&
           expect_true(
               plain.value() == kordex::cli::OutputMode::Text,
               "plain parsed value should match") &&
           expect_true(
               json.value() == kordex::cli::OutputMode::Json,
               "json parsed value should match") &&
           expect_true(
               quiet.value() == kordex::cli::OutputMode::Quiet,
               "quiet parsed value should match") &&
           expect_true(
               silent.value() == kordex::cli::OutputMode::Quiet,
               "silent parsed value should match") &&
           expect_true(
               !invalid,
               "invalid output mode should fail") &&
           expect_true(
               invalid.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "invalid output mode should map to invalid argument");
  }

  [[nodiscard]] bool test_invalid_empty_executable_name()
  {
    auto options = kordex::cli::CliOptions::defaults();
    options.executable_name.clear();

    auto result = kordex::cli::CliConfig::from_options(options);

    return expect_true(
               !result,
               "empty executable name should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "empty executable name should map to config error");
  }

  [[nodiscard]] bool test_invalid_no_enabled_commands()
  {
    auto options = kordex::cli::CliOptions::defaults();

    options.enable_help = false;
    options.enable_init = false;
    options.enable_run = false;
    options.enable_check = false;
    options.enable_build = false;
    options.enable_repl = false;
    options.enable_version = false;

    auto result = kordex::cli::CliConfig::from_options(options);

    return expect_true(
               !result,
               "no enabled commands should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::PermissionDenied,
               "no enabled commands should map to permission denied");
  }

  [[nodiscard]] bool test_invalid_empty_argument()
  {
    auto options = kordex::cli::CliOptions::defaults();
    options.args.push_back("");

    auto result = kordex::cli::CliConfig::from_options(options);

    return expect_true(
               !result,
               "empty argument should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "empty argument should map to invalid argument");
  }

  [[nodiscard]] bool test_invalid_quiet_verbose()
  {
    auto options = kordex::cli::CliOptions::quiet();
    options.verbose = true;

    auto result = kordex::cli::CliConfig::from_options(options);

    return expect_true(
               !result,
               "quiet verbose config should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "quiet verbose should map to config error");
  }

  [[nodiscard]] bool test_invalid_quiet_debug()
  {
    auto options = kordex::cli::CliOptions::quiet();
    options.debug = true;

    auto result = kordex::cli::CliConfig::from_options(options);

    return expect_true(
               !result,
               "quiet debug config should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "quiet debug should map to config error");
  }

  [[nodiscard]] bool test_invalid_json_color()
  {
    auto options = kordex::cli::CliOptions::json();
    options.color = true;

    auto result = kordex::cli::CliConfig::from_options(options);

    return expect_true(
               !result,
               "json color config should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "json color should map to config error");
  }

  [[nodiscard]] bool test_invalid_dry_run_interactive()
  {
    auto options = kordex::cli::CliOptions::defaults();
    options.dry_run = true;
    options.interactive = true;

    auto result = kordex::cli::CliConfig::from_options(options);

    return expect_true(
               !result,
               "dry-run interactive config should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "dry-run interactive should map to config error");
  }
} // namespace

int main()
{
  const bool ok =
      test_from_default_options() &&
      test_from_development_options() &&
      test_from_test_options() &&
      test_from_json_options() &&
      test_from_quiet_options() &&
      test_from_environment() &&
      test_helpers() &&
      test_command_enabled() &&
      test_parse_cli_mode() &&
      test_parse_output_mode() &&
      test_invalid_empty_executable_name() &&
      test_invalid_no_enabled_commands() &&
      test_invalid_empty_argument() &&
      test_invalid_quiet_verbose() &&
      test_invalid_quiet_debug() &&
      test_invalid_json_color() &&
      test_invalid_dry_run_interactive();

  return ok ? 0 : 1;
}

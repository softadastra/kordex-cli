/**
 *
 *  @file test_cli_options.cpp
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
#include <kordex/cli/CliOptions.hpp>

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

  [[nodiscard]] bool test_default_options()
  {
    const auto options = kordex::cli::CliOptions::defaults();

    return expect_true(
               options.mode == kordex::cli::CliMode::Normal,
               "default mode should be normal") &&
           expect_true(
               options.output_mode == kordex::cli::OutputMode::Text,
               "default output mode should be text") &&
           expect_true(
               options.has_executable_name(),
               "default options should have executable name") &&
           expect_true(
               ::std::string_view(options.executable_name) == "kordex",
               "default executable name should match") &&
           expect_true(
               !options.has_working_directory(),
               "default working directory should be empty") &&
           expect_true(
               !options.has_config_path(),
               "default config path should be empty") &&
           expect_true(
               !options.has_args(),
               "default args should be empty") &&
           expect_true(
               options.color,
               "default color should be enabled") &&
           expect_true(
               !options.verbose,
               "default verbose should be disabled") &&
           expect_true(
               !options.debug,
               "default debug should be disabled") &&
           expect_true(
               !options.dry_run,
               "default dry run should be disabled") &&
           expect_true(
               options.interactive,
               "default interactive should be enabled") &&
           expect_true(
               options.enable_aliases,
               "default aliases should be enabled") &&
           expect_true(
               !options.allow_unknown_args,
               "default unknown args should be disabled") &&
           expect_true(
               options.has_enabled_commands(),
               "default options should have enabled commands");
  }

  [[nodiscard]] bool test_development_options()
  {
    const auto options = kordex::cli::CliOptions::development();

    return expect_true(
               options.mode == kordex::cli::CliMode::Development,
               "development mode should match") &&
           expect_true(
               options.text_output(),
               "development should use text output") &&
           expect_true(
               options.color,
               "development color should be enabled") &&
           expect_true(
               options.verbose,
               "development verbose should be enabled") &&
           expect_true(
               options.debug,
               "development debug should be enabled") &&
           expect_true(
               options.interactive,
               "development interactive should be enabled") &&
           expect_true(
               options.enable_init &&
                   options.enable_run &&
                   options.enable_check &&
                   options.enable_build &&
                   options.enable_repl &&
                   options.enable_install &&
                   options.enable_update &&
                   options.enable_version,
               "development should enable commands");
  }

  [[nodiscard]] bool test_test_options()
  {
    const auto options = kordex::cli::CliOptions::test();

    return expect_true(
               options.mode == kordex::cli::CliMode::Test,
               "test mode should match") &&
           expect_true(
               ::std::string_view(options.executable_name) == "kordex-test",
               "test executable name should match") &&
           expect_true(
               !options.color,
               "test color should be disabled") &&
           expect_true(
               !options.verbose,
               "test verbose should be disabled") &&
           expect_true(
               options.debug,
               "test debug should be enabled") &&
           expect_true(
               !options.interactive,
               "test interactive should be disabled") &&
           expect_true(
               options.has_enabled_commands(),
               "test should enable commands");
  }

  [[nodiscard]] bool test_quiet_options()
  {
    const auto options = kordex::cli::CliOptions::quiet();

    return expect_true(
               options.mode == kordex::cli::CliMode::Normal,
               "quiet mode should remain normal") &&
           expect_true(
               options.quiet_output(),
               "quiet output should be selected") &&
           expect_true(
               !options.text_output(),
               "quiet should not be text output") &&
           expect_true(
               !options.json_output(),
               "quiet should not be json output") &&
           expect_true(
               !options.color,
               "quiet color should be disabled") &&
           expect_true(
               !options.verbose,
               "quiet verbose should be disabled") &&
           expect_true(
               !options.debug,
               "quiet debug should be disabled") &&
           expect_true(
               !options.interactive,
               "quiet interactive should be disabled");
  }

  [[nodiscard]] bool test_json_options()
  {
    const auto options = kordex::cli::CliOptions::json();

    return expect_true(
               options.mode == kordex::cli::CliMode::Normal,
               "json mode should remain normal") &&
           expect_true(
               options.json_output(),
               "json output should be selected") &&
           expect_true(
               !options.text_output(),
               "json should not be text output") &&
           expect_true(
               !options.quiet_output(),
               "json should not be quiet output") &&
           expect_true(
               !options.color,
               "json color should be disabled") &&
           expect_true(
               !options.interactive,
               "json interactive should be disabled");
  }

  [[nodiscard]] bool test_helpers()
  {
    auto options = kordex::cli::CliOptions::defaults();

    const bool initial_state =
        options.has_executable_name() &&
        !options.has_working_directory() &&
        !options.has_config_path() &&
        !options.has_args() &&
        options.text_output() &&
        options.has_enabled_commands();

    options.working_directory = "/tmp/kordex";
    options.config_path = "kordex.json";
    options.args.push_back("run");

    const bool configured_state =
        options.has_working_directory() &&
        options.has_config_path() &&
        options.has_args();

    options.enable_help = false;
    options.enable_init = false;
    options.enable_run = false;
    options.enable_check = false;
    options.enable_build = false;
    options.enable_repl = false;
    options.enable_install = false;
    options.enable_update = false;
    options.enable_version = false;

    const bool disabled_state =
        !options.has_enabled_commands();

    return expect_true(
               initial_state,
               "initial helper state should match") &&
           expect_true(
               configured_state,
               "configured helper state should match") &&
           expect_true(
               disabled_state,
               "disabled command state should match");
  }

  [[nodiscard]] bool test_cli_mode_strings()
  {
    return expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliMode::Normal)) == "normal",
               "normal mode string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliMode::Development)) == "development",
               "development mode string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::CliMode::Test)) == "test",
               "test mode string should match");
  }

  [[nodiscard]] bool test_output_mode_strings()
  {
    return expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::OutputMode::Text)) == "text",
               "text output string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::OutputMode::Json)) == "json",
               "json output string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::OutputMode::Quiet)) == "quiet",
               "quiet output string should match");
  }
} // namespace

int main()
{
  const bool ok =
      test_default_options() &&
      test_development_options() &&
      test_test_options() &&
      test_quiet_options() &&
      test_json_options() &&
      test_helpers() &&
      test_cli_mode_strings() &&
      test_output_mode_strings();

  return ok ? 0 : 1;
}

/**
 *
 *  @file test_cli.cpp
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

#include <sstream>
#include <string_view>
#include <vector>

#include <kordex/cli/Cli.hpp>

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

  [[nodiscard]] bool contains(
      const ::std::string &text,
      ::std::string_view needle)
  {
    return text.find(needle) != ::std::string::npos;
  }

  [[nodiscard]] kordex::cli::Command make_test_command()
  {
    kordex::cli::CommandInfo info;
    info.name = "custom";
    info.aliases = {"c"};
    info.summary = "Custom command";
    info.description = "Custom command used by tests.";
    info.usage = "kordex custom";
    info.hidden = false;
    info.enabled = true;

    auto result = kordex::cli::Command::create(
        info,
        [](const kordex::cli::CommandContext &context) -> kordex::cli::CliResult
        {
          return kordex::cli::CliResult::success(
              "custom:" + context.command_name);
        });

    return result.value();
  }

  [[nodiscard]] bool test_run_options_helpers()
  {
    kordex::cli::CliRunOptions options;

    const bool empty_state = !options.has_args();

    options.args.push_back("version");

    return expect_true(
               empty_state,
               "default run options should have no args") &&
           expect_true(
               options.has_args(),
               "run options should have args after push");
  }

  [[nodiscard]] bool test_create_default_cli()
  {
    auto result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::defaults());

    if (!expect_true(
            result_ok(result),
            "default CLI creation should succeed"))
    {
      return false;
    }

    const auto &cli = result.value();

    return expect_true(
               cli.has_commands(),
               "default CLI should have commands") &&
           expect_true(
               !cli.validate(),
               "default CLI should validate") &&
           expect_true(
               cli.registry().has_command("help"),
               "default CLI should have help command") &&
           expect_true(
               cli.registry().has_command("init"),
               "default CLI should have init command") &&
           expect_true(
               cli.registry().has_command("run"),
               "default CLI should have run command") &&
           expect_true(
               cli.registry().has_command("check"),
               "default CLI should have check command") &&
           expect_true(
               cli.registry().has_command("build"),
               "default CLI should have build command") &&
           expect_true(
               cli.registry().has_command("repl"),
               "default CLI should have repl command") &&
           expect_true(
               cli.registry().has_command("version"),
               "default CLI should have version command");
  }

  [[nodiscard]] bool test_default_constructor()
  {
    kordex::cli::Cli cli;

    return expect_true(
               cli.has_commands(),
               "default constructed CLI should have commands") &&
           expect_true(
               cli.registry().has_command("help"),
               "default constructed CLI should have help") &&
           expect_true(
               cli.registry().has_command("version"),
               "default constructed CLI should have version");
  }

  [[nodiscard]] bool test_construct_from_options()
  {
    auto options = kordex::cli::CliOptions::test();

    kordex::cli::Cli cli(options);

    return expect_true(
               cli.has_commands(),
               "CLI from options should have commands") &&
           expect_true(
               cli.config().mode == kordex::cli::CliMode::Test,
               "CLI config mode should match options") &&
           expect_true(
               ::std::string_view(cli.config().executable_name) == "kordex-test",
               "CLI executable name should match options") &&
           expect_true(
               cli.registry().has_command("version"),
               "CLI from options should have version command");
  }

  [[nodiscard]] bool test_construct_from_config_and_registry()
  {
    auto config_result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(config_result),
            "test config should be valid"))
    {
      return false;
    }

    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_test_command());

    if (!expect_true(
            !register_error,
            "custom command should register"))
    {
      return false;
    }

    kordex::cli::Cli cli(config_result.value(), registry);

    return expect_true(
               cli.has_commands(),
               "CLI should have custom command") &&
           expect_true(
               cli.registry().has_command("custom"),
               "CLI registry should contain custom command") &&
           expect_true(
               cli.registry().has_command("c"),
               "CLI registry should contain custom alias") &&
           expect_true(
               !cli.validate(),
               "CLI with custom registry should validate");
  }

  [[nodiscard]] bool test_from_args()
  {
    const char *raw[] = {"kordex", "version"};
    char **argv = const_cast<char **>(raw);

    auto result = kordex::cli::Cli::from_args(2, argv);

    if (!expect_true(
            result_ok(result),
            "CLI from args should succeed"))
    {
      return false;
    }

    return expect_true(
               result.value().has_commands(),
               "CLI from args should have commands") &&
           expect_true(
               result.value().config().has_args(),
               "CLI from args config should have args") &&
           expect_true(
               result.value().config().args.size() == 1,
               "CLI from args config should contain command") &&
           expect_true(
               result.value().config().args[0] == "version",
               "CLI from args command should be version");
  }

  [[nodiscard]] bool test_from_args_invalid()
  {
    auto result = kordex::cli::Cli::from_args(1, nullptr);

    return expect_true(
               !result,
               "CLI from invalid args should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "CLI from invalid args should map to invalid argument");
  }

  [[nodiscard]] bool test_register_command()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    auto cli = ::std::move(cli_result.value());

    const auto error = cli.register_command(make_test_command());

    return expect_true(
               !error,
               "custom command registration should succeed") &&
           expect_true(
               cli.registry().has_command("custom"),
               "CLI registry should contain custom command") &&
           expect_true(
               cli.registry().has_command("c"),
               "CLI registry should contain custom alias");
  }

  [[nodiscard]] bool test_register_duplicate_command_fails()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    auto cli = ::std::move(cli_result.value());

    const auto first_error = cli.register_command(make_test_command());
    const auto second_error = cli.register_command(make_test_command());

    return expect_true(
               !first_error,
               "first custom command registration should succeed") &&
           expect_true(
               second_error.has_error(),
               "duplicate custom command registration should fail") &&
           expect_true(
               second_error.code() == vix::error::ErrorCode::AlreadyExists,
               "duplicate command should map to already exists");
  }

  [[nodiscard]] bool test_run_version_command()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const auto result = cli_result.value().run(
        {"kordex-test", "version"});

    return expect_true(
               result.version_requested(),
               "version command should return version result") &&
           expect_true(
               result.exit_code == 0,
               "version command exit code should be zero") &&
           expect_true(
               contains(result.output, "Kordex"),
               "version output should contain product name");
  }

  [[nodiscard]] bool test_run_version_short_flag()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const auto result = cli_result.value().run(
        {"kordex-test", "-V"});

    return expect_true(
               result.version_requested(),
               "-V should return version result") &&
           expect_true(
               result.exit_code == 0,
               "-V exit code should be zero") &&
           expect_true(
               contains(result.output, "Kordex"),
               "-V output should contain product name");
  }

  [[nodiscard]] bool test_run_help_command()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const auto result = cli_result.value().run(
        {"kordex-test", "help"});

    return expect_true(
               result.help_requested(),
               "help command should return help result") &&
           expect_true(
               result.exit_code == 0,
               "help command exit code should be zero") &&
           expect_true(
               contains(result.output, "Usage:"),
               "help output should contain usage") &&
           expect_true(
               contains(result.output, "Commands:"),
               "help output should contain commands");
  }

  [[nodiscard]] bool test_run_help_short_flag()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const auto result = cli_result.value().run(
        {"kordex-test", "--help"});

    return expect_true(
               result.help_requested(),
               "--help should return help result") &&
           expect_true(
               contains(result.output, "Usage:"),
               "--help output should contain usage");
  }

  [[nodiscard]] bool test_run_without_args_defaults_to_help()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const auto result = cli_result.value().run(
        {"kordex-test"});

    return expect_true(
               result.help_requested(),
               "empty command should default to help") &&
           expect_true(
               contains(result.output, "Usage:"),
               "default help should contain usage");
  }

  [[nodiscard]] bool test_run_unknown_command()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const auto result = cli_result.value().run(
        {"kordex-test", "unknown"});

    return expect_true(
               result.failed(),
               "unknown command should fail") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::NotFound,
               "unknown command should map to not found");
  }

  [[nodiscard]] bool test_run_command_direct()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    auto cli = ::std::move(cli_result.value());

    const auto register_error = cli.register_command(make_test_command());

    if (!expect_true(
            !register_error,
            "custom command should register"))
    {
      return false;
    }

    const auto result = cli.run_command("custom");

    return expect_true(
               result.succeeded(),
               "direct custom command should succeed") &&
           expect_true(
               result.output == "custom:custom",
               "direct custom command output should match");
  }

  [[nodiscard]] bool test_run_command_alias_direct()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    auto cli = ::std::move(cli_result.value());

    const auto register_error = cli.register_command(make_test_command());

    if (!expect_true(
            !register_error,
            "custom command should register"))
    {
      return false;
    }

    const auto result = cli.run_command("c");

    return expect_true(
               result.succeeded(),
               "direct alias command should succeed") &&
           expect_true(
               result.output == "custom:c",
               "direct alias output should preserve requested name");
  }

  [[nodiscard]] bool test_run_with_streams_success()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    ::std::ostringstream out;
    ::std::ostringstream err;

    const auto result = cli_result.value().run(
        {"kordex-test", "version"},
        out,
        err);

    return expect_true(
               result.version_requested(),
               "stream version command should return version") &&
           expect_true(
               contains(out.str(), "Kordex"),
               "stream version output should be written to stdout") &&
           expect_true(
               err.str().empty(),
               "stream version should not write stderr");
  }

  [[nodiscard]] bool test_run_with_streams_failure()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    ::std::ostringstream out;
    ::std::ostringstream err;

    const auto result = cli_result.value().run(
        {"kordex-test", "unknown"},
        out,
        err);

    return expect_true(
               result.failed(),
               "stream unknown command should fail") &&
           expect_true(
               out.str().empty(),
               "stream failure should not write stdout") &&
           expect_true(
               contains(err.str(), "CLI command not found") ||
                   contains(err.str(), "unknown"),
               "stream failure should write error to stderr");
  }

  [[nodiscard]] bool test_run_argc_argv()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const char *raw[] = {"kordex-test", "version"};
    char **argv = const_cast<char **>(raw);

    const auto result = cli_result.value().run(2, argv);

    return expect_true(
               result.version_requested(),
               "argc argv version should return version") &&
           expect_true(
               result.exit_code == 0,
               "argc argv version should exit zero");
  }

  [[nodiscard]] bool test_run_argc_argv_streams()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const char *raw[] = {"kordex-test", "--help"};
    char **argv = const_cast<char **>(raw);

    ::std::ostringstream out;
    ::std::ostringstream err;

    const auto result = cli_result.value().run(2, argv, out, err);

    return expect_true(
               result.help_requested(),
               "argc argv stream help should return help") &&
           expect_true(
               contains(out.str(), "Usage:"),
               "argc argv stream help should write stdout") &&
           expect_true(
               err.str().empty(),
               "argc argv stream help should not write stderr");
  }

  [[nodiscard]] bool test_help_top_level()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    auto help = cli_result.value().help();

    if (!expect_true(
            result_ok(help),
            "top-level help should render"))
    {
      return false;
    }

    return expect_true(
               contains(help.value(), "Usage:"),
               "top-level help should contain usage") &&
           expect_true(
               contains(help.value(), "Commands:"),
               "top-level help should contain commands");
  }

  [[nodiscard]] bool test_help_command()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    auto help = cli_result.value().help("version");

    if (!expect_true(
            result_ok(help),
            "command help should render"))
    {
      return false;
    }

    return expect_true(
               contains(help.value(), "version"),
               "command help should contain version") &&
           expect_true(
               contains(help.value(), "Usage:"),
               "command help should contain usage");
  }

  [[nodiscard]] bool test_help_unknown_command()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    auto help = cli_result.value().help("unknown");

    return expect_true(
               !help,
               "unknown command help should fail") &&
           expect_true(
               help.error().code() == vix::error::ErrorCode::NotFound,
               "unknown command help should map to not found");
  }

  [[nodiscard]] bool test_version_text()
  {
    auto cli_result = kordex::cli::Cli::create(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(cli_result),
            "CLI creation should succeed"))
    {
      return false;
    }

    const auto text = cli_result.value().version_text();

    return expect_true(
               contains(text, "Kordex"),
               "version text should contain product") &&
           expect_true(
               contains(text, kordex::cli::version()),
               "version text should contain CLI version");
  }

  [[nodiscard]] bool test_create_invalid_options()
  {
    auto options = kordex::cli::CliOptions::defaults();
    options.executable_name.clear();

    auto result = kordex::cli::Cli::create(options);

    return expect_true(
               !result,
               "CLI creation with invalid options should fail") &&
           expect_true(
               result.error().code() == vix::error::ErrorCode::ConfigError,
               "invalid CLI options should map to config error");
  }

  [[nodiscard]] bool test_validate_without_commands_fails()
  {
    auto config_result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(config_result),
            "test config should be valid"))
    {
      return false;
    }

    kordex::cli::CommandRegistry registry;
    kordex::cli::Cli cli(config_result.value(), registry);

    const auto error = cli.validate();

    return expect_true(
               error.has_error(),
               "CLI without commands should fail validation") &&
           expect_true(
               error.code() == vix::error::ErrorCode::NotFound,
               "CLI without commands should map to not found");
  }

  [[nodiscard]] bool test_run_cli_helper_success()
  {
    const char *raw[] = {"kordex", "version"};
    char **argv = const_cast<char **>(raw);

    ::std::ostringstream out;
    ::std::ostringstream err;

    const int exit_code = kordex::cli::run_cli(2, argv, out, err);

    return expect_true(
               exit_code == 0,
               "run_cli helper should exit zero for version") &&
           expect_true(
               contains(out.str(), "Kordex"),
               "run_cli helper should write version output") &&
           expect_true(
               err.str().empty(),
               "run_cli helper should not write stderr for version");
  }

  [[nodiscard]] bool test_run_cli_helper_invalid_args()
  {
    ::std::ostringstream out;
    ::std::ostringstream err;

    const int exit_code = kordex::cli::run_cli(1, nullptr, out, err);

    return expect_true(
               exit_code != 0,
               "run_cli helper should fail for invalid argv") &&
           expect_true(
               out.str().empty(),
               "run_cli invalid args should not write stdout") &&
           expect_true(
               !err.str().empty(),
               "run_cli invalid args should write stderr");
  }

} // namespace

int main()
{
  const bool ok =
      test_run_options_helpers() &&
      test_create_default_cli() &&
      test_default_constructor() &&
      test_construct_from_options() &&
      test_construct_from_config_and_registry() &&
      test_from_args() &&
      test_from_args_invalid() &&
      test_register_command() &&
      test_register_duplicate_command_fails() &&
      test_run_version_command() &&
      test_run_version_short_flag() &&
      test_run_help_command() &&
      test_run_help_short_flag() &&
      test_run_without_args_defaults_to_help() &&
      test_run_unknown_command() &&
      test_run_command_direct() &&
      test_run_command_alias_direct() &&
      test_run_with_streams_success() &&
      test_run_with_streams_failure() &&
      test_run_argc_argv() &&
      test_run_argc_argv_streams() &&
      test_help_top_level() &&
      test_help_command() &&
      test_help_unknown_command() &&
      test_version_text() &&
      test_create_invalid_options() &&
      test_validate_without_commands_fails() &&
      test_run_cli_helper_success() &&
      test_run_cli_helper_invalid_args();

  return ok ? 0 : 1;
}

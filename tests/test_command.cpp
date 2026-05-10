/**
 *
 *  @file test_command.cpp
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

#include <stdexcept>
#include <string_view>

#include <kordex/cli/Command.hpp>

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

  [[nodiscard]] kordex::cli::CommandInfo make_info()
  {
    kordex::cli::CommandInfo info;
    info.name = "run";
    info.aliases = {"r", "execute"};
    info.summary = "Run a script";
    info.description = "Run a JavaScript or TypeScript file with Kordex.";
    info.usage = "kordex run <file>";
    info.hidden = false;
    info.enabled = true;
    return info;
  }

  [[nodiscard]] kordex::cli::CommandHandler success_handler()
  {
    return [](const kordex::cli::CommandContext &context)
    {
      return kordex::cli::CliResult::success(
          "command=" + context.command_name);
    };
  }

  [[nodiscard]] bool test_command_info_helpers()
  {
    const auto info = make_info();

    return expect_true(
               info.has_name(),
               "command info should have name") &&
           expect_true(
               info.has_aliases(),
               "command info should have aliases") &&
           expect_true(
               info.has_summary(),
               "command info should have summary") &&
           expect_true(
               info.has_usage(),
               "command info should have usage");
  }

  [[nodiscard]] bool test_command_context_helpers()
  {
    kordex::cli::CommandContext context;
    context.command_name = "run";
    context.args = {"main.js", "--watch"};

    return expect_true(
               context.has_args(),
               "context should have args") &&
           expect_true(
               context.arg_count() == 2,
               "context arg count should match");
  }

  [[nodiscard]] bool test_create_command()
  {
    auto result = kordex::cli::Command::create(
        make_info(),
        success_handler());

    if (!expect_true(
            result_ok(result),
            "command creation should succeed"))
    {
      return false;
    }

    const auto &command = result.value();

    return expect_true(
               command.valid(),
               "command should be valid") &&
           expect_true(
               command.enabled(),
               "command should be enabled") &&
           expect_true(
               !command.hidden(),
               "command should not be hidden") &&
           expect_true(
               command.has_handler(),
               "command should have handler") &&
           expect_true(
               ::std::string_view(command.name()) == "run",
               "command name should match") &&
           expect_true(
               ::std::string_view(command.summary()) == "Run a script",
               "command summary should match") &&
           expect_true(
               ::std::string_view(command.usage()) == "kordex run <file>",
               "command usage should match") &&
           expect_true(
               command.aliases().size() == 2,
               "command alias count should match");
  }

  [[nodiscard]] bool test_command_matches()
  {
    auto result = kordex::cli::Command::create(
        make_info(),
        success_handler());

    if (!expect_true(
            result_ok(result),
            "command creation should succeed"))
    {
      return false;
    }

    const auto &command = result.value();

    return expect_true(
               command.matches("run"),
               "command should match primary name") &&
           expect_true(
               command.matches("r"),
               "command should match alias r") &&
           expect_true(
               command.matches("execute"),
               "command should match alias execute") &&
           expect_true(
               command.has_alias("r"),
               "command should have alias r") &&
           expect_true(
               !command.matches("build"),
               "command should not match another name") &&
           expect_true(
               !command.has_alias("unknown"),
               "command should not have unknown alias");
  }

  [[nodiscard]] bool test_run_command_with_context()
  {
    auto command_result = kordex::cli::Command::create(
        make_info(),
        [](const kordex::cli::CommandContext &context)
        {
          if (context.command_name != "run")
          {
            return kordex::cli::CliResult::failure(
                kordex::cli::make_cli_error(
                    kordex::cli::CliErrorCode::InvalidArgument,
                    "wrong command"),
                1);
          }

          return kordex::cli::CliResult::success("ok");
        });

    if (!expect_true(
            result_ok(command_result),
            "command creation should succeed"))
    {
      return false;
    }

    kordex::cli::CommandContext context;
    context.config = kordex::cli::CliConfig::from_options(
                         kordex::cli::CliOptions::test())
                         .value();
    context.command_name = "run";
    context.args = {"main.js"};

    const auto result = command_result.value().run(context);

    return expect_true(
               result.succeeded(),
               "command run should succeed") &&
           expect_true(
               result.exit_code == 0,
               "command exit code should be zero") &&
           expect_true(
               ::std::string_view(result.output) == "ok",
               "command output should match");
  }

  [[nodiscard]] bool test_run_command_with_config_and_args()
  {
    auto command_result = kordex::cli::Command::create(
        make_info(),
        [](const kordex::cli::CommandContext &context)
        {
          return kordex::cli::CliResult::success(
              context.has_args() ? context.args.front() : "none");
        });

    if (!expect_true(
            result_ok(command_result),
            "command creation should succeed"))
    {
      return false;
    }

    auto config = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(config),
            "test config should be valid"))
    {
      return false;
    }

    const auto result = command_result.value().run(
        config.value(),
        {"main.js"});

    return expect_true(
               result.succeeded(),
               "command run should succeed") &&
           expect_true(
               ::std::string_view(result.output) == "main.js",
               "command should receive args");
  }

  [[nodiscard]] bool test_disabled_command()
  {
    auto info = make_info();
    info.enabled = false;

    auto command_result = kordex::cli::Command::create(
        info,
        success_handler());

    if (!expect_true(
            result_ok(command_result),
            "disabled command creation should succeed"))
    {
      return false;
    }

    auto config = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(config),
            "test config should be valid"))
    {
      return false;
    }

    const auto result = command_result.value().run(config.value());

    return expect_true(
               result.failed(),
               "disabled command should fail") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::PermissionDenied,
               "disabled command should map to permission denied");
  }

  [[nodiscard]] bool test_handler_failure_is_preserved()
  {
    auto command_result = kordex::cli::Command::create(
        make_info(),
        [](const kordex::cli::CommandContext &)
        {
          return kordex::cli::CliResult::failure(
              kordex::cli::make_cli_error(
                  kordex::cli::CliErrorCode::ParseError,
                  "parse failed"),
              3);
        });

    if (!expect_true(
            result_ok(command_result),
            "command creation should succeed"))
    {
      return false;
    }

    auto config = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(config),
            "test config should be valid"))
    {
      return false;
    }

    const auto result = command_result.value().run(config.value());

    return expect_true(
               result.failed(),
               "handler failure should fail") &&
           expect_true(
               result.exit_code == 3,
               "handler failure exit code should be preserved") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::ParseError,
               "handler failure error should be preserved");
  }

  [[nodiscard]] bool test_handler_exception_becomes_failure()
  {
    auto command_result = kordex::cli::Command::create(
        make_info(),
        [](const kordex::cli::CommandContext &) -> kordex::cli::CliResult
        {
          throw ::std::runtime_error("boom");
        });

    if (!expect_true(
            result_ok(command_result),
            "command creation should succeed"))
    {
      return false;
    }

    auto config = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(config),
            "test config should be valid"))
    {
      return false;
    }

    const auto result = command_result.value().run(config.value());

    return expect_true(
               result.failed(),
               "throwing handler should fail") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::InternalError,
               "throwing handler should map to internal error") &&
           expect_true(
               ::std::string_view(result.error.message()) == "boom",
               "exception message should be preserved");
  }

  [[nodiscard]] bool test_invalid_empty_name()
  {
    auto info = make_info();
    info.name.clear();

    auto result = kordex::cli::Command::create(
        info,
        success_handler());

    return expect_true(
               !result,
               "empty command name should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "empty command name should map to invalid argument");
  }

  [[nodiscard]] bool test_invalid_empty_alias()
  {
    auto info = make_info();
    info.aliases.push_back("");

    auto result = kordex::cli::Command::create(
        info,
        success_handler());

    return expect_true(
               !result,
               "empty command alias should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "empty command alias should map to invalid argument");
  }

  [[nodiscard]] bool test_invalid_empty_handler()
  {
    auto result = kordex::cli::Command::create(
        make_info(),
        {});

    return expect_true(
               !result,
               "empty command handler should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "empty handler should map to invalid argument");
  }
} // namespace

int main()
{
  const bool ok =
      test_command_info_helpers() &&
      test_command_context_helpers() &&
      test_create_command() &&
      test_command_matches() &&
      test_run_command_with_context() &&
      test_run_command_with_config_and_args() &&
      test_disabled_command() &&
      test_handler_failure_is_preserved() &&
      test_handler_exception_becomes_failure() &&
      test_invalid_empty_name() &&
      test_invalid_empty_alias() &&
      test_invalid_empty_handler();

  return ok ? 0 : 1;
}

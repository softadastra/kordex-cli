/**
 *
 *  @file test_command_registry.cpp
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
#include <kordex/cli/CommandRegistry.hpp>

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

  [[nodiscard]] kordex::cli::Command make_command(
      ::std::string name,
      ::std::vector<::std::string> aliases = {},
      bool hidden = false,
      bool enabled = true)
  {
    kordex::cli::CommandInfo info;
    info.name = ::std::move(name);
    info.aliases = ::std::move(aliases);
    info.summary = "Test command";
    info.description = "Test command description.";
    info.usage = "kordex test";
    info.hidden = hidden;
    info.enabled = enabled;

    auto result = kordex::cli::Command::create(
        info,
        [](const kordex::cli::CommandContext &context)
        {
          return kordex::cli::CliResult::success(
              "ran:" + context.command_name);
        });

    return result.value();
  }

  [[nodiscard]] kordex::cli::CliConfig test_config()
  {
    return kordex::cli::CliConfig::from_options(
               kordex::cli::CliOptions::test())
        .value();
  }

  [[nodiscard]] bool test_empty_registry()
  {
    kordex::cli::CommandRegistry registry;

    return expect_true(
               registry.empty(),
               "new registry should be empty") &&
           expect_true(
               registry.size() == 0,
               "new registry size should be zero") &&
           expect_true(
               registry.commands().empty(),
               "new registry commands should be empty") &&
           expect_true(
               registry.visible_commands().empty(),
               "new registry visible commands should be empty") &&
           expect_true(
               !registry.has_command("run"),
               "new registry should not contain run");
  }

  [[nodiscard]] bool test_register_command()
  {
    kordex::cli::CommandRegistry registry;

    const auto error = registry.register_command(
        make_command("run", {"r"}));

    return expect_true(
               !error,
               "register command should succeed") &&
           expect_true(
               !registry.empty(),
               "registry should not be empty") &&
           expect_true(
               registry.size() == 1,
               "registry size should be one") &&
           expect_true(
               registry.has_command("run"),
               "registry should contain command name") &&
           expect_true(
               registry.has_command("r"),
               "registry should contain command alias");
  }

  [[nodiscard]] bool test_register_multiple_commands()
  {
    kordex::cli::CommandRegistry registry;

    const auto run_error = registry.register_command(
        make_command("run", {"r"}));

    const auto build_error = registry.register_command(
        make_command("build", {"b"}));

    const auto check_error = registry.register_command(
        make_command("check", {"c"}));

    return expect_true(
               !run_error && !build_error && !check_error,
               "multiple command registration should succeed") &&
           expect_true(
               registry.size() == 3,
               "registry size should be three") &&
           expect_true(
               registry.has_command("run") &&
                   registry.has_command("build") &&
                   registry.has_command("check"),
               "registry should contain all names") &&
           expect_true(
               registry.has_command("r") &&
                   registry.has_command("b") &&
                   registry.has_command("c"),
               "registry should contain all aliases");
  }

  [[nodiscard]] bool test_register_duplicate_name()
  {
    kordex::cli::CommandRegistry registry;

    const auto first_error = registry.register_command(
        make_command("run", {"r"}));

    const auto second_error = registry.register_command(
        make_command("run", {"execute"}));

    return expect_true(
               !first_error,
               "first command should register") &&
           expect_true(
               second_error.has_error(),
               "duplicate command name should fail") &&
           expect_true(
               second_error.code() ==
                   vix::error::ErrorCode::AlreadyExists,
               "duplicate command should map to already exists") &&
           expect_true(
               registry.size() == 1,
               "duplicate command should not be inserted");
  }

  [[nodiscard]] bool test_register_duplicate_alias()
  {
    kordex::cli::CommandRegistry registry;

    const auto first_error = registry.register_command(
        make_command("run", {"r"}));

    const auto second_error = registry.register_command(
        make_command("build", {"r"}));

    return expect_true(
               !first_error,
               "first command should register") &&
           expect_true(
               second_error.has_error(),
               "duplicate alias should fail") &&
           expect_true(
               second_error.code() ==
                   vix::error::ErrorCode::AlreadyExists,
               "duplicate alias should map to already exists") &&
           expect_true(
               registry.size() == 1,
               "duplicate alias command should not be inserted");
  }

  [[nodiscard]] bool test_register_name_conflicts_with_existing_alias()
  {
    kordex::cli::CommandRegistry registry;

    const auto first_error = registry.register_command(
        make_command("run", {"start"}));

    const auto second_error = registry.register_command(
        make_command("start"));

    return expect_true(
               !first_error,
               "first command should register") &&
           expect_true(
               second_error.has_error(),
               "name conflicting with alias should fail") &&
           expect_true(
               second_error.code() ==
                   vix::error::ErrorCode::AlreadyExists,
               "name alias conflict should map to already exists");
  }

  [[nodiscard]] bool test_register_invalid_command()
  {
    kordex::cli::CommandRegistry registry;

    kordex::cli::Command invalid;

    const auto error = registry.register_command(invalid);

    return expect_true(
               error.has_error(),
               "invalid command should fail registration") &&
           expect_true(
               error.code() == vix::error::ErrorCode::InvalidArgument,
               "invalid command should map to invalid argument") &&
           expect_true(
               registry.empty(),
               "invalid command should not be inserted");
  }

  [[nodiscard]] bool test_find_command_by_name_and_alias()
  {
    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_command("run", {"r", "execute"}));

    if (!expect_true(
            !register_error,
            "command should register"))
    {
      return false;
    }

    auto by_name = registry.find("run");
    auto by_alias = registry.find("execute");

    if (!expect_true(
            result_ok(by_name) && result_ok(by_alias),
            "command should be found by name and alias"))
    {
      return false;
    }

    return expect_true(
               ::std::string_view(by_name.value().name()) == "run",
               "found by name should match") &&
           expect_true(
               ::std::string_view(by_alias.value().name()) == "run",
               "found by alias should return primary command");
  }

  [[nodiscard]] bool test_find_unknown_command()
  {
    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_command("run", {"r"}));

    if (!expect_true(
            !register_error,
            "command should register"))
    {
      return false;
    }

    auto result = registry.find("unknown");

    return expect_true(
               !result,
               "unknown command should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::NotFound,
               "unknown command should map to not found");
  }

  [[nodiscard]] bool test_find_empty_command_name()
  {
    kordex::cli::CommandRegistry registry;

    auto result = registry.find("");

    return expect_true(
               !result,
               "empty command name should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "empty command name should map to invalid argument");
  }

  [[nodiscard]] bool test_visible_commands()
  {
    kordex::cli::CommandRegistry registry;

    const auto visible_error = registry.register_command(
        make_command("run", {}, false));

    const auto hidden_error = registry.register_command(
        make_command("internal", {}, true));

    if (!expect_true(
            !visible_error && !hidden_error,
            "commands should register"))
    {
      return false;
    }

    const auto visible = registry.visible_commands();

    return expect_true(
               registry.size() == 2,
               "registry should contain two commands") &&
           expect_true(
               visible.size() == 1,
               "visible command list should contain one command") &&
           expect_true(
               ::std::string_view(visible.front().name()) == "run",
               "visible command should be run");
  }

  [[nodiscard]] bool test_clear()
  {
    kordex::cli::CommandRegistry registry;

    const auto run_error = registry.register_command(
        make_command("run"));

    const auto build_error = registry.register_command(
        make_command("build"));

    if (!expect_true(
            !run_error && !build_error,
            "commands should register"))
    {
      return false;
    }

    registry.clear();

    return expect_true(
               registry.empty(),
               "registry should be empty after clear") &&
           expect_true(
               registry.size() == 0,
               "registry size should be zero after clear") &&
           expect_true(
               !registry.has_command("run"),
               "registry should not have run after clear");
  }

  [[nodiscard]] bool test_resolve_command_name()
  {
    kordex::cli::CommandRegistry registry;

    const auto help_error = registry.register_command(
        make_command("help"));

    if (!expect_true(
            !help_error,
            "help command should register"))
    {
      return false;
    }

    auto empty = registry.resolve_command_name({});
    auto help_long = registry.resolve_command_name({"--help"});
    auto help_short = registry.resolve_command_name({"-h"});
    auto version_long = registry.resolve_command_name({"--version"});
    auto version_short = registry.resolve_command_name({"-V"});
    auto run = registry.resolve_command_name({"run", "main.js"});

    return expect_true(
               result_ok(empty),
               "empty args should resolve to help") &&
           expect_true(
               result_ok(help_long) &&
                   result_ok(help_short) &&
                   result_ok(version_long) &&
                   result_ok(version_short) &&
                   result_ok(run),
               "special command names should resolve") &&
           expect_true(
               empty.value() == "help",
               "empty args should map to help") &&
           expect_true(
               help_long.value() == "help",
               "--help should map to help") &&
           expect_true(
               help_short.value() == "help",
               "-h should map to help") &&
           expect_true(
               version_long.value() == "version",
               "--version should map to version") &&
           expect_true(
               version_short.value() == "version",
               "-V should map to version") &&
           expect_true(
               run.value() == "run",
               "run should remain run");
  }

  [[nodiscard]] bool test_resolve_empty_without_help()
  {
    kordex::cli::CommandRegistry registry;

    auto result = registry.resolve_command_name({});

    return expect_true(
               !result,
               "empty args without help should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::NotFound,
               "empty args without help should map to not found");
  }

  [[nodiscard]] bool test_resolve_command_args()
  {
    kordex::cli::CommandRegistry registry;

    auto run_args = registry.resolve_command_args(
        {"run", "main.js", "--watch"});

    auto help_args = registry.resolve_command_args({"--help"});
    auto version_args = registry.resolve_command_args({"--version"});
    auto empty_args = registry.resolve_command_args({});

    return expect_true(
               run_args.size() == 2,
               "run args should contain two values") &&
           expect_true(
               run_args[0] == "main.js",
               "first run arg should match") &&
           expect_true(
               run_args[1] == "--watch",
               "second run arg should match") &&
           expect_true(
               help_args.empty(),
               "help args should be empty") &&
           expect_true(
               version_args.empty(),
               "version args should be empty") &&
           expect_true(
               empty_args.empty(),
               "empty args should remain empty");
  }

  [[nodiscard]] bool test_run_command_by_name()
  {
    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_command("run", {"r"}));

    if (!expect_true(
            !register_error,
            "run command should register"))
    {
      return false;
    }

    const auto result = registry.run(
        "run",
        test_config(),
        {"main.js"});

    return expect_true(
               result.succeeded(),
               "run command should succeed") &&
           expect_true(
               ::std::string_view(result.output) == "ran:run",
               "run command output should match");
  }

  [[nodiscard]] bool test_run_command_by_alias()
  {
    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_command("run", {"r"}));

    if (!expect_true(
            !register_error,
            "run command should register"))
    {
      return false;
    }

    const auto result = registry.run(
        "r",
        test_config(),
        {"main.js"});

    return expect_true(
               result.succeeded(),
               "run command by alias should succeed") &&
           expect_true(
               ::std::string_view(result.output) == "ran:r",
               "run command alias output should preserve requested name");
  }

  [[nodiscard]] bool test_run_command_from_args()
  {
    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_command("run", {"r"}));

    if (!expect_true(
            !register_error,
            "run command should register"))
    {
      return false;
    }

    const auto result = registry.run(
        test_config(),
        {"run", "main.js"});

    return expect_true(
               result.succeeded(),
               "run command from args should succeed") &&
           expect_true(
               ::std::string_view(result.output) == "ran:run",
               "run from args output should match");
  }

  [[nodiscard]] bool test_run_unknown_command()
  {
    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_command("run"));

    if (!expect_true(
            !register_error,
            "run command should register"))
    {
      return false;
    }

    const auto result = registry.run(
        "unknown",
        test_config());

    return expect_true(
               result.failed(),
               "unknown command run should fail") &&
           expect_true(
               result.error.code() == vix::error::ErrorCode::NotFound,
               "unknown command run should map to not found");
  }

  [[nodiscard]] bool test_run_disabled_command_by_config()
  {
    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_command("run"));

    if (!expect_true(
            !register_error,
            "run command should register"))
    {
      return false;
    }

    auto config = test_config();
    config.enable_run = false;

    const auto result = registry.run(
        "run",
        config);

    return expect_true(
               result.failed(),
               "disabled command by config should fail") &&
           expect_true(
               result.error.code() ==
                   vix::error::ErrorCode::PermissionDenied,
               "disabled command by config should map to permission denied");
  }

  [[nodiscard]] bool test_run_disabled_command_object()
  {
    kordex::cli::CommandRegistry registry;

    const auto register_error = registry.register_command(
        make_command("run", {}, false, false));

    if (!expect_true(
            !register_error,
            "disabled command object should register"))
    {
      return false;
    }

    const auto result = registry.run(
        "run",
        test_config());

    return expect_true(
               result.failed(),
               "disabled command object should fail") &&
           expect_true(
               result.error.code() ==
                   vix::error::ErrorCode::PermissionDenied,
               "disabled command object should map to permission denied");
  }
} // namespace

int main()
{
  const bool ok =
      test_empty_registry() &&
      test_register_command() &&
      test_register_multiple_commands() &&
      test_register_duplicate_name() &&
      test_register_duplicate_alias() &&
      test_register_name_conflicts_with_existing_alias() &&
      test_register_invalid_command() &&
      test_find_command_by_name_and_alias() &&
      test_find_unknown_command() &&
      test_find_empty_command_name() &&
      test_visible_commands() &&
      test_clear() &&
      test_resolve_command_name() &&
      test_resolve_empty_without_help() &&
      test_resolve_command_args() &&
      test_run_command_by_name() &&
      test_run_command_by_alias() &&
      test_run_command_from_args() &&
      test_run_unknown_command() &&
      test_run_disabled_command_by_config() &&
      test_run_disabled_command_object();

  return ok ? 0 : 1;
}

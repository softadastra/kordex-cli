/**
 *
 *  @file test_help_formatter.cpp
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

#include <string_view>
#include <kordex/cli/HelpFormatter.hpp>

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

  [[nodiscard]] kordex::cli::Command make_command(
      ::std::string name,
      ::std::vector<::std::string> aliases = {},
      bool hidden = false)
  {
    kordex::cli::CommandInfo info;
    info.name = ::std::move(name);
    info.aliases = ::std::move(aliases);
    info.summary = "Test summary";
    info.description = "Test command description.";
    info.usage = "kordex " + info.name + " [args]";
    info.hidden = hidden;
    info.enabled = true;

    return kordex::cli::Command::create(
               info,
               [](const kordex::cli::CommandContext &)
               {
                 return kordex::cli::CliResult::success();
               })
        .value();
  }

  [[nodiscard]] kordex::cli::CommandRegistry make_registry()
  {
    kordex::cli::CommandRegistry registry;

    (void)registry.register_command(
        make_command("run", {"r"}));

    (void)registry.register_command(
        make_command("build", {"b"}));

    (void)registry.register_command(
        make_command("internal", {}, true));

    return registry;
  }

  [[nodiscard]] bool test_options_helpers()
  {
    kordex::cli::HelpFormatterOptions options;

    return expect_true(
               options.has_executable_name(),
               "formatter options should have executable name") &&
           expect_true(
               options.has_product_name(),
               "formatter options should have product name") &&
           expect_true(
               options.has_description(),
               "formatter options should have description") &&
           expect_true(
               options.show_aliases,
               "formatter should show aliases by default") &&
           expect_true(
               options.show_global_options,
               "formatter should show global options by default") &&
           expect_true(
               options.show_examples,
               "formatter should show examples by default");
  }

  [[nodiscard]] bool test_default_formatter()
  {
    kordex::cli::HelpFormatter formatter;

    return expect_true(
               !formatter.validate(),
               "default formatter should validate") &&
           expect_true(
               formatter.options().executable_name == "kordex",
               "default executable name should match") &&
           expect_true(
               formatter.options().product_name == "Kordex",
               "default product name should match");
  }

  [[nodiscard]] bool test_formatter_from_config()
  {
    auto config_result = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            result_ok(config_result),
            "test config should be valid"))
    {
      return false;
    }

    auto formatter = kordex::cli::HelpFormatter::from_config(
        config_result.value());

    return expect_true(
               formatter.options().executable_name == "kordex-test",
               "formatter executable name should come from config") &&
           expect_true(
               !formatter.options().show_hidden,
               "formatter should not show hidden without debug false") &&
           expect_true(
               formatter.options().show_aliases,
               "formatter should preserve alias setting");
  }

  [[nodiscard]] bool test_format_usage()
  {
    kordex::cli::HelpFormatter formatter;

    const auto usage = formatter.format_usage();

    return expect_true(
               contains(usage, "Usage:"),
               "usage should contain heading") &&
           expect_true(
               contains(usage, "kordex <command> [options] [args]"),
               "usage should contain default command shape");
  }

  [[nodiscard]] bool test_format_command_usage()
  {
    kordex::cli::HelpFormatter formatter;
    auto command = make_command("run", {"r"});

    const auto usage = formatter.format_command_usage(command);

    return expect_true(
               contains(usage, "Usage:"),
               "command usage should contain heading") &&
           expect_true(
               contains(usage, "kordex run [args]"),
               "command usage should contain command usage");
  }

  [[nodiscard]] bool test_format_aliases()
  {
    const auto aliases = kordex::cli::HelpFormatter::format_aliases(
        {"r", "execute", "start"});

    return expect_true(
        aliases == "r, execute, start",
        "aliases should be comma-separated");
  }

  [[nodiscard]] bool test_format_commands_visible_only()
  {
    kordex::cli::HelpFormatter formatter;
    auto registry = make_registry();

    const auto commands_text = formatter.format_commands(
        registry.visible_commands());

    return expect_true(
               contains(commands_text, "Commands:"),
               "commands text should contain heading") &&
           expect_true(
               contains(commands_text, "run"),
               "commands text should contain run") &&
           expect_true(
               contains(commands_text, "build"),
               "commands text should contain build") &&
           expect_true(
               contains(commands_text, "aliases: r"),
               "commands text should contain run alias") &&
           expect_true(
               !contains(commands_text, "internal"),
               "commands text should not contain hidden command");
  }

  [[nodiscard]] bool test_format_empty_commands()
  {
    kordex::cli::HelpFormatter formatter;

    const auto text = formatter.format_commands({});

    return expect_true(
               contains(text, "Commands:"),
               "empty commands should contain heading") &&
           expect_true(
               contains(text, "No commands available."),
               "empty commands should show empty message");
  }

  [[nodiscard]] bool test_format_global_options()
  {
    kordex::cli::HelpFormatter formatter;

    const auto text = formatter.format_global_options();

    return expect_true(
               contains(text, "Global options:"),
               "global options should contain heading") &&
           expect_true(
               contains(text, "--help"),
               "global options should contain help") &&
           expect_true(
               contains(text, "--version"),
               "global options should contain version") &&
           expect_true(
               contains(text, "--json"),
               "global options should contain json") &&
           expect_true(
               contains(text, "--dry-run"),
               "global options should contain dry-run");
  }

  [[nodiscard]] bool test_format_examples()
  {
    kordex::cli::HelpFormatter formatter;

    const auto text = formatter.format_examples();

    return expect_true(
               contains(text, "Examples:"),
               "examples should contain heading") &&
           expect_true(
               contains(text, "kordex init app"),
               "examples should contain init") &&
           expect_true(
               contains(text, "kordex run main.js"),
               "examples should contain run") &&
           expect_true(
               contains(text, "kordex build main.js"),
               "examples should contain build");
  }

  [[nodiscard]] bool test_format_top_level_help()
  {
    kordex::cli::HelpFormatter formatter;
    auto registry = make_registry();

    auto result = formatter.format(registry);

    if (!expect_true(
            result_ok(result),
            "top-level help should format"))
    {
      return false;
    }

    const auto &text = result.value();

    return expect_true(
               contains(text, "Kordex"),
               "help should contain product name") &&
           expect_true(
               contains(text, "Usage:"),
               "help should contain usage") &&
           expect_true(
               contains(text, "Commands:"),
               "help should contain commands") &&
           expect_true(
               contains(text, "Global options:"),
               "help should contain global options") &&
           expect_true(
               contains(text, "Examples:"),
               "help should contain examples") &&
           expect_true(
               contains(text, "run"),
               "help should contain run") &&
           expect_true(
               contains(text, "build"),
               "help should contain build") &&
           expect_true(
               !contains(text, "internal"),
               "help should hide hidden command by default");
  }

  [[nodiscard]] bool test_format_top_level_help_with_hidden()
  {
    kordex::cli::HelpFormatterOptions options;
    options.show_hidden = true;

    kordex::cli::HelpFormatter formatter(options);
    auto registry = make_registry();

    auto result = formatter.format(registry);

    if (!expect_true(
            result_ok(result),
            "top-level help with hidden should format"))
    {
      return false;
    }

    return expect_true(
        contains(result.value(), "internal"),
        "help should contain hidden command when enabled");
  }

  [[nodiscard]] bool test_format_command_help()
  {
    kordex::cli::HelpFormatter formatter;
    auto command = make_command("run", {"r", "execute"});

    auto result = formatter.format_command(command);

    if (!expect_true(
            result_ok(result),
            "command help should format"))
    {
      return false;
    }

    const auto &text = result.value();

    return expect_true(
               contains(text, "run"),
               "command help should contain command name") &&
           expect_true(
               contains(text, "Test summary"),
               "command help should contain summary") &&
           expect_true(
               contains(text, "Test command description."),
               "command help should contain description") &&
           expect_true(
               contains(text, "Usage:"),
               "command help should contain usage") &&
           expect_true(
               contains(text, "Aliases:"),
               "command help should contain aliases") &&
           expect_true(
               contains(text, "r, execute"),
               "command help should contain alias list");
  }

  [[nodiscard]] bool test_format_command_without_aliases()
  {
    kordex::cli::HelpFormatterOptions options;
    options.show_aliases = false;

    kordex::cli::HelpFormatter formatter(options);
    auto command = make_command("run", {"r", "execute"});

    auto result = formatter.format_command(command);

    if (!expect_true(
            result_ok(result),
            "command help without aliases should format"))
    {
      return false;
    }

    return expect_true(
        !contains(result.value(), "Aliases:"),
        "command help should hide aliases when disabled");
  }

  [[nodiscard]] bool test_invalid_formatter_empty_executable()
  {
    kordex::cli::HelpFormatterOptions options;
    options.executable_name.clear();

    kordex::cli::HelpFormatter formatter(options);

    auto result = formatter.format(make_registry());

    return expect_true(
               !result,
               "formatter with empty executable should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "empty executable should map to config error");
  }

  [[nodiscard]] bool test_invalid_formatter_empty_product()
  {
    kordex::cli::HelpFormatterOptions options;
    options.product_name.clear();

    kordex::cli::HelpFormatter formatter(options);

    auto result = formatter.format(make_registry());

    return expect_true(
               !result,
               "formatter with empty product should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::ConfigError,
               "empty product should map to config error");
  }

  [[nodiscard]] bool test_invalid_command_help()
  {
    kordex::cli::Command invalid;
    kordex::cli::HelpFormatter formatter;

    auto result = formatter.format_command(invalid);

    return expect_true(
               !result,
               "invalid command help should fail") &&
           expect_true(
               result.error().code() ==
                   vix::error::ErrorCode::InvalidArgument,
               "invalid command help should map to invalid argument");
  }
} // namespace

int main()
{
  const bool ok =
      test_options_helpers() &&
      test_default_formatter() &&
      test_formatter_from_config() &&
      test_format_usage() &&
      test_format_command_usage() &&
      test_format_aliases() &&
      test_format_commands_visible_only() &&
      test_format_empty_commands() &&
      test_format_global_options() &&
      test_format_examples() &&
      test_format_top_level_help() &&
      test_format_top_level_help_with_hidden() &&
      test_format_command_help() &&
      test_format_command_without_aliases() &&
      test_invalid_formatter_empty_executable() &&
      test_invalid_formatter_empty_product() &&
      test_invalid_command_help();

  return ok ? 0 : 1;
}

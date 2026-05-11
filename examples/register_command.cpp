/**
 *
 *  @file register_command.cpp
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

#include <iostream>
#include <kordex/cli/Cli.hpp>

int main()
{
  auto cli_result = kordex::cli::Cli::create(
      kordex::cli::CliOptions::test());

  if (!cli_result)
  {
    std::cerr << "failed to create CLI: "
              << cli_result.error().message()
              << '\n';
    return 1;
  }

  auto cli = std::move(cli_result.value());

  kordex::cli::CommandInfo info;
  info.name = "hello";
  info.aliases = {"hi"};
  info.summary = "Print a hello message";
  info.description = "Example command registered by user code.";
  info.usage = "kordex hello [name]";
  info.hidden = false;
  info.enabled = true;

  auto command_result = kordex::cli::Command::create(
      info,
      [](const kordex::cli::CommandContext &context) -> kordex::cli::CliResult
      {
        const std::string name = context.has_args()
                                     ? context.args.front()
                                     : "Kordex";

        return kordex::cli::CliResult::success(
            "Hello from " + name);
      });

  if (!command_result)
  {
    std::cerr << "failed to create command: "
              << command_result.error().message()
              << '\n';
    return 1;
  }

  const auto register_error = cli.register_command(
      std::move(command_result.value()));

  if (register_error)
  {
    std::cerr << "failed to register command: "
              << register_error.message()
              << '\n';
    return 1;
  }

  const auto result = cli.run_command(
      "hello",
      {"custom command"});

  if (!result.succeeded())
  {
    std::cerr << "command failed: "
              << result.error.message()
              << '\n';
    return result.exit_code;
  }

  std::cout << result.output << '\n';

  const auto alias_result = cli.run_command(
      "hi",
      {"alias"});

  if (!alias_result.succeeded())
  {
    std::cerr << "alias command failed: "
              << alias_result.error.message()
              << '\n';
    return alias_result.exit_code;
  }

  std::cout << alias_result.output << '\n';

  return 0;
}

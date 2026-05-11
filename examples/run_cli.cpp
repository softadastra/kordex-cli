/**
 *
 *  @file run_cli.cpp
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
#include <sstream>

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

  {
    const auto result = cli.run(
        {"kordex-test", "version"});

    if (!result.version_requested())
    {
      std::cerr << "version command failed\n";
      return 1;
    }

    std::cout << "version output:\n";
    std::cout << result.output << "\n\n";
  }

  {
    const auto result = cli.run(
        {"kordex-test", "--help"});

    if (!result.help_requested())
    {
      std::cerr << "help command failed\n";
      return 1;
    }

    std::cout << "help output:\n";
    std::cout << result.output << "\n";
  }

  {
    std::ostringstream out;
    std::ostringstream err;

    const auto result = cli.run(
        {"kordex-test", "unknown"},
        out,
        err);

    std::cout << "\nunknown command exit code = "
              << result.exit_code
              << '\n';

    if (!out.str().empty())
    {
      std::cout << "stdout:\n"
                << out.str() << '\n';
    }

    if (!err.str().empty())
    {
      std::cout << "stderr:\n"
                << err.str() << '\n';
    }
  }

  return 0;
}

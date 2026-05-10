/**
 *
 *  @file parse_args.cpp
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

#include <iostream>
#include <kordex/cli/ArgumentParser.hpp>

int main()
{
  kordex::cli::ArgumentParser parser;

  auto parsed_result = parser.parse(
      {"kordex", "--verbose", "--json", "run", "src/main.js", "--", "--user", "gaspard"});

  if (!parsed_result)
  {
    std::cerr << "parse failed: "
              << parsed_result.error().message()
              << '\n';
    return 1;
  }

  const auto &parsed = parsed_result.value();

  std::cout << "executable = " << parsed.executable_name << '\n';
  std::cout << "command    = " << parsed.command << '\n';
  std::cout << "output     = " << kordex::cli::to_string(parsed.output_mode) << '\n';
  std::cout << "verbose    = " << (parsed.verbose ? "yes" : "no") << '\n';
  std::cout << "debug      = " << (parsed.debug ? "yes" : "no") << '\n';
  std::cout << "json       = " << (parsed.json ? "yes" : "no") << '\n';
  std::cout << "quiet      = " << (parsed.quiet ? "yes" : "no") << '\n';
  std::cout << "no color   = " << (parsed.no_color ? "yes" : "no") << '\n';
  std::cout << "dry run    = " << (parsed.dry_run ? "yes" : "no") << '\n';

  std::cout << "positional = " << parsed.positional.size() << '\n';

  for (const auto &arg : parsed.positional)
  {
    std::cout << "- " << arg << '\n';
  }

  auto options_result = parser.to_options(parsed);
  if (!options_result)
  {
    std::cerr << "options failed: "
              << options_result.error().message()
              << '\n';
    return 1;
  }

  const auto &options = options_result.value();

  std::cout << "normalized args = " << options.args.size() << '\n';

  for (const auto &arg : options.args)
  {
    std::cout << "> " << arg << '\n';
  }

  return 0;
}

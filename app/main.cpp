/**
 *
 *  @file main.cpp
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

#include <kordex/cli/Cli.hpp>

int main(int argc, char **argv)
{
  return kordex::cli::run_cli(argc, argv);
}

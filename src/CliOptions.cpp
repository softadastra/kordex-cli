/**
 *
 *  @file CliOptions.cpp
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

#include <kordex/cli/CliOptions.hpp>

namespace kordex::cli
{
  CliOptions CliOptions::defaults()
  {
    return CliOptions{};
  }

  CliOptions CliOptions::development()
  {
    CliOptions options;
    options.mode = CliMode::Development;
    options.output_mode = OutputMode::Text;
    options.executable_name = "kordex";

    options.color = true;
    options.verbose = true;
    options.debug = true;
    options.dry_run = false;
    options.interactive = true;
    options.enable_aliases = true;
    options.allow_unknown_args = false;

    options.enable_help = true;
    options.enable_init = true;
    options.enable_run = true;
    options.enable_check = true;
    options.enable_build = true;
    options.enable_repl = true;
    options.enable_version = true;

    return options;
  }

  CliOptions CliOptions::test()
  {
    CliOptions options;
    options.mode = CliMode::Test;
    options.output_mode = OutputMode::Text;
    options.executable_name = "kordex-test";

    options.color = false;
    options.verbose = false;
    options.debug = true;
    options.dry_run = false;
    options.interactive = false;
    options.enable_aliases = true;
    options.allow_unknown_args = false;

    options.enable_help = true;
    options.enable_init = true;
    options.enable_run = true;
    options.enable_check = true;
    options.enable_build = true;
    options.enable_repl = true;
    options.enable_version = true;

    return options;
  }

  CliOptions CliOptions::quiet()
  {
    CliOptions options;
    options.mode = CliMode::Normal;
    options.output_mode = OutputMode::Quiet;
    options.executable_name = "kordex";

    options.color = false;
    options.verbose = false;
    options.debug = false;
    options.interactive = false;

    return options;
  }

  CliOptions CliOptions::json()
  {
    CliOptions options;
    options.mode = CliMode::Normal;
    options.output_mode = OutputMode::Json;
    options.executable_name = "kordex";

    options.color = false;
    options.verbose = false;
    options.debug = false;
    options.interactive = false;

    return options;
  }

  bool CliOptions::has_executable_name() const noexcept
  {
    return !executable_name.empty();
  }

  bool CliOptions::has_working_directory() const noexcept
  {
    return !working_directory.empty();
  }

  bool CliOptions::has_config_path() const noexcept
  {
    return !config_path.empty();
  }

  bool CliOptions::has_args() const noexcept
  {
    return !args.empty();
  }

  bool CliOptions::text_output() const noexcept
  {
    return output_mode == OutputMode::Text;
  }

  bool CliOptions::json_output() const noexcept
  {
    return output_mode == OutputMode::Json;
  }

  bool CliOptions::quiet_output() const noexcept
  {
    return output_mode == OutputMode::Quiet;
  }

  bool CliOptions::has_enabled_commands() const noexcept
  {
    return enable_help ||
           enable_init ||
           enable_run ||
           enable_check ||
           enable_build ||
           enable_repl ||
           enable_version;
  }

  const char *to_string(
      CliMode mode) noexcept
  {
    switch (mode)
    {
    case CliMode::Normal:
      return "normal";
    case CliMode::Development:
      return "development";
    case CliMode::Test:
      return "test";
    }

    return "normal";
  }

  const char *to_string(
      OutputMode mode) noexcept
  {
    switch (mode)
    {
    case OutputMode::Text:
      return "text";
    case OutputMode::Json:
      return "json";
    case OutputMode::Quiet:
      return "quiet";
    }

    return "text";
  }

} // namespace kordex::cli

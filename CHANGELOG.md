# Changelog

All notable changes to `kordex::cli` will be documented in this file.

The format follows a simple release-oriented structure.

## [0.1.0] - 2026-01-01

### Added

- Added initial `kordex::cli` module.
- Added official Kordex CLI executable:

```txt
kordex
```

- Added executable entry point:

```
app/main.cpp
```

- Added public umbrella header:

```cpp
#include <kordex/cli/Cli.hpp>
```

- Added version API:
  - `version()`
  - `version_major()`
  - `version_minor()`
  - `version_patch()`
- Added shared CLI error model:
  - `CliErrorCode`
  - `make_cli_error()`
  - `ok()`
  - `to_error_code()`
  - `default_message()`
- Added result aliases:
  - `Result<T>`
  - `BoolResult`
  - `StringResult`
  - `IntResult`
- Added CLI options:
  - `CliOptions::defaults()`
  - `CliOptions::development()`
  - `CliOptions::test()`
  - `CliOptions::quiet()`
  - `CliOptions::json()`
- Added normalized CLI config:
  - `CliConfig`
  - command enable flags
  - output mode handling
  - executable name support
  - working directory support
  - raw argument support
  - validation for quiet/debug/verbose/json/dry-run conflicts
- Added CLI result model:
  - `CliResult`
  - `CliExitStatus`
  - success result
  - failure result
  - help result
  - version result
  - cancelled result
- Added command abstraction:
  - `Command`
  - `CommandInfo`
  - `CommandContext`
  - `CommandHandler`
  - command aliases
  - command visibility
  - command enabled/disabled state
  - command validation
  - exception-safe command execution
- Added command registry:
  - register commands
  - find commands by name
  - find commands by alias
  - detect duplicate command names
  - detect alias conflicts
  - resolve command from argv-style arguments
  - run command by name
  - run command from raw arguments
  - visible command listing
- Added argument parser:
  - executable name parsing
  - command parsing
  - positional argument parsing
  - global flag parsing
  - option terminator support with `--`
  - unknown option policy
  - argc/argv conversion
  - conversion to `CliOptions`
- Added help formatter:
  - top-level help output
  - command-specific help output
  - usage section
  - command list section
  - global options section
  - examples section
  - alias formatting
  - hidden command support
- Added output renderer:
  - text output
  - JSON output
  - quiet output
  - stdout/stderr result routing
  - colored labels
  - JSON escaping
  - error rendering
  - result rendering
- Added built-in command registry helpers:
  - `create_help_command()`
  - `create_init_command()`
  - `create_run_command()`
  - `create_check_command()`
  - `create_build_command()`
  - `create_repl_command()`
  - `create_version_command()`
  - `register_default_commands()`
  - `create_default_command_registry()`
- Added `help` command:
  - top-level help output
  - built-in command listing
  - global options output
- Added `init` command:
  - create a new Kordex project
  - generate `README.md`
  - generate `package.json`
  - generate `kordex.json`
  - generate `src/main.js`
  - support `--force`
  - support `--name`
  - support `--dir`
  - support `--no-package-json`
  - support `--no-kordex-json`
  - support `--no-main`
  - support dry-run mode
- Added `run` command:
  - load and run a JavaScript or TypeScript source file through `kordex::runtime`
  - support runtime args after `--`
  - support `--debug`
  - support `--allow-net`
  - support `--allow-process`
  - support `--no-diagnostics`
  - support `--no-env`
  - support dry-run mode
- Added `check` command:
  - validate source file existence
  - validate source file type
  - load source through `kordex::runtime::SourceFile`
  - detect executable status
  - report file size
  - report line count
  - report diagnostics
  - support `--details`
  - support `--json`
  - support `--no-warnings`
  - warn on empty files
  - warn on whitespace-only files
  - warn on trailing whitespace
- Added `build` command:
  - build source file into output file
  - build project directory through manifest entry
  - support `--project`
  - support `--file`
  - support `--out-dir`
  - support `--out`
  - support `--force`
  - support `--details`
  - support `--minify`
  - support `--source-map`
  - emit a Kordex build banner
  - write output to `dist` by default
- Added `repl` command:
  - REPL placeholder
  - eval mode with `--eval`
  - eval mode with `-e`
  - eval mode with `--`
  - runtime option support
  - dry-run support
- Added `version` command:
  - simple version output
  - detailed version output with `--details`
  - JSON output with `--json`
  - reports versions for:
    - CLI
    - runtime
    - bindings
    - std
- Added main CLI facade:
  - `Cli`
  - `Cli::create()`
  - `Cli::from_args()`
  - `Cli::run()`
  - `Cli::run_command()`
  - `Cli::help()`
  - `Cli::version_text()`
  - `run_cli()`
- Added examples:
  - `parse_args.cpp`
  - `register_command.cpp`
  - `run_cli.cpp`
- Added tests for:
  - version API
  - error model
  - CLI options
  - CLI config
  - CLI result
  - command abstraction
  - command registry
  - argument parser
  - help formatter
  - output renderer
  - CLI facade
- Added CMake package support:
  - `kordex::cli`
  - standalone build
  - umbrella build support
  - app build support
  - tests build support
  - examples build support
  - install/export support
  - package config support

### Notes

`kordex::cli` is the terminal interface only.

Runtime execution is delegated to `kordex::runtime`.

Standard modules are owned by `kordex::standard`.

Bindings are owned by `kordex::bindings`.

The dependency direction is:

```txt
cli
  -> std
  -> bindings
  -> runtime
  -> vix modules
```

`runtime`, `bindings`, and `std` must not depend on `cli`.

JavaScript execution is not fully connected yet. The CLI validates the command flow and delegates to the current runtime layer.

Real bundling, minification, source maps, and interactive REPL behavior are planned for later versions.

## Roadmap

### Planned

- Connect `run` to the real JavaScript engine through `kordex::bindings`.
- Connect `repl` to an interactive JavaScript engine session.
- Add real TypeScript checking.
- Add real bundling for `build`.
- Add minification backend.
- Add source map generation.
- Add project discovery from `kordex.json`.
- Add command-specific help routing through `kordex help <command>`.
- Add shell completion generation.
- Add plugin command registration.
- Add `install`/`update` commands later.
- Add stable JSON schemas for machine-readable output.
- Add richer diagnostics and structured CLI error output.
- Add integration tests for the final `kordex` executable.

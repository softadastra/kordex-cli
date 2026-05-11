# Kordex CLI

Official command line interface for Kordex.

`kordex::cli` provides the terminal entry point for Kordex. It parses command-line arguments, registers commands, runs source files through the runtime, renders help/version output, and exposes the executable used by developers.

## Purpose

Kordex CLI is the user-facing command layer.

It connects:

```txt
kordex::cli
  -> kordex::std
  -> kordex::bindings
  -> kordex::runtime
  -> vix modules
```

The dependency direction must stay one-way.

`runtime`, `bindings`, and `std` must not depend on `cli`.

## Public include

Use the umbrella header:

```cpp
#include <kordex/cli/Cli.hpp>
```

For lower-level APIs:

```cpp
#include <kordex/cli/ArgumentParser.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/CommandRegistry.hpp>
#include <kordex/cli/Output.hpp>
```

## Executable

The module builds the executable:

```
kordex
```

Entry point:

```
app/main.cpp
```

It calls:

```cpp
return kordex::cli::run_cli(argc, argv);
```

## Commands

Kordex CLI currently provides:

| Command | Description |
|---|---|
| `help` | Show help |
| `init` | Create a new Kordex project |
| `run` | Run a JavaScript or TypeScript file |
| `check` | Check a source file |
| `build` | Build a source file or project |
| `repl` | Start a REPL or evaluate source |
| `version` | Show Kordex version |

Aliases:

| Alias | Command |
|---|---|
| `new` | alias for `init` |
| `-h` | alias for `help` |
| `--help` | alias for `help` |
| `-V` | alias for `version` |
| `--version` | alias for `version` |

## Quick usage

```bash
kordex --help
kordex version
kordex init app
kordex run src/main.js
kordex check src/main.js
kordex build src/main.js
kordex repl
```

## Init command

Create a new Kordex project:

```bash
kordex init app
```

Generated files:

```
app/
├── README.md
├── package.json
├── kordex.json
└── src/
    └── main.js
```

Options:

```bash
kordex init app --force
kordex init app --no-package-json
kordex init app --no-kordex-json
kordex init app --no-main
kordex init --name app --dir ./apps/app
```

Dry run:

```bash
kordex --dry-run init app
```

## Run command

Run a JavaScript or TypeScript file:

```bash
kordex run src/main.js
```

Pass arguments to the executed source:

```bash
kordex run src/main.js -- user 42
```

Runtime options:

```bash
kordex run src/main.js --debug
kordex run src/main.js --allow-net
kordex run src/main.js --allow-process
kordex run src/main.js --no-diagnostics
kordex run src/main.js --no-env
```

The current implementation validates and loads the source through `kordex::runtime`. JavaScript execution is connected later through `kordex::bindings`.

## Check command

Check a source file:

```bash
kordex check src/main.js
```

With details:

```bash
kordex check src/main.js --details
```

JSON output:

```bash
kordex check src/main.js --json
```

Disable warnings:

```bash
kordex check src/main.js --no-warnings
```

The check command validates:

- file existence
- file type
- source loading
- source executable status
- basic warnings

Warnings include:

- empty file
- whitespace-only file
- trailing whitespace

## Build command

Build a source file:

```bash
kordex build src/main.js
```

Build into a custom output directory:

```bash
kordex build src/main.js --out-dir dist
```

Build with a custom output name:

```bash
kordex build src/main.js --out app.js
```

Build a project directory:

```bash
kordex build . --project
```

Other options:

```bash
kordex build src/main.js --force
kordex build src/main.js --details
kordex build src/main.js --minify
kordex build src/main.js --source-map
```

The first build implementation writes a bundled output with a Kordex build banner. Real bundling, minification, and source map generation can be connected later behind the same command shape.

## Repl command

Start the REPL placeholder:

```bash
kordex repl
```

Evaluate source directly:

```bash
kordex repl --eval "console.log('hello')"
```

Or:

```bash
kordex repl -- console.log("hello")
```

The interactive REPL is currently a placeholder until the JavaScript engine is connected through `kordex::bindings`.

## Version command

Show version:

```bash
kordex version
```

Show detailed version information:

```bash
kordex version --details
```

JSON output:

```bash
kordex version --json
```

The version command prints versions for:

- `cli`
- `runtime`
- `bindings`
- `std`

## Global options

| Option | Description |
|---|---|
| `-h`, `--help` | Show help |
| `-V`, `--version` | Show version |
| `-v`, `--verbose` | Enable verbose output |
| `--debug` | Enable debug output |
| `-q`, `--quiet` | Disable normal output |
| `--json` | Render machine-readable JSON output |
| `--no-color` | Disable colored output |
| `--dry-run` | Show what would happen without executing |

## C++ API example

```cpp
#include <iostream>

#include <kordex/cli/Cli.hpp>

int main()
{
  auto cli_result = kordex::cli::Cli::create(
      kordex::cli::CliOptions::test());

  if (!cli_result)
  {
    std::cerr << cli_result.error().message() << '\n';
    return 1;
  }

  auto cli = std::move(cli_result.value());

  const auto result = cli.run(
      {"kordex-test", "version"});

  if (!result.version_requested())
  {
    std::cerr << result.error.message() << '\n';
    return result.exit_code;
  }

  std::cout << result.output << '\n';

  return 0;
}
```

## Register a custom command

```cpp
#include <iostream>

#include <kordex/cli/Cli.hpp>

int main()
{
  auto cli_result = kordex::cli::Cli::create(
      kordex::cli::CliOptions::test());

  if (!cli_result)
  {
    return 1;
  }

  auto cli = std::move(cli_result.value());

  kordex::cli::CommandInfo info;
  info.name = "hello";
  info.aliases = {"hi"};
  info.summary = "Print hello";
  info.usage = "kordex hello [name]";

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
    return 1;
  }

  const auto register_error = cli.register_command(
      std::move(command_result.value()));

  if (register_error)
  {
    return 1;
  }

  const auto result = cli.run_command("hello", {"custom command"});

  std::cout << result.output << '\n';

  return result.exit_code;
}
```

## Main concepts

### CliOptions

`CliOptions` is the user-facing configuration object.

```cpp
auto options = kordex::cli::CliOptions::defaults();
```

Available presets:

```cpp
kordex::cli::CliOptions::defaults();
kordex::cli::CliOptions::development();
kordex::cli::CliOptions::test();
kordex::cli::CliOptions::quiet();
kordex::cli::CliOptions::json();
```

It controls:

- CLI mode
- output mode
- executable name
- working directory
- raw args
- color
- verbose/debug
- dry-run
- interactive mode
- enabled commands

### CliConfig

`CliConfig` is the normalized internal configuration.

```cpp
auto config = kordex::cli::CliConfig::from_options(options);
```

It validates:

- executable name
- enabled commands
- argument entries
- quiet/verbose conflict
- quiet/debug conflict
- JSON/color conflict
- dry-run/interactive conflict

### ArgumentParser

`ArgumentParser` parses argv-style input.

```cpp
kordex::cli::ArgumentParser parser;

auto parsed = parser.parse(
    {"kordex", "--json", "run", "src/main.js"});
```

It handles:

- global flags
- command name
- positional args
- unknown option policy
- option terminator `--`

### Command

`Command` represents one executable CLI command.

```cpp
auto command = kordex::cli::Command::create(info, handler);
```

A command contains:

- name
- aliases
- summary
- description
- usage
- hidden flag
- enabled flag
- handler

### CommandRegistry

`CommandRegistry` owns and resolves commands.

```cpp
kordex::cli::CommandRegistry registry;

registry.register_command(std::move(command));
auto result = registry.run("hello", config);
```

It supports:

- register command
- find by name
- find by alias
- resolve command from args
- run command
- visible command list
- duplicate detection

### HelpFormatter

`HelpFormatter` renders CLI help.

```cpp
kordex::cli::HelpFormatter formatter;

auto help = formatter.format(registry);
```

It can render:

- top-level help
- single command help
- usage
- command list
- global options
- examples

### Output

`Output` centralizes stdout/stderr rendering.

```cpp
kordex::cli::Output output;

output.write_result(std::cout, std::cerr, result);
```

It supports:

- text output
- JSON output
- quiet output
- colored labels
- error rendering
- result rendering

### Cli

`Cli` is the main facade.

```cpp
auto cli = kordex::cli::Cli::create();
```

It owns:

- `CliConfig`
- `CommandRegistry`
- `ArgumentParser`
- `Output`

## Build

From the module directory:

```bash
vix build --build-target all -v
```

Or with CMake:

```bash
cmake -S . -B build-ninja -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DKORDEX_CLI_BUILD_APP=ON \
  -DKORDEX_CLI_BUILD_TESTS=ON \
  -DKORDEX_CLI_BUILD_EXAMPLES=ON

cmake --build build-ninja
```

## Build options

```
KORDEX_CLI_BUILD_APP=ON
KORDEX_CLI_BUILD_TESTS=OFF
KORDEX_CLI_BUILD_EXAMPLES=OFF

KORDEX_CLI_ENABLE_WARNINGS=ON
KORDEX_CLI_ENABLE_SANITIZERS=OFF

KORDEX_CLI_ENABLE_INIT_COMMAND=ON
KORDEX_CLI_ENABLE_RUN_COMMAND=ON
KORDEX_CLI_ENABLE_CHECK_COMMAND=ON
KORDEX_CLI_ENABLE_BUILD_COMMAND=ON
KORDEX_CLI_ENABLE_REPL_COMMAND=ON
KORDEX_CLI_ENABLE_VERSION_COMMAND=ON

KORDEX_CLI_FETCH_RUNTIME=ON
KORDEX_CLI_FETCH_BINDINGS=ON
KORDEX_CLI_FETCH_STD=ON

KORDEX_CLI_FETCH_ERROR=ON
KORDEX_CLI_FETCH_LOG=ON
KORDEX_CLI_FETCH_JSON=ON
KORDEX_CLI_FETCH_FS=ON
KORDEX_CLI_FETCH_PATH=ON
KORDEX_CLI_FETCH_ENV=ON
KORDEX_CLI_FETCH_PROCESS=ON
KORDEX_CLI_FETCH_TIME=ON
KORDEX_CLI_FETCH_TESTS=ON

KORDEX_VIX_GIT_TAG=main
KORDEX_RUNTIME_GIT_TAG=main
KORDEX_BINDINGS_GIT_TAG=main
KORDEX_STD_GIT_TAG=main
```

## Tests

Enable tests:

```bash
cmake -S . -B build-ninja -G Ninja \
  -DKORDEX_CLI_BUILD_TESTS=ON

cmake --build build-ninja

ctest --test-dir build-ninja --output-on-failure
```

Test files:

```
tests/
├── test_version.cpp
├── test_error.cpp
├── test_cli_options.cpp
├── test_cli_config.cpp
├── test_cli_result.cpp
├── test_command.cpp
├── test_command_registry.cpp
├── test_argument_parser.cpp
├── test_help_formatter.cpp
├── test_output.cpp
└── test_cli.cpp
```

## Examples

Enable examples:

```bash
cmake -S . -B build-ninja -G Ninja \
  -DKORDEX_CLI_BUILD_EXAMPLES=ON

cmake --build build-ninja
```

Examples:

```
examples/
├── parse_args.cpp
├── register_command.cpp
└── run_cli.cpp
```

## Project structure

```
modules/cli/
├── README.md
├── CHANGELOG.md
├── LICENSE
├── CMakeLists.txt
├── vix.json
├── .gitignore
├── cmake/
│   ├── KordexCliConfig.cmake.in
│   └── KordexCliOptions.cmake
├── include/kordex/cli/
│   ├── Version.hpp
│   ├── Error.hpp
│   ├── Result.hpp
│   ├── CliOptions.hpp
│   ├── CliConfig.hpp
│   ├── CliResult.hpp
│   ├── Command.hpp
│   ├── CommandRegistry.hpp
│   ├── ArgumentParser.hpp
│   ├── HelpFormatter.hpp
│   ├── Output.hpp
│   ├── Commands.hpp
│   ├── InitCommand.hpp
│   ├── RunCommand.hpp
│   ├── CheckCommand.hpp
│   ├── BuildCommand.hpp
│   ├── ReplCommand.hpp
│   ├── VersionCommand.hpp
│   └── Cli.hpp
├── src/
│   ├── Version.cpp
│   ├── Error.cpp
│   ├── CliOptions.cpp
│   ├── CliConfig.cpp
│   ├── CliResult.cpp
│   ├── Command.cpp
│   ├── CommandRegistry.cpp
│   ├── ArgumentParser.cpp
│   ├── HelpFormatter.cpp
│   ├── Output.cpp
│   ├── Commands.cpp
│   ├── InitCommand.cpp
│   ├── RunCommand.cpp
│   ├── CheckCommand.cpp
│   ├── BuildCommand.cpp
│   ├── ReplCommand.cpp
│   ├── VersionCommand.cpp
│   └── Cli.cpp
├── app/
│   └── main.cpp
├── tests/
└── examples/
```

## Design rules

Kordex CLI is only the terminal interface.

It should not own runtime execution internals, bindings internals, or standard module implementations.

- `cli` may depend on `std`
- `cli` may depend on `bindings`
- `cli` may depend on `runtime`
- `std` must not depend on `cli`
- `bindings` must not depend on `cli`
- `runtime` must not depend on `cli`

This keeps the architecture modular and prevents circular dependencies.

## Roadmap

Planned next steps:

- connect `run` and `repl` to the real JavaScript engine
- add real TypeScript checking
- add real bundling for `build`
- add project discovery from `kordex.json`
- add command-specific help
- add `install`/`update` commands later
- add plugin command registration
- improve JSON output with stable schemas
- add shell completion generation
- add better diagnostics and error codes

## License

MIT License.


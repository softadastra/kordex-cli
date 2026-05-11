# Kordex CLI

Command-line interface for Kordex.

Kordex is a JavaScript runtime for reliable local-first applications. This module provides the `kordex` command used to initialize projects, run scripts, evaluate code, check files, bundle projects, manage packages, and expose project plugin commands.

## Features

- Run JavaScript and TypeScript files
- Evaluate JavaScript with `repl --eval`
- Resolve project entry from `kordex.json` or `package.json`
- Support relative imports and JSON modules
- Support standard modules such as `kordex:path`, `kordex:fs`, `kordex:env`, and `kordex:process`
- Bundle projects into `dist/main.js`
- Generate basic source maps
- Enforce runtime permissions
- Load plugin commands from project configuration
- Generate and update `kordex.lock`

## Commands

```txt
kordex init <name>
kordex run [file]
kordex repl --eval <source>
kordex check <file>
kordex build <file|project>
kordex install [package[@version]]
kordex update [package]
kordex version
kordex help
```

## Quick start

Create a project:

```bash
kordex init app
cd app
```

Run a file:

```bash
kordex run src/main.js
```

Run the project entry from `kordex.json`:

```bash
kordex run
```

Evaluate code directly:

```bash
kordex repl --eval "1 + 2"
```

Build a project:

```bash
kordex build . --project --out-dir dist --force
```

## Project configuration

Kordex looks for `kordex.json` first, then `package.json`.

Example `kordex.json`:

```json
{
  "name": "my-app",
  "version": "0.1.0",
  "entry": "src/main.ts",
  "registry": "https://registry.vixcpp.com",
  "dependencies": {
    "kordex/std": "0.1.0"
  },
  "scripts": {
    "dev": "kordex run src/main.ts",
    "build": "kordex build . --project"
  }
}
```

## Running files

Run JavaScript:

```bash
kordex run main.js
```

Run TypeScript:

```bash
kordex run main.ts
```

Run the project entry:

```bash
kordex run
```

The CLI resolves the entry from:

- `kordex.json` `entry`
- `package.json` `kordex`
- `package.json` `module`
- `package.json` `main`
- fallback files such as `src/main.ts`, `src/main.js`, `index.ts`, or `index.js`

## Imports

Relative imports are supported:

```js
import { message } from "./lib/message.js";

message;
```

Extension resolution is supported:

```js
import { message } from "./lib/message";
```

Directory index resolution is supported:

```js
import { name } from "./pkg";
```

JSON imports are supported:

```js
import user from "./data/user.json";

user.name;
```

## Standard modules

Kordex standard modules can be imported with the `kordex:` prefix.

Example:

```js
import { join } from "kordex:path";

join("/tmp", "kordex", "app");
```

Some modules require explicit permissions.

## Permissions

Sensitive modules are disabled by default.

Available permission flags:

- `--allow-fs`
- `--allow-env`
- `--allow-net`
- `--allow-process`

Example:

```bash
kordex run main.js --allow-fs
```

Without `--allow-fs`, this script fails:

```js
import { exists } from "kordex:fs";

exists("/tmp");
```

With permission:

```bash
kordex run main.js --allow-fs
```

## Build

Bundle one file:

```bash
kordex build main.js --out-dir dist --force
```

Bundle a project:

```bash
kordex build . --project --out-dir dist --force
```

Choose output file:

```bash
kordex build . --project --out-dir dist --out-file app.js --force
```

Generate source map:

```bash
kordex build . --project --source-map --force
```

This generates:

```
dist/main.js
dist/main.js.map
```

## Package install

Install dependencies declared in `kordex.json`:

```bash
kordex install
```

Install one package:

```bash
kordex install softadastra/plugin-example@0.1.0
```

Use a custom registry:

```bash
kordex install --registry https://registry.vixcpp.com
```

The command writes:

```
kordex.lock
```

## Package update

Update all dependencies:

```bash
kordex update
```

Update one dependency:

```bash
kordex update softadastra/plugin-example
```

## Plugin commands

Kordex can load project plugin commands from `kordex.json`.

Example:

```json
{
  "plugins": {
    "commands": [
      {
        "name": "hello",
        "summary": "Run hello plugin",
        "run": "scripts/hello.ts",
        "aliases": ["hi"],
        "permissions": {
          "fs": false,
          "env": false,
          "net": false,
          "process": false
        }
      }
    ]
  }
}
```

Then:

```bash
kordex hello
```

or:

```bash
kordex hi
```

Plugin commands are loaded with isolated permissions. They cannot override built-in commands.

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

## Build from source

From the module directory:

```bash
vix build --preset dev-ninja
```

With tests:

```bash
vix build \
  --preset dev-ninja \
  -- \
  -DKORDEX_CLI_BUILD_TESTS=ON

vix tests -- --output-on-failure
```

## Integration tests

The CLI integration test covers:

- `kordex run`
- `kordex repl --eval`
- relative imports
- JSON imports
- standard modules
- build output
- permissions

Run:

```bash
vix tests -R kordex_cli_integration_tests
```

## Module role

`kordex-cli` is the user-facing command layer.

It connects:

- `kordex-runtime` for runtime options, project execution model, and permissions
- `kordex-bindings` for JavaScript engine execution
- `kordex-std` for native standard modules
- project discovery for `kordex.json` and `package.json`
- package lock generation through `kordex.lock`

## License

MIT License.

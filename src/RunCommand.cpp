
/**
 *
 *  @file RunCommand.cpp
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

#include <filesystem>
#include <sstream>
#include <utility>

#include <kordex/cli/RunCommand.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool is_flag(
        const ::std::string &value) noexcept
    {
      return value.size() > 1 && value.front() == '-';
    }

    [[nodiscard]] bool is_separator(
        const ::std::string &value) noexcept
    {
      return value == "--";
    }

    [[nodiscard]] Error ensure_file_exists(
        const ::std::string &file)
    {
      try
      {
        if (!::std::filesystem::exists(file))
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "source file does not exist: " + file);
        }

        if (!::std::filesystem::is_regular_file(file))
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "source path is not a file: " + file);
        }
      }
      catch (const ::std::filesystem::filesystem_error &exception)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            exception.what());
      }

      return ok();
    }

    [[nodiscard]] ::std::string format_runtime_success(
        const ::std::string &file,
        const kordex::runtime::RuntimeResult &result)
    {
      if (!result.output.empty())
      {
        return result.output;
      }

      ::std::ostringstream stream;
      stream << "Ran " << file;

      return stream.str();
    }

    [[nodiscard]] ::std::string format_runtime_failure(
        const kordex::runtime::RuntimeResult &result)
    {
      if (!result.error_output.empty())
      {
        return result.error_output;
      }

      if (result.error.has_error())
      {
        return ::std::string(result.error.message());
      }

      return "runtime execution failed";
    }

    [[nodiscard]] Error map_runtime_error(
        const kordex::runtime::RuntimeResult &result)
    {
      if (result.error.has_error())
      {
        return make_cli_error(
            CliErrorCode::RuntimeError,
            ::std::string(result.error.message()));
      }

      return make_cli_error(
          CliErrorCode::RuntimeError,
          "runtime execution failed");
    }
  } // namespace

  bool RunCommandOptions::has_file() const noexcept
  {
    return !file.empty();
  }

  bool RunCommandOptions::has_args() const noexcept
  {
    return !args.empty();
  }

  Result<RunCommandOptions> parse_run_options(
      const CommandContext &context)
  {
    RunCommandOptions options;
    options.debug = context.config.debug;
    options.diagnostics = true;

    bool passthrough = false;

    for (::std::size_t index = 0; index < context.args.size(); ++index)
    {
      const auto &arg = context.args[index];

      if (arg.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "run arguments cannot contain empty entries");
      }

      if (passthrough)
      {
        options.args.push_back(arg);
        continue;
      }

      if (is_separator(arg))
      {
        passthrough = true;
        continue;
      }

      if (arg == "--debug")
      {
        options.debug = true;
        continue;
      }

      if (arg == "--no-diagnostics")
      {
        options.diagnostics = false;
        continue;
      }

      if (arg == "--allow-net")
      {
        options.allow_net = true;
        continue;
      }

      if (arg == "--allow-process")
      {
        options.allow_process = true;
        continue;
      }

      if (arg == "--no-fs")
      {
        options.allow_fs = false;
        continue;
      }

      if (arg == "--no-env")
      {
        options.allow_env = false;
        continue;
      }

      if (is_flag(arg) && !options.has_file())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "unknown run option: " + arg);
      }

      if (!options.has_file())
      {
        options.file = arg;
        continue;
      }

      options.args.push_back(arg);
    }

    const auto validation = validate_run_options(options);
    if (validation)
    {
      return validation;
    }

    return options;
  }

  Error validate_run_options(
      const RunCommandOptions &options)
  {
    if (options.file.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "run command requires a source file");
    }

    if (is_flag(options.file))
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "run source file cannot be an option");
    }

    if (!options.allow_fs)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "run command requires filesystem access to load the source file");
    }

    return ok();
  }

  kordex::runtime::RuntimeOptions to_runtime_options(
      const RunCommandOptions &options,
      const CliConfig &config)
  {
    auto runtime_options = config.debug
                               ? kordex::runtime::RuntimeOptions::development()
                               : kordex::runtime::RuntimeOptions::defaults();

    runtime_options.mode = config.debug
                               ? kordex::runtime::RuntimeMode::Development
                               : kordex::runtime::RuntimeMode::Development;

    runtime_options.permission_mode = kordex::runtime::PermissionMode::Relaxed;

    runtime_options.working_directory = config.working_directory;
    runtime_options.args = options.args;

    runtime_options.allow_fs = options.allow_fs;
    runtime_options.allow_net = options.allow_net;
    runtime_options.allow_process = options.allow_process;
    runtime_options.allow_env = options.allow_env;

    runtime_options.diagnostics = options.diagnostics;
    runtime_options.debug = options.debug || config.debug;

    return runtime_options;
  }

  CliResult run_run_command(
      const CommandContext &context)
  {
    auto options = parse_run_options(context);
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    if (context.config.dry_run)
    {
      ::std::ostringstream stream;
      stream << "Would run " << options.value().file;

      if (options.value().has_args())
      {
        stream << " with " << options.value().args.size() << " argument(s)";
      }

      return CliResult::success(stream.str());
    }

    const auto file_error = ensure_file_exists(options.value().file);
    if (file_error)
    {
      return CliResult::failure(file_error, 1);
    }

    auto runtime_options = to_runtime_options(
        options.value(),
        context.config);

    auto runtime_result = kordex::runtime::Runtime::from_options(
        runtime_options);

    if (!runtime_result)
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::RuntimeError,
              ::std::string(runtime_result.error().message())),
          1);
    }

    auto runtime = ::std::move(runtime_result.value());

    auto start_error = runtime.start();
    if (start_error)
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::RuntimeError,
              ::std::string(start_error.message())),
          1);
    }

    auto result = runtime.run_file(options.value().file);

    const auto shutdown_error = runtime.shutdown();

    if (result.succeeded())
    {
      auto output = format_runtime_success(
          options.value().file,
          result);

      if (shutdown_error)
      {
        return CliResult::failure(
            make_cli_error(
                CliErrorCode::RuntimeError,
                ::std::string(shutdown_error.message())),
            1);
      }

      return CliResult::success(output);
    }

    CliResult cli_result = CliResult::failure(
        map_runtime_error(result),
        result.exit_code == 0 ? 1 : result.exit_code);

    cli_result.error_output = format_runtime_failure(result);

    if (shutdown_error && cli_result.error_output.empty())
    {
      cli_result.error_output = ::std::string(shutdown_error.message());
    }

    return cli_result;
  }

  Result<Command> create_run_command()
  {
    CommandInfo info;
    info.name = "run";
    info.aliases = {};
    info.summary = "Run a JavaScript or TypeScript file";
    info.description =
        "Run a JavaScript or TypeScript source file through the Kordex runtime.";
    info.usage = "kordex run <file> [--] [args]";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        ::std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return run_run_command(context);
        });
  }

} // namespace kordex::cli

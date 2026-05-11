/**
 *
 *  @file ReplCommand.cpp
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
#include <string>
#include <utility>

#include <kordex/bindings/BindingOptions.hpp>
#include <kordex/bindings/Bindings.hpp>
#include <kordex/bindings/Engine.hpp>
#include <kordex/bindings/ScriptResult.hpp>

#include <kordex/cli/ReplCommand.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool is_flag(
        const ::std::string &value) noexcept
    {
      return value.size() > 1 && value.front() == '-';
    }

    [[nodiscard]] ::std::string join_eval_args(
        const ::std::vector<::std::string> &args,
        ::std::size_t start)
    {
      ::std::ostringstream stream;

      for (::std::size_t index = start; index < args.size(); ++index)
      {
        if (index > start)
        {
          stream << ' ';
        }

        stream << args[index];
      }

      return stream.str();
    }

    [[nodiscard]] kordex::bindings::BindingOptions to_binding_options(
        const ReplCommandOptions &options,
        const CliConfig &config)
    {
      auto binding_options = options.debug || config.debug
                                 ? kordex::bindings::BindingOptions::development()
                                 : kordex::bindings::BindingOptions::defaults();

      binding_options.backend = kordex::bindings::default_backend();
      binding_options.module_policy = kordex::bindings::ModulePolicy::Full;

      binding_options.allow_native_modules = true;
      binding_options.allow_native_functions = true;
      binding_options.allow_runtime_bridge = true;

      binding_options.diagnostics = options.diagnostics;
      binding_options.debug = options.debug || config.debug;
      binding_options.source_maps = false;

      binding_options.engine_name =
          ::std::string("kordex-repl-") +
          kordex::bindings::to_string(binding_options.backend);

      return binding_options;
    }

    [[nodiscard]] ::std::string format_eval_success(
        const kordex::bindings::ScriptResult &result)
    {
      if (!result.output.empty())
      {
        return result.output;
      }

      if (result.has_value())
      {
        return result.value.display();
      }

      return "Evaluated REPL source";
    }

    [[nodiscard]] ::std::string format_eval_failure(
        const kordex::bindings::ScriptResult &result)
    {
      if (!result.error_output.empty())
      {
        return result.error_output;
      }

      if (result.error.has_error())
      {
        return ::std::string(result.error.message());
      }

      return "REPL evaluation failed";
    }

    [[nodiscard]] Error map_binding_error(
        const kordex::bindings::ScriptResult &result)
    {
      if (result.error.has_error())
      {
        return make_cli_error(
            CliErrorCode::BindingError,
            ::std::string(result.error.message()));
      }

      return make_cli_error(
          CliErrorCode::BindingError,
          "bindings evaluation failed");
    }

    [[nodiscard]] CliResult run_interactive_placeholder(
        const ReplCommandOptions &options,
        const CliConfig &config)
    {
      (void)options;
      (void)config;

      ::std::ostringstream stream;

      stream << "Kordex interactive REPL is not connected yet.\n";
      stream << "Use `kordex repl --eval \"1 + 2\"` "
             << "to evaluate JavaScript through the bindings engine.";

      return CliResult::success(stream.str());
    }
  } // namespace

  bool ReplCommandOptions::has_eval() const noexcept
  {
    return !eval.empty();
  }

  Result<ReplCommandOptions> parse_repl_options(
      const CommandContext &context)
  {
    ReplCommandOptions options;

    options.debug = context.config.debug;
    options.diagnostics = true;
    options.interactive = context.config.interactive;

    for (::std::size_t index = 0; index < context.args.size(); ++index)
    {
      const auto &arg = context.args[index];

      if (arg.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "repl arguments cannot contain empty entries");
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

      if (arg == "--no-interactive")
      {
        options.interactive = false;
        continue;
      }

      if (arg == "--eval" || arg == "-e")
      {
        if (index + 1 >= context.args.size())
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "missing value for --eval");
        }

        options.eval = context.args[++index];
        options.interactive = false;
        continue;
      }

      if (arg == "--")
      {
        options.eval = join_eval_args(context.args, index + 1);
        options.interactive = false;
        break;
      }

      if (is_flag(arg))
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "unknown repl option: " + arg);
      }

      if (!options.has_eval())
      {
        options.eval = arg;
        options.interactive = false;
        continue;
      }

      options.eval += ' ';
      options.eval += arg;
    }

    const auto validation = validate_repl_options(options);
    if (validation)
    {
      return validation;
    }

    return options;
  }

  Error validate_repl_options(
      const ReplCommandOptions &options)
  {
    if (!options.interactive && options.eval.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "repl command requires eval source when interactive mode is disabled");
    }

    return ok();
  }

  kordex::runtime::RuntimeOptions to_runtime_options(
      const ReplCommandOptions &options,
      const CliConfig &config)
  {
    auto runtime_options = config.debug || options.debug
                               ? kordex::runtime::RuntimeOptions::development()
                               : kordex::runtime::RuntimeOptions::defaults();

    runtime_options.mode = kordex::runtime::RuntimeMode::Development;
    runtime_options.permission_mode = kordex::runtime::PermissionMode::Relaxed;

    runtime_options.working_directory = config.working_directory;

    runtime_options.allow_fs = options.allow_fs;
    runtime_options.allow_net = options.allow_net;
    runtime_options.allow_process = options.allow_process;
    runtime_options.allow_env = options.allow_env;

    runtime_options.diagnostics = options.diagnostics;
    runtime_options.debug = options.debug || config.debug;

    return runtime_options;
  }

  CliResult run_repl_eval(
      const ReplCommandOptions &options,
      const CliConfig &config)
  {
    if (options.eval.empty())
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::InvalidArgument,
              "eval source cannot be empty"),
          1);
    }

    auto engine_result = kordex::bindings::Engine::create(
        to_binding_options(options, config));

    if (!engine_result)
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::BindingError,
              ::std::string(engine_result.error().message())),
          1);
    }

    auto engine = ::std::move(engine_result.value());

    auto init_result = engine.initialize();
    if (!init_result.succeeded())
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::BindingError,
              ::std::string(init_result.error.message())),
          init_result.exit_code == 0 ? 1 : init_result.exit_code);
    }

    auto result = engine.eval(
        options.eval,
        "repl.js");

    const auto shutdown_result = engine.shutdown();

    if (result.succeeded())
    {
      if (!shutdown_result.succeeded())
      {
        return CliResult::failure(
            make_cli_error(
                CliErrorCode::BindingError,
                ::std::string(shutdown_result.error.message())),
            shutdown_result.exit_code == 0 ? 1 : shutdown_result.exit_code);
      }

      return CliResult::success(
          format_eval_success(result));
    }

    CliResult cli_result = CliResult::failure(
        map_binding_error(result),
        result.exit_code == 0 ? 1 : result.exit_code);

    cli_result.error_output = format_eval_failure(result);

    if (!shutdown_result.succeeded() && cli_result.error_output.empty())
    {
      cli_result.error_output =
          ::std::string(shutdown_result.error.message());
    }

    return cli_result;
  }

  CliResult run_repl_command(
      const CommandContext &context)
  {
    auto options = parse_repl_options(context);
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    if (context.config.dry_run)
    {
      if (options.value().has_eval())
      {
        return CliResult::success("Would evaluate REPL source");
      }

      return CliResult::success("Would start Kordex REPL");
    }

    if (options.value().has_eval())
    {
      return run_repl_eval(options.value(), context.config);
    }

    return run_interactive_placeholder(options.value(), context.config);
  }

  Result<Command> create_repl_command()
  {
    CommandInfo info;
    info.name = "repl";
    info.aliases = {};
    info.summary = "Start an interactive Kordex session";
    info.description =
        "Start a Kordex REPL session or evaluate a source snippet with --eval.";
    info.usage = "kordex repl [--eval <source>]";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        ::std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return run_repl_command(context);
        });
  }

} // namespace kordex::cli

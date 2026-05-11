/**
 *
 *  @file Cli.cpp
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
#include <utility>

#include <kordex/cli/Cli.hpp>
#include <kordex/cli/VersionCommand.hpp>

namespace kordex::cli
{
  bool CliRunOptions::has_args() const noexcept
  {
    return !args.empty();
  }

  Cli::Cli()
      : Cli(CliOptions::defaults())
  {
  }

  Cli::Cli(
      CliOptions options)
  {
    auto config = CliConfig::from_options(options);
    if (config)
    {
      config_ = config.value();
    }
    else
    {
      config_ = {};
    }

    parser_ = ArgumentParser(options);
    output_ = Output::from_config(config_);

    (void)register_defaults();
  }

  Cli::Cli(
      CliConfig config,
      CommandRegistry registry)
      : config_(::std::move(config)),
        registry_(::std::move(registry)),
        parser_(CliOptions::defaults()),
        output_(Output::from_config(config_))
  {
    CliOptions options;
    options.mode = config_.mode;
    options.output_mode = config_.output_mode;
    options.executable_name = config_.executable_name;
    options.working_directory = config_.working_directory;
    options.config_path = config_.config_path;
    options.args = config_.args;

    options.color = config_.color;
    options.verbose = config_.verbose;
    options.debug = config_.debug;
    options.dry_run = config_.dry_run;
    options.interactive = config_.interactive;
    options.enable_aliases = config_.enable_aliases;
    options.allow_unknown_args = config_.allow_unknown_args;

    options.enable_help = config_.enable_help;
    options.enable_init = config_.enable_init;
    options.enable_run = config_.enable_run;
    options.enable_check = config_.enable_check;
    options.enable_build = config_.enable_build;
    options.enable_repl = config_.enable_repl;
    options.enable_version = config_.enable_version;

    parser_ = ArgumentParser(options);
  }

  Result<Cli> Cli::create(
      const CliOptions &options)
  {
    auto config = CliConfig::from_options(options);
    if (!config)
    {
      return config.error();
    }

    auto registry = create_default_command_registry(config.value());
    if (!registry)
    {
      return registry.error();
    }

    Cli cli(config.value(), ::std::move(registry.value()));

    const auto validation = cli.validate();
    if (validation)
    {
      return validation;
    }

    return cli;
  }

  Result<Cli> Cli::from_args(
      int argc,
      char **argv)
  {
    ArgumentParser parser;

    auto options = parser.parse_options(argc, argv);
    if (!options)
    {
      return options.error();
    }

    return create(options.value());
  }

  const CliConfig &Cli::config() const noexcept
  {
    return config_;
  }

  const CommandRegistry &Cli::registry() const noexcept
  {
    return registry_;
  }

  CommandRegistry &Cli::registry() noexcept
  {
    return registry_;
  }

  const ArgumentParser &Cli::parser() const noexcept
  {
    return parser_;
  }

  const Output &Cli::output() const noexcept
  {
    return output_;
  }

  bool Cli::has_commands() const noexcept
  {
    return !registry_.empty();
  }

  Error Cli::validate() const
  {
    const auto config_validation = config_.validate();
    if (config_validation)
    {
      return config_validation;
    }

    const auto output_validation = output_.validate();
    if (output_validation)
    {
      return output_validation;
    }

    if (registry_.empty())
    {
      return make_cli_error(
          CliErrorCode::CommandNotFound,
          "CLI has no registered commands");
    }

    return ok();
  }

  Error Cli::register_command(
      Command command)
  {
    return registry_.register_command(::std::move(command));
  }

  Error Cli::register_defaults()
  {
    return register_default_commands(registry_, config_);
  }

  CliResult Cli::run() const
  {
    return run(config_.args);
  }

  CliResult Cli::run(
      const ::std::vector<::std::string> &args) const
  {
    const auto validation = validate();
    if (validation)
    {
      return CliResult::failure(validation, 1);
    }

    auto parsed = parser_.parse(args);
    if (!parsed)
    {
      return CliResult::failure(parsed.error(), 1);
    }

    auto options = parser_.to_options(parsed.value());
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    auto run_config = CliConfig::from_options(options.value());
    if (!run_config)
    {
      return CliResult::failure(run_config.error(), 1);
    }

    return registry_.run(
        run_config.value(),
        run_config.value().args);
  }

  CliResult Cli::run(
      int argc,
      char **argv) const
  {
    auto args = ArgumentParser::argv_to_vector(argc, argv);
    if (!args)
    {
      return CliResult::failure(args.error(), 1);
    }

    return run(args.value());
  }

  CliResult Cli::run(
      const ::std::vector<::std::string> &args,
      ::std::ostream &out,
      ::std::ostream &err) const
  {
    return write_and_return(run(args), out, err);
  }

  CliResult Cli::run(
      int argc,
      char **argv,
      ::std::ostream &out,
      ::std::ostream &err) const
  {
    return write_and_return(run(argc, argv), out, err);
  }

  CliResult Cli::run_command(
      const ::std::string &name,
      CommandArguments args) const
  {
    const auto validation = validate();
    if (validation)
    {
      return CliResult::failure(validation, 1);
    }

    auto command = registry_.find(name);
    if (!command)
    {
      return CliResult::failure(
          command.error(),
          1);
    }

    CommandContext context;
    context.command_name = name;
    context.args = ::std::move(args);
    context.config = config_;

    return command.value().run(context);
  }

  Result<::std::string> Cli::help() const
  {
    HelpFormatter formatter = HelpFormatter::from_config(config_);

    return formatter.format(registry_);
  }

  Result<::std::string> Cli::help(
      const ::std::string &command_name) const
  {
    auto command = registry_.find(command_name);
    if (!command)
    {
      return command.error();
    }

    HelpFormatter formatter = HelpFormatter::from_config(config_);

    return formatter.format_command(command.value());
  }

  ::std::string Cli::version_text() const
  {
    VersionCommandOptions options;
    options.details = false;
    options.json = config_.json_output();

    return render_version_info(
        collect_version_info(),
        options);
  }

  CliResult Cli::write_and_return(
      CliResult result,
      ::std::ostream &out,
      ::std::ostream &err) const
  {
    const auto write_error = output_.write_result(out, err, result);

    if (write_error)
    {
      return CliResult::failure(write_error, 1);
    }

    return result;
  }

  int run_cli(
      int argc,
      char **argv)
  {
    return run_cli(argc, argv, ::std::cout, ::std::cerr);
  }

  int run_cli(
      int argc,
      char **argv,
      ::std::ostream &out,
      ::std::ostream &err)
  {
    auto cli = Cli::from_args(argc, argv);
    if (!cli)
    {
      OutputOptions options;
      options.color = false;

      Output output(options);

      CliResult result = CliResult::failure(cli.error(), 1);
      (void)output.write_result(out, err, result);

      return result.exit_code;
    }

    const auto result = cli.value().run(argc, argv, out, err);

    return result.exit_code;
  }

} // namespace kordex::cli

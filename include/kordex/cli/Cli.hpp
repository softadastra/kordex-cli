/**
 *
 *  @file Cli.hpp
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

#ifndef KORDEX_CLI_CLI_HPP
#define KORDEX_CLI_CLI_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include <kordex/cli/ArgumentParser.hpp>
#include <kordex/cli/CliConfig.hpp>
#include <kordex/cli/CliOptions.hpp>
#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/CommandRegistry.hpp>
#include <kordex/cli/Commands.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/HelpFormatter.hpp>
#include <kordex/cli/Output.hpp>
#include <kordex/cli/Result.hpp>
#include <kordex/cli/Version.hpp>

namespace kordex::cli
{
  /**
   * @struct CliRunOptions
   * @brief Options used for one CLI run.
   */
  struct CliRunOptions
  {
    /**
     * @brief Raw argv-style arguments.
     */
    ::std::vector<::std::string> args{};

    /**
     * @brief Whether output should be written to streams.
     */
    bool write_output{true};

    /**
     * @brief Return true if args exist.
     */
    [[nodiscard]] bool has_args() const noexcept;
  };

  /**
   * @class Cli
   * @brief Main Kordex CLI facade.
   *
   * Cli owns:
   * - normalized configuration
   * - command registry
   * - argument parser
   * - output renderer
   *
   * It is the main entry point used by the executable.
   */
  class Cli
  {
  public:
    /**
     * @brief Construct CLI using default options.
     */
    Cli();

    /**
     * @brief Construct CLI from explicit options.
     */
    explicit Cli(CliOptions options);

    /**
     * @brief Construct CLI from config and registry.
     */
    Cli(CliConfig config, CommandRegistry registry);

    /**
     * @brief Create CLI from options.
     */
    [[nodiscard]] static Result<Cli> create(
        const CliOptions &options = CliOptions::defaults());

    /**
     * @brief Create CLI from argc/argv.
     */
    [[nodiscard]] static Result<Cli> from_args(
        int argc,
        char **argv);

    /**
     * @brief Return CLI config.
     */
    [[nodiscard]] const CliConfig &config() const noexcept;

    /**
     * @brief Return command registry.
     */
    [[nodiscard]] const CommandRegistry &registry() const noexcept;

    /**
     * @brief Return mutable command registry.
     */
    [[nodiscard]] CommandRegistry &registry() noexcept;

    /**
     * @brief Return argument parser.
     */
    [[nodiscard]] const ArgumentParser &parser() const noexcept;

    /**
     * @brief Return output writer.
     */
    [[nodiscard]] const Output &output() const noexcept;

    /**
     * @brief Return true if CLI has at least one command.
     */
    [[nodiscard]] bool has_commands() const noexcept;

    /**
     * @brief Validate CLI state.
     */
    [[nodiscard]] Error validate() const;

    /**
     * @brief Register one command.
     */
    [[nodiscard]] Error register_command(
        Command command);

    /**
     * @brief Register default built-in commands.
     */
    [[nodiscard]] Error register_defaults();

    /**
     * @brief Run CLI with stored config args.
     */
    [[nodiscard]] CliResult run() const;

    /**
     * @brief Run CLI with vector args.
     */
    [[nodiscard]] CliResult run(
        const ::std::vector<::std::string> &args) const;

    /**
     * @brief Run CLI with argc/argv.
     */
    [[nodiscard]] CliResult run(
        int argc,
        char **argv) const;

    /**
     * @brief Run CLI and write result to streams.
     */
    [[nodiscard]] CliResult run(
        const ::std::vector<::std::string> &args,
        ::std::ostream &out,
        ::std::ostream &err) const;

    /**
     * @brief Run CLI with argc/argv and write result to streams.
     */
    [[nodiscard]] CliResult run(
        int argc,
        char **argv,
        ::std::ostream &out,
        ::std::ostream &err) const;

    /**
     * @brief Run one command directly.
     */
    [[nodiscard]] CliResult run_command(
        const ::std::string &name,
        CommandArguments args = {}) const;

    /**
     * @brief Render top-level help.
     */
    [[nodiscard]] Result<::std::string> help() const;

    /**
     * @brief Render command help.
     */
    [[nodiscard]] Result<::std::string> help(
        const ::std::string &command_name) const;

    /**
     * @brief Render CLI version output.
     */
    [[nodiscard]] ::std::string version_text() const;

  private:
    [[nodiscard]] CliResult write_and_return(
        CliResult result,
        ::std::ostream &out,
        ::std::ostream &err) const;

    CliConfig config_{};
    CommandRegistry registry_{};
    ArgumentParser parser_{};
    Output output_{};
  };

  /**
   * @brief Run Kordex CLI using default options.
   */
  [[nodiscard]] int run_cli(
      int argc,
      char **argv);

  /**
   * @brief Run Kordex CLI with explicit streams.
   */
  [[nodiscard]] int run_cli(
      int argc,
      char **argv,
      ::std::ostream &out,
      ::std::ostream &err);

} // namespace kordex::cli

#endif // KORDEX_CLI_CLI_HPP

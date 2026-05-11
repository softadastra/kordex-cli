/**
 *
 *  @file RunCommand.hpp
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

#ifndef KORDEX_CLI_RUN_COMMAND_HPP
#define KORDEX_CLI_RUN_COMMAND_HPP

#include <string>
#include <vector>

#include <kordex/runtime/Runtime.hpp>
#include <kordex/runtime/RuntimeOptions.hpp>

#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @struct RunCommandOptions
   * @brief Options used by the run command.
   */
  struct RunCommandOptions
  {
    /**
     * @brief Source file to run.
     */
    ::std::string file{};

    /**
     * @brief Arguments passed to the executed source.
     */
    ::std::vector<::std::string> args{};

    /**
     * @brief Whether runtime debug mode is enabled.
     */
    bool debug{false};

    /**
     * @brief Whether runtime diagnostics are enabled.
     */
    bool diagnostics{true};

    /**
     * @brief Whether filesystem access is allowed.
     */
    bool allow_fs{false};

    /**
     * @brief Whether network access is allowed.
     */
    bool allow_net{false};

    /**
     * @brief Whether process access is allowed.
     */
    bool allow_process{false};

    /**
     * @brief Whether environment access is allowed.
     */
    bool allow_env{false};

    /**
     * @brief Return true if a source file was provided.
     */
    [[nodiscard]] bool has_file() const noexcept;

    /**
     * @brief Return true if program arguments were provided.
     */
    [[nodiscard]] bool has_args() const noexcept;
  };

  /**
   * @brief Parse run command arguments.
   */
  [[nodiscard]] Result<RunCommandOptions> parse_run_options(
      const CommandContext &context);

  /**
   * @brief Validate run command options.
   */
  [[nodiscard]] Error validate_run_options(
      const RunCommandOptions &options);

  /**
   * @brief Convert run command options to runtime options.
   */
  [[nodiscard]] kordex::runtime::RuntimeOptions to_runtime_options(
      const RunCommandOptions &options,
      const CliConfig &config);

  /**
   * @brief Run a source file through the Kordex runtime.
   */
  [[nodiscard]] CliResult run_run_command(
      const CommandContext &context);

  /**
   * @brief Create the run command.
   */
  [[nodiscard]] Result<Command> create_run_command();

} // namespace kordex::cli

#endif // KORDEX_CLI_RUN_COMMAND_HPP

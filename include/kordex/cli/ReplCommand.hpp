/**
 *
 *  @file ReplCommand.hpp
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

#ifndef KORDEX_CLI_REPL_COMMAND_HPP
#define KORDEX_CLI_REPL_COMMAND_HPP

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
   * @struct ReplCommandOptions
   * @brief Options used by the repl command.
   */
  struct ReplCommandOptions
  {
    /**
     * @brief Optional source passed directly to the REPL command.
     */
    ::std::string eval{};

    /**
     * @brief Whether runtime debug mode is enabled.
     */
    bool debug{false};

    /**
     * @brief Whether diagnostics are enabled.
     */
    bool diagnostics{true};

    /**
     * @brief Whether filesystem access is allowed.
     */
    bool allow_fs{true};

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
    bool allow_env{true};

    /**
     * @brief Whether interactive mode is requested.
     */
    bool interactive{true};

    /**
     * @brief Return true if eval source exists.
     */
    [[nodiscard]] bool has_eval() const noexcept;
  };

  /**
   * @brief Parse repl command arguments.
   */
  [[nodiscard]] Result<ReplCommandOptions> parse_repl_options(
      const CommandContext &context);

  /**
   * @brief Validate repl command options.
   */
  [[nodiscard]] Error validate_repl_options(
      const ReplCommandOptions &options);

  /**
   * @brief Convert repl options to runtime options.
   */
  [[nodiscard]] kordex::runtime::RuntimeOptions to_runtime_options(
      const ReplCommandOptions &options,
      const CliConfig &config);

  /**
   * @brief Run eval source through the runtime.
   */
  [[nodiscard]] CliResult run_repl_eval(
      const ReplCommandOptions &options,
      const CliConfig &config);

  /**
   * @brief Run the repl command.
   */
  [[nodiscard]] CliResult run_repl_command(
      const CommandContext &context);

  /**
   * @brief Create the repl command.
   */
  [[nodiscard]] Result<Command> create_repl_command();

} // namespace kordex::cli

#endif // KORDEX_CLI_REPL_COMMAND_HPP

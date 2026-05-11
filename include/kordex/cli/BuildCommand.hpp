/**
 *
 *  @file BuildCommand.hpp
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

#ifndef KORDEX_CLI_BUILD_COMMAND_HPP
#define KORDEX_CLI_BUILD_COMMAND_HPP

#include <string>
#include <vector>

#include <kordex/runtime/Manifest.hpp>
#include <kordex/runtime/SourceFile.hpp>

#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @enum BuildKind
   * @brief Type of build requested by the command.
   */
  enum class BuildKind
  {
    SourceFile,
    Project
  };

  /**
   * @struct BuildCommandOptions
   * @brief Options used by the build command.
   */
  struct BuildCommandOptions
  {
    /**
     * @brief Input source file or project directory.
     */
    ::std::string input{};

    /**
     * @brief Optional output directory.
     */
    ::std::string output_dir{"dist"};

    /**
     * @brief Optional output file name.
     */
    ::std::string output_name{};

    /**
     * @brief Build kind.
     */
    BuildKind kind{BuildKind::SourceFile};

    /**
     * @brief Whether minification is requested.
     */
    bool minify{false};

    /**
     * @brief Whether source map generation is requested.
     */
    bool source_maps{false};

    /**
     * @brief Whether detailed build information should be printed.
     */
    bool details{false};

    /**
     * @brief Whether files should be overwritten.
     */
    bool force{false};

    /**
     * @brief Return true if input exists.
     */
    [[nodiscard]] bool has_input() const noexcept;

    /**
     * @brief Return true if output directory exists.
     */
    [[nodiscard]] bool has_output_dir() const noexcept;

    /**
     * @brief Return true if output file name exists.
     */
    [[nodiscard]] bool has_output_name() const noexcept;
  };

  /**
   * @struct BuildReport
   * @brief Report produced by the build command.
   */
  struct BuildReport
  {
    /**
     * @brief Build kind.
     */
    BuildKind kind{BuildKind::SourceFile};

    /**
     * @brief Input path.
     */
    ::std::string input{};

    /**
     * @brief Output path.
     */
    ::std::string output{};

    /**
     * @brief Entry source path.
     */
    ::std::string entry{};

    /**
     * @brief Source type.
     */
    kordex::runtime::SourceType source_type{
        kordex::runtime::SourceType::Unknown};

    /**
     * @brief Source size in bytes.
     */
    ::std::size_t source_size{0};

    /**
     * @brief Whether build succeeded.
     */
    bool ok{false};

    /**
     * @brief Whether source map generation was requested.
     */
    bool source_maps{false};

    /**
     * @brief Whether minification was requested.
     */
    bool minify{false};

    /**
     * @brief Human-readable messages.
     */
    ::std::vector<::std::string> messages{};

    /**
     * @brief Return true if output path exists.
     */
    [[nodiscard]] bool has_output() const noexcept;

    /**
     * @brief Return true if messages exist.
     */
    [[nodiscard]] bool has_messages() const noexcept;
  };

  /**
   * @brief Parse build command arguments.
   */
  [[nodiscard]] Result<BuildCommandOptions> parse_build_options(
      const CommandContext &context);

  /**
   * @brief Validate build command options.
   */
  [[nodiscard]] Error validate_build_options(
      const BuildCommandOptions &options);

  /**
   * @brief Build a source file or project.
   */
  [[nodiscard]] Result<BuildReport> build_input(
      const BuildCommandOptions &options);

  /**
   * @brief Render build report as text.
   */
  [[nodiscard]] ::std::string render_build_report(
      const BuildReport &report,
      const BuildCommandOptions &options);

  /**
   * @brief Run the build command.
   */
  [[nodiscard]] CliResult run_build_command(
      const CommandContext &context);

  /**
   * @brief Create the build command.
   */
  [[nodiscard]] Result<Command> create_build_command();

  /**
   * @brief Convert BuildKind to a stable string.
   */
  [[nodiscard]] const char *to_string(
      BuildKind kind) noexcept;

} // namespace kordex::cli

#endif // KORDEX_CLI_BUILD_COMMAND_HPP

/**
 *
 *  @file Output.hpp
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

#ifndef KORDEX_CLI_OUTPUT_HPP
#define KORDEX_CLI_OUTPUT_HPP

#include <ostream>
#include <string>
#include <string_view>

#include <kordex/cli/CliConfig.hpp>
#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Error.hpp>

namespace kordex::cli
{
  /**
   * @enum OutputLevel
   * @brief Logical output level.
   */
  enum class OutputLevel
  {
    Normal,
    Info,
    Warning,
    Error,
    Debug
  };

  /**
   * @struct OutputOptions
   * @brief Options controlling CLI output rendering.
   */
  struct OutputOptions
  {
    OutputMode mode{OutputMode::Text};

    bool color{true};
    bool verbose{false};
    bool debug{false};
    bool quiet{false};

    /**
     * @brief Return true if text output is selected.
     */
    [[nodiscard]] bool text_output() const noexcept;

    /**
     * @brief Return true if JSON output is selected.
     */
    [[nodiscard]] bool json_output() const noexcept;

    /**
     * @brief Return true if quiet output is selected.
     */
    [[nodiscard]] bool quiet_output() const noexcept;
  };

  /**
   * @class Output
   * @brief CLI output writer.
   *
   * Output centralizes stdout/stderr rendering for text, JSON, and quiet modes.
   */
  class Output
  {
  public:
    /**
     * @brief Construct output writer with default options.
     */
    Output();

    /**
     * @brief Construct output writer with explicit options.
     */
    explicit Output(OutputOptions options);

    /**
     * @brief Create output writer from CLI config.
     */
    [[nodiscard]] static Output from_config(
        const CliConfig &config);

    /**
     * @brief Return output options.
     */
    [[nodiscard]] const OutputOptions &options() const noexcept;

    /**
     * @brief Validate output options.
     */
    [[nodiscard]] Error validate() const;

    /**
     * @brief Write a line to stdout.
     */
    [[nodiscard]] Error write_line(
        ::std::ostream &stream,
        ::std::string_view message) const;

    /**
     * @brief Write a message to stdout without adding a newline.
     */
    [[nodiscard]] Error write(
        ::std::ostream &stream,
        ::std::string_view message) const;

    /**
     * @brief Write a diagnostic line.
     */
    [[nodiscard]] Error write_level(
        ::std::ostream &stream,
        OutputLevel level,
        ::std::string_view message) const;

    /**
     * @brief Write a CLI result to stdout/stderr streams.
     */
    [[nodiscard]] Error write_result(
        ::std::ostream &out,
        ::std::ostream &err,
        const CliResult &result) const;

    /**
     * @brief Render a CLI result to a string.
     */
    [[nodiscard]] ::std::string render_result(
        const CliResult &result) const;

    /**
     * @brief Render an error to a string.
     */
    [[nodiscard]] ::std::string render_error(
        const Error &error) const;

    /**
     * @brief Render a JSON string value with basic escaping.
     */
    [[nodiscard]] static ::std::string json_escape(
        ::std::string_view value);

    /**
     * @brief Return level label.
     */
    [[nodiscard]] static const char *level_label(
        OutputLevel level) noexcept;

    /**
     * @brief Return colored level label if color is enabled.
     */
    [[nodiscard]] ::std::string format_level(
        OutputLevel level) const;

  private:
    [[nodiscard]] ::std::string render_text_result(
        const CliResult &result) const;

    [[nodiscard]] ::std::string render_json_result(
        const CliResult &result) const;

    OutputOptions options_{};
  };

  /**
   * @brief Convert OutputLevel to a stable string.
   */
  [[nodiscard]] const char *to_string(
      OutputLevel level) noexcept;

} // namespace kordex::cli

#endif // KORDEX_CLI_OUTPUT_HPP

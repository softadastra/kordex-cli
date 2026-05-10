/**
 *
 *  @file HelpFormatter.hpp
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

#ifndef KORDEX_CLI_HELP_FORMATTER_HPP
#define KORDEX_CLI_HELP_FORMATTER_HPP

#include <string>
#include <vector>

#include <kordex/cli/CliConfig.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/CommandRegistry.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @struct HelpFormatterOptions
   * @brief Options controlling help text rendering.
   */
  struct HelpFormatterOptions
  {
    /**
     * @brief Program name shown in usage.
     */
    ::std::string executable_name{"kordex"};

    /**
     * @brief Product name shown in the header.
     */
    ::std::string product_name{"Kordex"};

    /**
     * @brief Short product description.
     */
    ::std::string description{
        "A JavaScript runtime for reliable local-first applications."};

    /**
     * @brief Whether hidden commands should be rendered.
     */
    bool show_hidden{false};

    /**
     * @brief Whether command aliases should be rendered.
     */
    bool show_aliases{true};

    /**
     * @brief Whether global options should be rendered.
     */
    bool show_global_options{true};

    /**
     * @brief Whether examples should be rendered.
     */
    bool show_examples{true};

    /**
     * @brief Return true if executable name is configured.
     */
    [[nodiscard]] bool has_executable_name() const noexcept;

    /**
     * @brief Return true if product name is configured.
     */
    [[nodiscard]] bool has_product_name() const noexcept;

    /**
     * @brief Return true if description is configured.
     */
    [[nodiscard]] bool has_description() const noexcept;
  };

  /**
   * @class HelpFormatter
   * @brief Formats CLI help text.
   */
  class HelpFormatter
  {
  public:
    /**
     * @brief Construct with default formatter options.
     */
    HelpFormatter();

    /**
     * @brief Construct with explicit formatter options.
     */
    explicit HelpFormatter(HelpFormatterOptions options);

    /**
     * @brief Create formatter from CLI config.
     */
    [[nodiscard]] static HelpFormatter from_config(
        const CliConfig &config);

    /**
     * @brief Return formatter options.
     */
    [[nodiscard]] const HelpFormatterOptions &options() const noexcept;

    /**
     * @brief Validate formatter options.
     */
    [[nodiscard]] Error validate() const;

    /**
     * @brief Format top-level help.
     */
    [[nodiscard]] Result<::std::string> format(
        const CommandRegistry &registry) const;

    /**
     * @brief Format help for one command.
     */
    [[nodiscard]] Result<::std::string> format_command(
        const Command &command) const;

    /**
     * @brief Format usage header.
     */
    [[nodiscard]] ::std::string format_usage() const;

    /**
     * @brief Format command usage.
     */
    [[nodiscard]] ::std::string format_command_usage(
        const Command &command) const;

    /**
     * @brief Format command list.
     */
    [[nodiscard]] ::std::string format_commands(
        const ::std::vector<Command> &commands) const;

    /**
     * @brief Format global options section.
     */
    [[nodiscard]] ::std::string format_global_options() const;

    /**
     * @brief Format examples section.
     */
    [[nodiscard]] ::std::string format_examples() const;

    /**
     * @brief Format alias list.
     */
    [[nodiscard]] static ::std::string format_aliases(
        const ::std::vector<::std::string> &aliases);

  private:
    HelpFormatterOptions options_{};
  };

} // namespace kordex::cli

#endif // KORDEX_CLI_HELP_FORMATTER_HPP

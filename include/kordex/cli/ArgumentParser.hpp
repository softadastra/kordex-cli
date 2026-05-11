/**
 *
 *  @file ArgumentParser.hpp
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

#ifndef KORDEX_CLI_ARGUMENT_PARSER_HPP
#define KORDEX_CLI_ARGUMENT_PARSER_HPP

#include <string>
#include <vector>

#include <kordex/cli/CliConfig.hpp>
#include <kordex/cli/CliOptions.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @struct ParsedArguments
   * @brief Result of parsing argv-style CLI arguments.
   */
  struct ParsedArguments
  {
    /**
     * @brief Executable name.
     */
    ::std::string executable_name{"kordex"};

    /**
     * @brief Command name.
     */
    ::std::string command{};

    /**
     * @brief Positional arguments after command/options parsing.
     */
    ::std::vector<::std::string> positional{};

    /**
     * @brief Unknown arguments preserved when allowed.
     */
    ::std::vector<::std::string> unknown{};

    /**
     * @brief Requested output mode.
     */
    OutputMode output_mode{OutputMode::Text};

    bool help{false};
    bool version{false};
    bool verbose{false};
    bool debug{false};
    bool quiet{false};
    bool json{false};
    bool no_color{false};
    bool dry_run{false};

    /**
     * @brief Return true if command was parsed.
     */
    [[nodiscard]] bool has_command() const noexcept;

    /**
     * @brief Return true if positional args exist.
     */
    [[nodiscard]] bool has_positional() const noexcept;

    /**
     * @brief Return true if unknown args exist.
     */
    [[nodiscard]] bool has_unknown() const noexcept;

    /**
     * @brief Return true if help or version was requested.
     */
    [[nodiscard]] bool is_meta_request() const noexcept;
  };

  /**
   * @class ArgumentParser
   * @brief Parses raw CLI arguments into CliOptions / ParsedArguments.
   */
  class ArgumentParser
  {
  public:
    /**
     * @brief Construct parser with default options.
     */
    ArgumentParser();

    /**
     * @brief Construct parser with base options.
     */
    explicit ArgumentParser(CliOptions options);

    /**
     * @brief Return base parser options.
     */
    [[nodiscard]] const CliOptions &options() const noexcept;

    /**
     * @brief Parse argc/argv into ParsedArguments.
     */
    [[nodiscard]] Result<ParsedArguments> parse(
        int argc,
        char **argv) const;

    /**
     * @brief Parse vector arguments into ParsedArguments.
     */
    [[nodiscard]] Result<ParsedArguments> parse(
        const ::std::vector<::std::string> &args) const;

    /**
     * @brief Parse vector arguments and produce CliOptions.
     */
    [[nodiscard]] Result<CliOptions> parse_options(
        const ::std::vector<::std::string> &args) const;

    /**
     * @brief Parse argc/argv and produce CliOptions.
     */
    [[nodiscard]] Result<CliOptions> parse_options(
        int argc,
        char **argv) const;

    /**
     * @brief Build CliOptions from parsed arguments.
     */
    [[nodiscard]] Result<CliOptions> to_options(
        const ParsedArguments &parsed) const;

    /**
     * @brief Convert argc/argv to vector strings.
     */
    [[nodiscard]] static Result<::std::vector<::std::string>> argv_to_vector(
        int argc,
        char **argv);

    /**
     * @brief Return true if argument looks like an option.
     */
    [[nodiscard]] static bool is_option(
        const ::std::string &arg) noexcept;

    /**
     * @brief Return true if argument is the option terminator.
     */
    [[nodiscard]] static bool is_option_terminator(
        const ::std::string &arg) noexcept;

    /**
     * @brief Return true if argument is a known global option.
     */
    [[nodiscard]] static bool is_known_global_option(
        const ::std::string &arg) noexcept;

  private:
    [[nodiscard]] Result<ParsedArguments> parse_tokens(
        const ::std::vector<::std::string> &tokens) const;

    CliOptions options_{};
  };

} // namespace kordex::cli

#endif // KORDEX_CLI_ARGUMENT_PARSER_HPP

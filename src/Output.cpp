/**
 *
 *  @file Output.cpp
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

#include <sstream>
#include <utility>

#include <kordex/cli/Output.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] const char *ansi_color(
        OutputLevel level) noexcept
    {
      switch (level)
      {
      case OutputLevel::Normal:
        return "";
      case OutputLevel::Info:
        return "\033[36m";
      case OutputLevel::Warning:
        return "\033[33m";
      case OutputLevel::Error:
        return "\033[31m";
      case OutputLevel::Debug:
        return "\033[35m";
      }

      return "";
    }

    [[nodiscard]] bool should_suppress_level(
        const OutputOptions &options,
        OutputLevel level) noexcept
    {
      if (options.quiet_output())
      {
        return level != OutputLevel::Error;
      }

      if (level == OutputLevel::Debug && !options.debug)
      {
        return true;
      }

      if (level == OutputLevel::Info && !options.verbose)
      {
        return false;
      }

      return false;
    }
  } // namespace

  bool OutputOptions::text_output() const noexcept
  {
    return mode == OutputMode::Text;
  }

  bool OutputOptions::json_output() const noexcept
  {
    return mode == OutputMode::Json;
  }

  bool OutputOptions::quiet_output() const noexcept
  {
    return quiet || mode == OutputMode::Quiet;
  }

  Output::Output()
      : options_()
  {
  }

  Output::Output(
      OutputOptions options)
      : options_(::std::move(options))
  {
  }

  Output Output::from_config(
      const CliConfig &config)
  {
    OutputOptions options;
    options.mode = config.output_mode;
    options.color = config.color;
    options.verbose = config.verbose;
    options.debug = config.debug;
    options.quiet = config.quiet_output();

    return Output(options);
  }

  const OutputOptions &Output::options() const noexcept
  {
    return options_;
  }

  Error Output::validate() const
  {
    if (options_.json_output() && options_.color)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "JSON output cannot use colored text");
    }

    if (options_.quiet_output() && options_.verbose)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "quiet output cannot be combined with verbose mode");
    }

    if (options_.quiet_output() && options_.debug)
    {
      return make_cli_error(
          CliErrorCode::InvalidConfig,
          "quiet output cannot be combined with debug mode");
    }

    return ok();
  }

  Error Output::write_line(
      ::std::ostream &stream,
      ::std::string_view message) const
  {
    const auto validation = validate();
    if (validation)
    {
      return validation;
    }

    if (options_.quiet_output())
    {
      return ok();
    }

    stream << message << '\n';

    if (!stream)
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "failed to write CLI output line");
    }

    return ok();
  }

  Error Output::write(
      ::std::ostream &stream,
      ::std::string_view message) const
  {
    const auto validation = validate();
    if (validation)
    {
      return validation;
    }

    if (options_.quiet_output())
    {
      return ok();
    }

    stream << message;

    if (!stream)
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "failed to write CLI output");
    }

    return ok();
  }

  Error Output::write_level(
      ::std::ostream &stream,
      OutputLevel level,
      ::std::string_view message) const
  {
    const auto validation = validate();
    if (validation)
    {
      return validation;
    }

    if (should_suppress_level(options_, level))
    {
      return ok();
    }

    if (level == OutputLevel::Normal)
    {
      stream << message << '\n';
    }
    else
    {
      stream << format_level(level) << ": " << message << '\n';
    }

    if (!stream)
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "failed to write CLI diagnostic output");
    }

    return ok();
  }

  Error Output::write_result(
      ::std::ostream &out,
      ::std::ostream &err,
      const CliResult &result) const
  {
    const auto validation = validate();
    if (validation)
    {
      return validation;
    }

    if (options_.quiet_output())
    {
      return ok();
    }

    if (result.failed() || result.was_cancelled())
    {
      const auto text = render_result(result);

      if (!text.empty())
      {
        err << text;

        if (!text.empty() && text.back() != '\n')
        {
          err << '\n';
        }
      }

      if (!err)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            "failed to write CLI error output");
      }

      return ok();
    }

    const auto text = render_result(result);

    if (!text.empty())
    {
      out << text;

      if (!text.empty() && text.back() != '\n')
      {
        out << '\n';
      }
    }

    if (!out)
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "failed to write CLI result output");
    }

    return ok();
  }

  ::std::string Output::render_result(
      const CliResult &result) const
  {
    if (options_.quiet_output())
    {
      return {};
    }

    if (options_.json_output())
    {
      return render_json_result(result);
    }

    return render_text_result(result);
  }

  ::std::string Output::render_error(
      const Error &error) const
  {
    if (!error.has_error())
    {
      return {};
    }

    if (options_.json_output())
    {
      ::std::ostringstream stream;
      stream << "{";
      stream << "\"ok\":false,";
      stream << "\"error\":{";
      stream << "\"code\":\"" << json_escape(to_string(CliErrorCode::InternalError)) << "\",";
      stream << "\"message\":\"" << json_escape(error.message()) << "\"";
      stream << "}";
      stream << "}";

      return stream.str();
    }

    ::std::ostringstream stream;
    stream << format_level(OutputLevel::Error) << ": " << error.message();

    return stream.str();
  }

  ::std::string Output::json_escape(
      ::std::string_view value)
  {
    ::std::ostringstream stream;

    for (const char character : value)
    {
      switch (character)
      {
      case '"':
        stream << "\\\"";
        break;
      case '\\':
        stream << "\\\\";
        break;
      case '\b':
        stream << "\\b";
        break;
      case '\f':
        stream << "\\f";
        break;
      case '\n':
        stream << "\\n";
        break;
      case '\r':
        stream << "\\r";
        break;
      case '\t':
        stream << "\\t";
        break;
      default:
        stream << character;
        break;
      }
    }

    return stream.str();
  }

  const char *Output::level_label(
      OutputLevel level) noexcept
  {
    switch (level)
    {
    case OutputLevel::Normal:
      return "";
    case OutputLevel::Info:
      return "info";
    case OutputLevel::Warning:
      return "warning";
    case OutputLevel::Error:
      return "error";
    case OutputLevel::Debug:
      return "debug";
    }

    return "info";
  }

  ::std::string Output::format_level(
      OutputLevel level) const
  {
    const char *label = level_label(level);

    if (!options_.color || level == OutputLevel::Normal)
    {
      return label;
    }

    ::std::ostringstream stream;
    stream << ansi_color(level) << label << "\033[0m";

    return stream.str();
  }

  ::std::string Output::render_text_result(
      const CliResult &result) const
  {
    if (result.failed() || result.was_cancelled())
    {
      if (!result.error_output.empty())
      {
        return result.error_output;
      }

      if (result.error.has_error())
      {
        return render_error(result.error);
      }

      return "error: CLI command failed";
    }

    if (!result.output.empty())
    {
      return result.output;
    }

    return {};
  }

  ::std::string Output::render_json_result(
      const CliResult &result) const
  {
    ::std::ostringstream stream;

    stream << "{";
    stream << "\"status\":\"" << to_string(result.status) << "\",";
    stream << "\"exitCode\":" << result.exit_code << ",";
    stream << "\"ok\":" << (result.succeeded() ? "true" : "false") << ",";
    stream << "\"output\":\"" << json_escape(result.output) << "\",";
    stream << "\"errorOutput\":\"" << json_escape(result.error_output) << "\",";

    stream << "\"error\":";

    if (result.error.has_error())
    {
      stream << "{";
      stream << "\"message\":\"" << json_escape(result.error.message()) << "\"";
      stream << "}";
    }
    else
    {
      stream << "null";
    }

    stream << "}";

    return stream.str();
  }

  const char *to_string(
      OutputLevel level) noexcept
  {
    switch (level)
    {
    case OutputLevel::Normal:
      return "normal";
    case OutputLevel::Info:
      return "info";
    case OutputLevel::Warning:
      return "warning";
    case OutputLevel::Error:
      return "error";
    case OutputLevel::Debug:
      return "debug";
    }

    return "info";
  }

} // namespace kordex::cli

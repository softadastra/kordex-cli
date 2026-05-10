/**
 *
 *  @file CheckCommand.cpp
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

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <utility>

#include <kordex/cli/CheckCommand.hpp>
#include <kordex/cli/Output.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool is_flag(
        const ::std::string &value) noexcept
    {
      return value.size() > 1 && value.front() == '-';
    }

    [[nodiscard]] bool is_space(
        char character) noexcept
    {
      return character == ' ' ||
             character == '\t' ||
             character == '\n' ||
             character == '\r' ||
             character == '\f' ||
             character == '\v';
    }

    [[nodiscard]] ::std::size_t count_lines(
        const ::std::string &content) noexcept
    {
      if (content.empty())
      {
        return 0;
      }

      return static_cast<::std::size_t>(
                 ::std::count(content.begin(), content.end(), '\n')) +
             1;
    }

    [[nodiscard]] bool content_is_blank(
        const ::std::string &content) noexcept
    {
      return ::std::all_of(
          content.begin(),
          content.end(),
          [](char character)
          {
            return is_space(character);
          });
    }

    [[nodiscard]] bool has_trailing_whitespace(
        const ::std::string &line) noexcept
    {
      if (line.empty())
      {
        return false;
      }

      const char last = line.back();

      return last == ' ' || last == '\t' || last == '\r';
    }

    [[nodiscard]] ::std::vector<CheckDiagnostic> collect_warnings(
        const kordex::runtime::SourceFile &source)
    {
      ::std::vector<CheckDiagnostic> diagnostics;

      if (source.content.empty())
      {
        CheckDiagnostic diagnostic;
        diagnostic.level = "warning";
        diagnostic.message = "source file is empty";
        diagnostic.path = source.path;
        diagnostic.line = 0;
        diagnostic.column = 0;
        diagnostics.push_back(::std::move(diagnostic));
        return diagnostics;
      }

      if (content_is_blank(source.content))
      {
        CheckDiagnostic diagnostic;
        diagnostic.level = "warning";
        diagnostic.message = "source file contains only whitespace";
        diagnostic.path = source.path;
        diagnostic.line = 1;
        diagnostic.column = 1;
        diagnostics.push_back(::std::move(diagnostic));
      }

      ::std::istringstream stream(source.content);
      ::std::string line;
      ::std::size_t line_number = 1;

      while (::std::getline(stream, line))
      {
        if (has_trailing_whitespace(line))
        {
          CheckDiagnostic diagnostic;
          diagnostic.level = "warning";
          diagnostic.message = "line has trailing whitespace";
          diagnostic.path = source.path;
          diagnostic.line = line_number;
          diagnostic.column = line.size();
          diagnostics.push_back(::std::move(diagnostic));
        }

        ++line_number;
      }

      return diagnostics;
    }

    [[nodiscard]] CheckDiagnostic make_error_diagnostic(
        const ::std::string &path,
        const ::std::string &message)
    {
      CheckDiagnostic diagnostic;
      diagnostic.level = "error";
      diagnostic.message = message;
      diagnostic.path = path;
      diagnostic.line = 0;
      diagnostic.column = 0;
      return diagnostic;
    }

    [[nodiscard]] ::std::string source_type_name(
        kordex::runtime::SourceType type)
    {
      return ::std::string(kordex::runtime::to_string(type));
    }
  } // namespace

  bool CheckCommandOptions::has_file() const noexcept
  {
    return !file.empty();
  }

  bool CheckDiagnostic::has_message() const noexcept
  {
    return !message.empty();
  }

  bool CheckDiagnostic::has_location() const noexcept
  {
    return !path.empty() && line > 0;
  }

  bool CheckReport::has_diagnostics() const noexcept
  {
    return !diagnostics.empty();
  }

  ::std::size_t CheckReport::error_count() const noexcept
  {
    return static_cast<::std::size_t>(
        ::std::count_if(
            diagnostics.begin(),
            diagnostics.end(),
            [](const CheckDiagnostic &diagnostic)
            {
              return diagnostic.level == "error";
            }));
  }

  ::std::size_t CheckReport::warning_count() const noexcept
  {
    return static_cast<::std::size_t>(
        ::std::count_if(
            diagnostics.begin(),
            diagnostics.end(),
            [](const CheckDiagnostic &diagnostic)
            {
              return diagnostic.level == "warning";
            }));
  }

  Result<CheckCommandOptions> parse_check_options(
      const CommandContext &context)
  {
    CheckCommandOptions options;
    options.json = context.config.json_output();

    for (::std::size_t index = 0; index < context.args.size(); ++index)
    {
      const auto &arg = context.args[index];

      if (arg.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "check arguments cannot contain empty entries");
      }

      if (arg == "--no-warnings")
      {
        options.warnings = false;
        continue;
      }

      if (arg == "--details")
      {
        options.details = true;
        continue;
      }

      if (arg == "--json")
      {
        options.json = true;
        continue;
      }

      if (is_flag(arg))
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "unknown check option: " + arg);
      }

      if (!options.has_file())
      {
        options.file = arg;
        continue;
      }

      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "unexpected check argument: " + arg);
    }

    const auto validation = validate_check_options(options);
    if (validation)
    {
      return validation;
    }

    return options;
  }

  Error validate_check_options(
      const CheckCommandOptions &options)
  {
    if (options.file.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "check command requires a source file");
    }

    if (is_flag(options.file))
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "check source file cannot be an option");
    }

    return ok();
  }

  Result<CheckReport> check_source_file(
      const CheckCommandOptions &options)
  {
    const auto validation = validate_check_options(options);
    if (validation)
    {
      return validation;
    }

    CheckReport report;
    report.file = options.file;
    report.ok = false;

    try
    {
      if (!::std::filesystem::exists(options.file))
      {
        report.diagnostics.push_back(
            make_error_diagnostic(
                options.file,
                "source file does not exist"));

        return report;
      }

      if (!::std::filesystem::is_regular_file(options.file))
      {
        report.diagnostics.push_back(
            make_error_diagnostic(
                options.file,
                "source path is not a file"));

        return report;
      }
    }
    catch (const ::std::filesystem::filesystem_error &exception)
    {
      return make_cli_error(
          CliErrorCode::IoError,
          exception.what());
    }

    auto source = kordex::runtime::SourceFile::load(options.file);
    if (!source)
    {
      report.diagnostics.push_back(
          make_error_diagnostic(
              options.file,
              ::std::string(source.error().message())));

      return report;
    }

    report.file = source.value().path;
    report.source_type = source.value().type;
    report.size = source.value().size();
    report.lines = count_lines(source.value().content);
    report.executable = source.value().executable();

    if (!report.executable)
    {
      report.diagnostics.push_back(
          make_error_diagnostic(
              report.file,
              "source file is not executable by Kordex"));
    }

    if (options.warnings)
    {
      auto warnings = collect_warnings(source.value());

      for (auto &diagnostic : warnings)
      {
        report.diagnostics.push_back(::std::move(diagnostic));
      }
    }

    report.ok = report.error_count() == 0;

    return report;
  }

  ::std::string render_check_report(
      const CheckReport &report,
      const CheckCommandOptions &options)
  {
    if (options.json)
    {
      return render_check_report_json(report);
    }

    ::std::ostringstream stream;

    if (report.ok)
    {
      stream << "Check passed: " << report.file;
    }
    else
    {
      stream << "Check failed: " << report.file;
    }

    stream << '\n';

    if (options.details)
    {
      stream << "type       = " << source_type_name(report.source_type) << '\n';
      stream << "size       = " << report.size << " bytes\n";
      stream << "lines      = " << report.lines << '\n';
      stream << "executable = " << (report.executable ? "yes" : "no") << '\n';
      stream << "errors     = " << report.error_count() << '\n';
      stream << "warnings   = " << report.warning_count() << '\n';
    }

    for (const auto &diagnostic : report.diagnostics)
    {
      stream << diagnostic.level << ": ";

      if (diagnostic.has_location())
      {
        stream << diagnostic.path << ':' << diagnostic.line;

        if (diagnostic.column > 0)
        {
          stream << ':' << diagnostic.column;
        }

        stream << ": ";
      }

      stream << diagnostic.message << '\n';
    }

    return stream.str();
  }

  ::std::string render_check_report_json(
      const CheckReport &report)
  {
    ::std::ostringstream stream;

    stream << "{";
    stream << "\"ok\":" << (report.ok ? "true" : "false") << ",";
    stream << "\"file\":\"" << Output::json_escape(report.file) << "\",";
    stream << "\"type\":\"" << Output::json_escape(source_type_name(report.source_type)) << "\",";
    stream << "\"size\":" << report.size << ",";
    stream << "\"lines\":" << report.lines << ",";
    stream << "\"executable\":" << (report.executable ? "true" : "false") << ",";
    stream << "\"errors\":" << report.error_count() << ",";
    stream << "\"warnings\":" << report.warning_count() << ",";
    stream << "\"diagnostics\":[";

    for (::std::size_t index = 0; index < report.diagnostics.size(); ++index)
    {
      const auto &diagnostic = report.diagnostics[index];

      if (index > 0)
      {
        stream << ',';
      }

      stream << "{";
      stream << "\"level\":\"" << Output::json_escape(diagnostic.level) << "\",";
      stream << "\"message\":\"" << Output::json_escape(diagnostic.message) << "\",";
      stream << "\"path\":\"" << Output::json_escape(diagnostic.path) << "\",";
      stream << "\"line\":" << diagnostic.line << ",";
      stream << "\"column\":" << diagnostic.column;
      stream << "}";
    }

    stream << "]";
    stream << "}";

    return stream.str();
  }

  CliResult run_check_command(
      const CommandContext &context)
  {
    auto options = parse_check_options(context);
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    if (context.config.dry_run)
    {
      ::std::ostringstream stream;
      stream << "Would check " << options.value().file;

      return CliResult::success(stream.str());
    }

    auto report = check_source_file(options.value());
    if (!report)
    {
      return CliResult::failure(report.error(), 1);
    }

    const auto output = render_check_report(
        report.value(),
        options.value());

    if (!report.value().ok)
    {
      CliResult result = CliResult::failure(
          make_cli_error(
              CliErrorCode::RuntimeError,
              "source check failed"),
          1);

      result.error_output = output;

      return result;
    }

    return CliResult::success(output);
  }

  Result<Command> create_check_command()
  {
    CommandInfo info;
    info.name = "check";
    info.aliases = {};
    info.summary = "Check a JavaScript or TypeScript file";
    info.description =
        "Check that a source file can be loaded and executed by the Kordex runtime.";
    info.usage = "kordex check <file> [--details] [--no-warnings]";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        ::std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return run_check_command(context);
        });
  }

} // namespace kordex::cli

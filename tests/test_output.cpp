/**
 *
 *  @file test_output.cpp
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
#include <string_view>

#include <kordex/cli/Output.hpp>

namespace
{
  [[nodiscard]] bool expect_true(
      bool condition,
      const char *message)
  {
    if (!condition)
    {
      (void)message;
      return false;
    }

    return true;
  }

  [[nodiscard]] bool contains(
      const ::std::string &text,
      ::std::string_view needle)
  {
    return text.find(needle) != ::std::string::npos;
  }

  [[nodiscard]] bool test_output_options_helpers()
  {
    kordex::cli::OutputOptions options;

    const bool default_state =
        options.text_output() &&
        !options.json_output() &&
        !options.quiet_output();

    options.mode = kordex::cli::OutputMode::Json;

    const bool json_state =
        options.json_output() &&
        !options.text_output() &&
        !options.quiet_output();

    options.mode = kordex::cli::OutputMode::Quiet;

    const bool quiet_state =
        options.quiet_output() &&
        !options.text_output() &&
        !options.json_output();

    return expect_true(
               default_state,
               "default output options should be text") &&
           expect_true(
               json_state,
               "json output options should be json") &&
           expect_true(
               quiet_state,
               "quiet output options should be quiet");
  }

  [[nodiscard]] bool test_output_from_config()
  {
    auto config = kordex::cli::CliConfig::from_options(
        kordex::cli::CliOptions::test());

    if (!expect_true(
            !(!config),
            "test config should be valid"))
    {
      return false;
    }

    auto output = kordex::cli::Output::from_config(config.value());

    return expect_true(
               output.options().mode == kordex::cli::OutputMode::Text,
               "output mode should come from config") &&
           expect_true(
               !output.options().color,
               "output color should come from config") &&
           expect_true(
               output.options().debug,
               "output debug should come from config") &&
           expect_true(
               !output.validate(),
               "output from test config should validate");
  }

  [[nodiscard]] bool test_write_line()
  {
    kordex::cli::Output output;
    ::std::ostringstream stream;

    const auto error = output.write_line(stream, "hello");

    return expect_true(
               !error,
               "write_line should succeed") &&
           expect_true(
               stream.str() == "hello\n",
               "write_line should add newline");
  }

  [[nodiscard]] bool test_write()
  {
    kordex::cli::Output output;
    ::std::ostringstream stream;

    const auto error = output.write(stream, "hello");

    return expect_true(
               !error,
               "write should succeed") &&
           expect_true(
               stream.str() == "hello",
               "write should not add newline");
  }

  [[nodiscard]] bool test_quiet_write_suppresses_output()
  {
    kordex::cli::OutputOptions options;
    options.mode = kordex::cli::OutputMode::Quiet;

    kordex::cli::Output output(options);
    ::std::ostringstream stream;

    const auto line_error = output.write_line(stream, "hello");
    const auto write_error = output.write(stream, "world");

    return expect_true(
               !line_error && !write_error,
               "quiet writes should succeed") &&
           expect_true(
               stream.str().empty(),
               "quiet writes should suppress output");
  }

  [[nodiscard]] bool test_write_level()
  {
    kordex::cli::OutputOptions options;
    options.color = false;
    options.verbose = true;
    options.debug = true;

    kordex::cli::Output output(options);
    ::std::ostringstream stream;

    const auto info_error = output.write_level(
        stream,
        kordex::cli::OutputLevel::Info,
        "hello");

    const auto debug_error = output.write_level(
        stream,
        kordex::cli::OutputLevel::Debug,
        "debugging");

    return expect_true(
               !info_error && !debug_error,
               "write_level should succeed") &&
           expect_true(
               contains(stream.str(), "info: hello"),
               "info output should contain label") &&
           expect_true(
               contains(stream.str(), "debug: debugging"),
               "debug output should contain label");
  }

  [[nodiscard]] bool test_debug_suppressed_when_disabled()
  {
    kordex::cli::OutputOptions options;
    options.color = false;
    options.debug = false;

    kordex::cli::Output output(options);
    ::std::ostringstream stream;

    const auto error = output.write_level(
        stream,
        kordex::cli::OutputLevel::Debug,
        "debugging");

    return expect_true(
               !error,
               "suppressed debug output should succeed") &&
           expect_true(
               stream.str().empty(),
               "debug output should be suppressed");
  }

  [[nodiscard]] bool test_render_success_result()
  {
    kordex::cli::OutputOptions options;
    options.color = false;

    kordex::cli::Output output(options);

    const auto rendered = output.render_result(
        kordex::cli::CliResult::success("done"));

    return expect_true(
        rendered == "done",
        "success result should render output");
  }

  [[nodiscard]] bool test_render_failure_result()
  {
    kordex::cli::OutputOptions options;
    options.color = false;

    kordex::cli::Output output(options);

    const auto result = kordex::cli::CliResult::failure(
        kordex::cli::make_cli_error(
            kordex::cli::CliErrorCode::InvalidArgument,
            "bad arg"),
        1);

    const auto rendered = output.render_result(result);

    return expect_true(
               contains(rendered, "error: bad arg"),
               "failure result should render error") &&
           expect_true(
               !contains(rendered, "\033"),
               "color disabled output should not contain ANSI");
  }

  [[nodiscard]] bool test_render_failure_prefers_error_output()
  {
    kordex::cli::Output output;

    auto result = kordex::cli::CliResult::failure(
        kordex::cli::make_cli_error(
            kordex::cli::CliErrorCode::InvalidArgument,
            "bad arg"),
        1);

    result.error_output = "custom stderr";

    const auto rendered = output.render_result(result);

    return expect_true(
        rendered == "custom stderr",
        "failure should prefer explicit error output");
  }

  [[nodiscard]] bool test_render_quiet_result()
  {
    kordex::cli::OutputOptions options;
    options.mode = kordex::cli::OutputMode::Quiet;

    kordex::cli::Output output(options);

    const auto rendered = output.render_result(
        kordex::cli::CliResult::success("done"));

    return expect_true(
        rendered.empty(),
        "quiet result rendering should be empty");
  }

  [[nodiscard]] bool test_render_json_result()
  {
    kordex::cli::OutputOptions options;
    options.mode = kordex::cli::OutputMode::Json;
    options.color = false;

    kordex::cli::Output output(options);

    const auto rendered = output.render_result(
        kordex::cli::CliResult::success("done"));

    return expect_true(
               contains(rendered, "\"status\":\"success\""),
               "json result should contain status") &&
           expect_true(
               contains(rendered, "\"exitCode\":0"),
               "json result should contain exit code") &&
           expect_true(
               contains(rendered, "\"ok\":true"),
               "json result should contain ok true") &&
           expect_true(
               contains(rendered, "\"output\":\"done\""),
               "json result should contain output");
  }

  [[nodiscard]] bool test_render_json_failure()
  {
    kordex::cli::OutputOptions options;
    options.mode = kordex::cli::OutputMode::Json;
    options.color = false;

    kordex::cli::Output output(options);

    const auto result = kordex::cli::CliResult::failure(
        kordex::cli::make_cli_error(
            kordex::cli::CliErrorCode::CommandNotFound,
            "missing command"),
        1);

    const auto rendered = output.render_result(result);

    return expect_true(
               contains(rendered, "\"status\":\"failed\""),
               "json failure should contain failed status") &&
           expect_true(
               contains(rendered, "\"ok\":false"),
               "json failure should contain ok false") &&
           expect_true(
               contains(rendered, "\"message\":\"missing command\""),
               "json failure should contain error message");
  }

  [[nodiscard]] bool test_write_result_success()
  {
    kordex::cli::Output output;
    ::std::ostringstream out;
    ::std::ostringstream err;

    const auto write_error = output.write_result(
        out,
        err,
        kordex::cli::CliResult::success("done"));

    return expect_true(
               !write_error,
               "write_result success should succeed") &&
           expect_true(
               out.str() == "done\n",
               "success result should write to stdout") &&
           expect_true(
               err.str().empty(),
               "success result should not write to stderr");
  }

  [[nodiscard]] bool test_write_result_failure()
  {
    kordex::cli::OutputOptions options;
    options.color = false;

    kordex::cli::Output output(options);
    ::std::ostringstream out;
    ::std::ostringstream err;

    const auto result = kordex::cli::CliResult::failure(
        kordex::cli::make_cli_error(
            kordex::cli::CliErrorCode::InvalidArgument,
            "bad arg"),
        1);

    const auto write_error = output.write_result(out, err, result);

    return expect_true(
               !write_error,
               "write_result failure should succeed") &&
           expect_true(
               out.str().empty(),
               "failure result should not write to stdout") &&
           expect_true(
               contains(err.str(), "error: bad arg"),
               "failure result should write to stderr");
  }

  [[nodiscard]] bool test_json_escape()
  {
    const auto escaped = kordex::cli::Output::json_escape(
        "hello \"kordex\"\npath\\file");

    return expect_true(
        escaped == "hello \\\"kordex\\\"\\npath\\\\file",
        "json_escape should escape quotes, newline, and backslash");
  }

  [[nodiscard]] bool test_level_labels()
  {
    return expect_true(
               ::std::string_view(kordex::cli::Output::level_label(
                   kordex::cli::OutputLevel::Normal)) == "",
               "normal label should be empty") &&
           expect_true(
               ::std::string_view(kordex::cli::Output::level_label(
                   kordex::cli::OutputLevel::Info)) == "info",
               "info label should match") &&
           expect_true(
               ::std::string_view(kordex::cli::Output::level_label(
                   kordex::cli::OutputLevel::Warning)) == "warning",
               "warning label should match") &&
           expect_true(
               ::std::string_view(kordex::cli::Output::level_label(
                   kordex::cli::OutputLevel::Error)) == "error",
               "error label should match") &&
           expect_true(
               ::std::string_view(kordex::cli::Output::level_label(
                   kordex::cli::OutputLevel::Debug)) == "debug",
               "debug label should match");
  }

  [[nodiscard]] bool test_output_level_strings()
  {
    return expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::OutputLevel::Normal)) == "normal",
               "normal string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::OutputLevel::Info)) == "info",
               "info string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::OutputLevel::Warning)) == "warning",
               "warning string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::OutputLevel::Error)) == "error",
               "error string should match") &&
           expect_true(
               ::std::string_view(kordex::cli::to_string(
                   kordex::cli::OutputLevel::Debug)) == "debug",
               "debug string should match");
  }

  [[nodiscard]] bool test_invalid_json_color()
  {
    kordex::cli::OutputOptions options;
    options.mode = kordex::cli::OutputMode::Json;
    options.color = true;

    kordex::cli::Output output(options);

    const auto error = output.validate();

    return expect_true(
               error.has_error(),
               "json color output should fail validation") &&
           expect_true(
               error.code() == vix::error::ErrorCode::ConfigError,
               "json color should map to config error");
  }

  [[nodiscard]] bool test_invalid_quiet_verbose()
  {
    kordex::cli::OutputOptions options;
    options.mode = kordex::cli::OutputMode::Quiet;
    options.verbose = true;

    kordex::cli::Output output(options);

    const auto error = output.validate();

    return expect_true(
               error.has_error(),
               "quiet verbose output should fail validation") &&
           expect_true(
               error.code() == vix::error::ErrorCode::ConfigError,
               "quiet verbose should map to config error");
  }

  [[nodiscard]] bool test_invalid_quiet_debug()
  {
    kordex::cli::OutputOptions options;
    options.mode = kordex::cli::OutputMode::Quiet;
    options.debug = true;

    kordex::cli::Output output(options);

    const auto error = output.validate();

    return expect_true(
               error.has_error(),
               "quiet debug output should fail validation") &&
           expect_true(
               error.code() == vix::error::ErrorCode::ConfigError,
               "quiet debug should map to config error");
  }
} // namespace

int main()
{
  const bool ok =
      test_output_options_helpers() &&
      test_output_from_config() &&
      test_write_line() &&
      test_write() &&
      test_quiet_write_suppresses_output() &&
      test_write_level() &&
      test_debug_suppressed_when_disabled() &&
      test_render_success_result() &&
      test_render_failure_result() &&
      test_render_failure_prefers_error_output() &&
      test_render_quiet_result() &&
      test_render_json_result() &&
      test_render_json_failure() &&
      test_write_result_success() &&
      test_write_result_failure() &&
      test_json_escape() &&
      test_level_labels() &&
      test_output_level_strings() &&
      test_invalid_json_color() &&
      test_invalid_quiet_verbose() &&
      test_invalid_quiet_debug();

  return ok ? 0 : 1;
}

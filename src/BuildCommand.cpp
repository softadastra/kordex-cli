/**
 *
 *  @file BuildCommand.cpp
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

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <kordex/bindings/EngineContext.hpp>
#include <kordex/bindings/ModuleLoader.hpp>
#include <kordex/bindings/Script.hpp>
#include <kordex/runtime/SourceFile.hpp>
#include <kordex/std/Std.hpp>
#include <kordex/std/StdOptions.hpp>
#include <kordex/cli/ProjectDiscovery.hpp>
#include <kordex/cli/BuildCommand.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool is_flag(
        const ::std::string &value) noexcept
    {
      return value.size() > 1 && value.front() == '-';
    }

    [[nodiscard]] bool ends_with(
        const ::std::string &value,
        const ::std::string &suffix) noexcept
    {
      return value.size() >= suffix.size() &&
             value.compare(
                 value.size() - suffix.size(),
                 suffix.size(),
                 suffix) == 0;
    }

    [[nodiscard]] ::std::string trim(
        ::std::string value)
    {
      auto not_space = [](unsigned char character)
      {
        return !::std::isspace(character);
      };

      value.erase(
          value.begin(),
          ::std::find_if(value.begin(), value.end(), not_space));

      value.erase(
          ::std::find_if(value.rbegin(), value.rend(), not_space).base(),
          value.end());

      return value;
    }

    [[nodiscard]] ::std::string read_text_file(
        const ::std::filesystem::path &path)
    {
      ::std::ifstream input(path, ::std::ios::binary);
      if (!input)
      {
        return {};
      }

      ::std::ostringstream stream;
      stream << input.rdbuf();

      return stream.str();
    }

    [[nodiscard]] ::std::string json_escape_string(
        const ::std::string &value)
    {
      ::std::string result;
      result.reserve(value.size() + 8);

      for (char character : value)
      {
        switch (character)
        {
        case '\\':
          result += "\\\\";
          break;
        case '"':
          result += "\\\"";
          break;
        case '\n':
          result += "\\n";
          break;
        case '\r':
          result += "\\r";
          break;
        case '\t':
          result += "\\t";
          break;
        default:
          result += character;
          break;
        }
      }

      return result;
    }

    [[nodiscard]] ::std::string relative_source_path(
        const ::std::filesystem::path &source,
        const ::std::filesystem::path &base)
    {
      try
      {
        return ::std::filesystem::relative(source, base).string();
      }
      catch (...)
      {
        return source.lexically_normal().string();
      }
    }

    [[nodiscard]] ::std::string build_source_map_json(
        const BuildReport &report,
        const ::std::filesystem::path &output_path,
        const ::std::string &generated_file_name)
    {
      const auto entry_path = ::std::filesystem::path(report.entry);
      const auto output_dir = output_path.parent_path();

      const ::std::string source_name = relative_source_path(
          entry_path,
          output_dir.empty() ? ::std::filesystem::current_path() : output_dir);

      const ::std::string source_content = read_text_file(entry_path);

      ::std::ostringstream stream;

      stream << "{\n";
      stream << "  \"version\": 3,\n";
      stream << "  \"file\": \""
             << json_escape_string(generated_file_name)
             << "\",\n";

      stream << "  \"sources\": [";
      stream << "\""
             << json_escape_string(source_name)
             << "\"";
      stream << "],\n";

      stream << "  \"sourcesContent\": [";
      stream << "\""
             << json_escape_string(source_content)
             << "\"";
      stream << "],\n";

      stream << "  \"names\": [],\n";

      /*
       * MVP source map.
       *
       * Empty mappings are valid source-map v3.
       * They preserve source identity and source content now, while leaving
       * detailed line/column mappings for the next bundler iteration.
       */
      stream << "  \"mappings\": \"\"\n";
      stream << "}\n";

      return stream.str();
    }

    [[nodiscard]] Error write_source_map_file(
        const BuildReport &report,
        const ::std::filesystem::path &output_path,
        bool force)
    {
      const ::std::filesystem::path map_path =
          ::std::filesystem::path(output_path.string() + ".map");

      const ::std::string map_json = build_source_map_json(
          report,
          output_path,
          output_path.filename().string());

      return write_text_file(
          map_path,
          map_json,
          force);
    }

    [[nodiscard]] ::std::string append_source_mapping_url(
        ::std::string output,
        const ::std::filesystem::path &output_path)
    {
      const ::std::string map_name =
          output_path.filename().string() + ".map";

      if (!output.empty() && output.back() != '\n')
      {
        output += '\n';
      }

      output += "\n//# sourceMappingURL=";
      output += map_name;
      output += '\n';

      return output;
    }

    [[nodiscard]] Error write_text_file(
        const ::std::filesystem::path &path,
        const ::std::string &content,
        bool force)
    {
      try
      {
        if (::std::filesystem::exists(path) && !force)
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "output file already exists, use --force to overwrite: " +
                  path.string());
        }

        const auto parent = path.parent_path();
        if (!parent.empty())
        {
          ::std::filesystem::create_directories(parent);
        }

        ::std::ofstream output(path, ::std::ios::binary | ::std::ios::trunc);
        if (!output)
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "failed to open output file: " + path.string());
        }

        output << content;

        if (!output)
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "failed to write output file: " + path.string());
        }
      }
      catch (const ::std::filesystem::filesystem_error &exception)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            exception.what());
      }

      return ok();
    }

    [[nodiscard]] ::std::string extract_json_string(
        const ::std::string &json,
        const ::std::string &key)
    {
      const ::std::regex pattern(
          "\"" + key + "\"\\s*:\\s*\"([^\"]+)\"");

      ::std::smatch match;

      if (::std::regex_search(json, match, pattern) &&
          match.size() >= 2)
      {
        return match[1].str();
      }

      return {};
    }

    [[nodiscard]] Result<::std::string> resolve_project_entry(
        const ::std::filesystem::path &project_dir)
    {
      ProjectDiscoveryOptions discovery_options;
      discovery_options.start_directory = project_dir.string();
      discovery_options.search_parents = false;

      ProjectDiscovery discovery(discovery_options);

      return discovery.resolve_entry();
    }

    [[nodiscard]] Result<::std::string> resolve_entry(
        const BuildCommandOptions &options)
    {
      const ::std::filesystem::path input(options.input);

      if (options.kind == BuildKind::Project ||
          (::std::filesystem::exists(input) &&
           ::std::filesystem::is_directory(input)))
      {
        return resolve_project_entry(input);
      }

      return input.lexically_normal().string();
    }

    [[nodiscard]] ::std::string default_output_name(
        const ::std::string &entry)
    {
      const ::std::filesystem::path path(entry);
      ::std::string name = path.stem().string();

      if (name.empty())
      {
        name = "main";
      }

      return name + ".js";
    }

    [[nodiscard]] ::std::filesystem::path output_path_for(
        const BuildCommandOptions &options,
        const ::std::string &entry)
    {
      ::std::string output_name = options.output_name;

      if (output_name.empty())
      {
        output_name = default_output_name(entry);
      }

      if (!ends_with(output_name, ".js"))
      {
        output_name += ".js";
      }

      return ::std::filesystem::path(options.output_dir) / output_name;
    }

    [[nodiscard]] ::std::string simple_minify(
        const ::std::string &source)
    {
      ::std::ostringstream output;

      bool in_single_quote = false;
      bool in_double_quote = false;
      bool in_template = false;
      bool escaped = false;
      bool previous_space = false;

      for (::std::size_t index = 0; index < source.size(); ++index)
      {
        const char character = source[index];

        if (escaped)
        {
          output << character;
          escaped = false;
          previous_space = false;
          continue;
        }

        if (character == '\\' &&
            (in_single_quote || in_double_quote || in_template))
        {
          output << character;
          escaped = true;
          previous_space = false;
          continue;
        }

        if (character == '\'' && !in_double_quote && !in_template)
        {
          in_single_quote = !in_single_quote;
          output << character;
          previous_space = false;
          continue;
        }

        if (character == '"' && !in_single_quote && !in_template)
        {
          in_double_quote = !in_double_quote;
          output << character;
          previous_space = false;
          continue;
        }

        if (character == '`' && !in_single_quote && !in_double_quote)
        {
          in_template = !in_template;
          output << character;
          previous_space = false;
          continue;
        }

        if (in_single_quote || in_double_quote || in_template)
        {
          output << character;
          previous_space = false;
          continue;
        }

        if (::std::isspace(static_cast<unsigned char>(character)))
        {
          if (!previous_space)
          {
            output << ' ';
            previous_space = true;
          }

          continue;
        }

        output << character;
        previous_space = false;
      }

      return trim(output.str());
    }

    [[nodiscard]] Result<::std::string> bundle_entry(
        const ::std::string &entry,
        BuildReport &report)
    {
      auto script = kordex::bindings::Script::load(entry);
      if (!script)
      {
        return make_cli_error(
            CliErrorCode::BindingError,
            ::std::string(script.error().message()));
      }

      kordex::bindings::BindingConfig binding_config;
      binding_config.engine_name = "kordex-build";
      binding_config.backend = kordex::bindings::EngineBackend::Native;
      binding_config.module_policy = kordex::bindings::ModulePolicy::Full;
      binding_config.allow_native_modules = true;
      binding_config.allow_native_functions = true;
      binding_config.allow_runtime_bridge = true;

      kordex::bindings::EngineContext context(binding_config);
      const auto init_error = context.initialize();
      if (init_error)
      {
        return make_cli_error(
            CliErrorCode::BindingError,
            ::std::string(init_error.message()));
      }

      auto std_options = kordex::standard::StdOptions::production();
      std_options.enable_process = false;
      std_options.enable_http = false;

      const auto std_error = kordex::standard::install(
          context,
          std_options);

      if (std_error)
      {
        return make_cli_error(
            CliErrorCode::StdError,
            ::std::string(std_error.message()));
      }

      kordex::bindings::ModuleLoader loader(&context);

      auto loaded = loader.load_entry(script.value());
      if (!loaded)
      {
        return make_cli_error(
            CliErrorCode::BindingError,
            ::std::string(loaded.error().message()));
      }

      report.messages.push_back(
          "modules bundled: " +
          ::std::to_string(loaded.value().report.loaded_modules.size()));

      if (loaded.value().report.used_cache)
      {
        report.messages.push_back("module cache used");
      }

      return loaded.value().script.source();
    }

    [[nodiscard]] Error ensure_input_valid(
        const BuildCommandOptions &options)
    {
      if (options.input.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "build command requires an input file or project directory");
      }

      try
      {
        const ::std::filesystem::path input(options.input);

        if (!::std::filesystem::exists(input))
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "build input does not exist: " + options.input);
        }

        if (options.kind == BuildKind::SourceFile &&
            ::std::filesystem::is_directory(input))
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "build input is a directory, use --project");
        }
      }
      catch (const ::std::filesystem::filesystem_error &exception)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            exception.what());
      }

      return ok();
    }
  } // namespace

  bool BuildCommandOptions::has_input() const noexcept
  {
    return !input.empty();
  }

  bool BuildCommandOptions::has_output_dir() const noexcept
  {
    return !output_dir.empty();
  }

  bool BuildCommandOptions::has_output_name() const noexcept
  {
    return !output_name.empty();
  }

  bool BuildReport::has_output() const noexcept
  {
    return !output.empty();
  }

  bool BuildReport::has_messages() const noexcept
  {
    return !messages.empty();
  }

  Result<BuildCommandOptions> parse_build_options(
      const CommandContext &context)
  {
    BuildCommandOptions options;
    options.output_dir = "dist";
    options.kind = BuildKind::SourceFile;

    for (::std::size_t index = 0; index < context.args.size(); ++index)
    {
      const auto &arg = context.args[index];

      if (arg.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "build arguments cannot contain empty entries");
      }

      if (arg == "--project")
      {
        options.kind = BuildKind::Project;
        continue;
      }

      if (arg == "--file")
      {
        options.kind = BuildKind::SourceFile;
        continue;
      }

      if (arg == "--minify")
      {
        options.minify = true;
        continue;
      }

      if (arg == "--source-map" || arg == "--source-maps")
      {
        options.source_maps = true;
        continue;
      }

      if (arg == "--details")
      {
        options.details = true;
        continue;
      }

      if (arg == "--force" || arg == "-f")
      {
        options.force = true;
        continue;
      }

      if (arg == "--out-dir" || arg == "-o")
      {
        if (index + 1 >= context.args.size())
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "missing value for --out-dir");
        }

        options.output_dir = context.args[++index];
        continue;
      }

      if (arg == "--out-file")
      {
        if (index + 1 >= context.args.size())
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "missing value for --out-file");
        }

        options.output_name = context.args[++index];
        continue;
      }

      if (is_flag(arg))
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "unknown build option: " + arg);
      }

      if (!options.has_input())
      {
        options.input = arg;
        continue;
      }

      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "unexpected build argument: " + arg);
    }

    const auto validation = validate_build_options(options);
    if (validation)
    {
      return validation;
    }

    return options;
  }

  Error validate_build_options(
      const BuildCommandOptions &options)
  {
    if (options.input.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "build command requires an input file or project directory");
    }

    if (options.output_dir.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "build output directory cannot be empty");
    }

    return ensure_input_valid(options);
  }

  Result<BuildReport> build_input(
      const BuildCommandOptions &options)
  {
    BuildReport report;
    report.kind = options.kind;
    report.input = options.input;
    report.source_maps = options.source_maps;
    report.minify = options.minify;

    auto entry = resolve_entry(options);
    if (!entry)
    {
      return entry.error();
    }

    report.entry = entry.value();

    auto source = kordex::runtime::SourceFile::load(report.entry);
    if (!source)
    {
      return make_cli_error(
          CliErrorCode::RuntimeError,
          ::std::string(source.error().message()));
    }

    report.source_type = source.value().type;
    report.source_size = source.value().size();

    auto bundled = bundle_entry(
        report.entry,
        report);

    if (!bundled)
    {
      return bundled.error();
    }

    ::std::string output = bundled.value();

    if (options.minify)
    {
      output = simple_minify(output);
      report.messages.push_back("minified output");
    }

    const auto output_path = output_path_for(
        options,
        report.entry);

    if (options.source_maps)
    {
      output = append_source_mapping_url(
          output,
          output_path);
    }

    const auto write_error = write_text_file(
        output_path,
        output,
        options.force);

    if (write_error)
    {
      return write_error;
    }

    if (options.source_maps)
    {
      const auto map_error = write_source_map_file(
          report,
          output_path,
          options.force);

      if (map_error)
      {
        return map_error;
      }

      report.messages.push_back(
          "source map generated: " + output_path.string() + ".map");
    }

    report.output = output_path.string();
    report.ok = true;

    return report;
  }

  ::std::string render_build_report(
      const BuildReport &report,
      const BuildCommandOptions &options)
  {
    ::std::ostringstream stream;

    if (!report.ok)
    {
      stream << "Build failed";
      return stream.str();
    }

    stream << "Build completed\n";
    stream << "input  = " << report.input << '\n';
    stream << "entry  = " << report.entry << '\n';
    stream << "output = " << report.output;

    if (options.details)
    {
      stream << '\n';
      stream << "kind   = " << to_string(report.kind) << '\n';
      stream << "type   = "
             << kordex::runtime::to_string(report.source_type)
             << '\n';
      stream << "size   = " << report.source_size << " bytes";

      if (report.has_messages())
      {
        for (const auto &message : report.messages)
        {
          stream << '\n'
                 << "- " << message;
        }
      }
    }

    return stream.str();
  }

  CliResult run_build_command(
      const CommandContext &context)
  {
    auto options = parse_build_options(context);
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    if (context.config.dry_run)
    {
      ::std::ostringstream stream;
      stream << "Would build " << options.value().input;
      stream << " into " << options.value().output_dir;

      return CliResult::success(stream.str());
    }

    auto report = build_input(options.value());
    if (!report)
    {
      return CliResult::failure(report.error(), 1);
    }

    return CliResult::success(
        render_build_report(report.value(), options.value()));
  }

  Result<Command> create_build_command()
  {
    CommandInfo info;
    info.name = "build";
    info.aliases = {};
    info.summary = "Bundle a JavaScript or TypeScript file";
    info.description =
        "Analyze imports, bundle modules, and generate a runnable JavaScript output file.";
    info.usage =
        "kordex build <file|project> [--project] [--out-dir dist] [--out-file main.js] [--minify] [--force]";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        ::std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return run_build_command(context);
        });
  }

  const char *to_string(
      BuildKind kind) noexcept
  {
    switch (kind)
    {
    case BuildKind::SourceFile:
      return "source_file";
    case BuildKind::Project:
      return "project";
    }

    return "source_file";
  }

} // namespace kordex::cli

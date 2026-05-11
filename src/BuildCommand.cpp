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

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

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

    [[nodiscard]] ::std::string default_output_name_for_file(
        const ::std::string &file)
    {
      const ::std::filesystem::path path(file);

      auto stem = path.stem().string();
      if (stem.empty())
      {
        stem = "main";
      }

      return stem + ".js";
    }

    [[nodiscard]] ::std::filesystem::path project_manifest_path(
        const ::std::filesystem::path &root)
    {
      const auto kordex_json = root / "kordex.json";
      if (::std::filesystem::exists(kordex_json))
      {
        return kordex_json;
      }

      return root / "package.json";
    }

    [[nodiscard]] ::std::filesystem::path make_output_path(
        const BuildCommandOptions &options)
    {
      const ::std::filesystem::path output_dir(options.output_dir);

      const ::std::string output_name =
          options.output_name.empty()
              ? default_output_name_for_file(options.input)
              : options.output_name;

      return output_dir / output_name;
    }

    [[nodiscard]] Error ensure_parent_directory(
        const ::std::filesystem::path &path)
    {
      try
      {
        const auto parent = path.parent_path();

        if (!parent.empty())
        {
          ::std::filesystem::create_directories(parent);
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

    [[nodiscard]] Error write_output_file(
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
              "output file already exists: " + path.string());
        }

        const auto directory_error = ensure_parent_directory(path);
        if (directory_error)
        {
          return directory_error;
        }

        ::std::ofstream file(path, ::std::ios::binary | ::std::ios::trunc);
        if (!file)
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "failed to open output file: " + path.string());
        }

        file << content;

        if (!file)
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

    [[nodiscard]] ::std::string build_banner(
        const BuildCommandOptions &options)
    {
      ::std::ostringstream stream;

      stream << "/* Built by Kordex CLI */\n";
      stream << "/* input: " << options.input << " */\n";

      if (options.minify)
      {
        stream << "/* minify: requested */\n";
      }

      if (options.source_maps)
      {
        stream << "/* sourceMap: requested */\n";
      }

      stream << '\n';

      return stream.str();
    }

    [[nodiscard]] ::std::string source_type_name(
        kordex::runtime::SourceType type)
    {
      return ::std::string(kordex::runtime::to_string(type));
    }

    [[nodiscard]] Result<::std::string> resolve_project_entry(
        const ::std::string &directory)
    {
      const ::std::filesystem::path root(directory);
      const auto manifest_path = project_manifest_path(root);

      if (!::std::filesystem::exists(manifest_path))
      {
        return make_cli_error(
            CliErrorCode::IoError,
            "project manifest not found: " + directory);
      }

      auto manifest = kordex::runtime::Manifest::load(
          manifest_path.string());

      if (!manifest)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            ::std::string(manifest.error().message()));
      }

      if (!manifest.value().has_entry())
      {
        return make_cli_error(
            CliErrorCode::InvalidConfig,
            "project manifest has no entry file");
      }

      return (root / manifest.value().entry).string();
    }

    [[nodiscard]] Result<kordex::runtime::SourceFile> load_build_source(
        const BuildCommandOptions &options)
    {
      if (options.kind == BuildKind::Project)
      {
        auto entry = resolve_project_entry(options.input);
        if (!entry)
        {
          return entry.error();
        }

        return kordex::runtime::SourceFile::load(entry.value());
      }

      return kordex::runtime::SourceFile::load(options.input);
    }

    [[nodiscard]] Error validate_input_exists(
        const BuildCommandOptions &options)
    {
      try
      {
        if (!::std::filesystem::exists(options.input))
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "build input does not exist: " + options.input);
        }

        if (options.kind == BuildKind::SourceFile &&
            !::std::filesystem::is_regular_file(options.input))
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "build input is not a file: " + options.input);
        }

        if (options.kind == BuildKind::Project &&
            !::std::filesystem::is_directory(options.input))
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "build input is not a project directory: " + options.input);
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

      if (arg == "--out-dir")
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

      if (arg == "--out")
      {
        if (index + 1 >= context.args.size())
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "missing value for --out");
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

    if (is_flag(options.input))
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "build input cannot be an option");
    }

    if (options.output_dir.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "build output directory cannot be empty");
    }

    if (!options.output_name.empty() && is_flag(options.output_name))
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "build output name cannot be an option");
    }

    return ok();
  }

  Result<BuildReport> build_input(
      const BuildCommandOptions &options)
  {
    const auto validation = validate_build_options(options);
    if (validation)
    {
      return validation;
    }

    const auto input_error = validate_input_exists(options);
    if (input_error)
    {
      return input_error;
    }

    auto source = load_build_source(options);
    if (!source)
    {
      return make_cli_error(
          CliErrorCode::RuntimeError,
          ::std::string(source.error().message()));
    }

    if (!source.value().executable())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "build input is not an executable JavaScript or TypeScript source");
    }

    BuildReport report;
    report.kind = options.kind;
    report.input = options.input;
    report.entry = source.value().path;
    report.source_type = source.value().type;
    report.source_size = source.value().size();
    report.minify = options.minify;
    report.source_maps = options.source_maps;

    const auto output_path = make_output_path(options);
    report.output = output_path.string();

    ::std::string output_content = build_banner(options);
    output_content += source.value().content;

    const auto write_error = write_output_file(
        output_path,
        output_content,
        options.force);

    if (write_error)
    {
      return write_error;
    }

    report.ok = true;
    report.messages.push_back("build completed");
    report.messages.push_back("output written to " + report.output);

    if (options.minify)
    {
      report.messages.push_back("minification requested but not implemented yet");
    }

    if (options.source_maps)
    {
      report.messages.push_back("source map generation requested but not implemented yet");
    }

    return report;
  }

  ::std::string render_build_report(
      const BuildReport &report,
      const BuildCommandOptions &options)
  {
    ::std::ostringstream stream;

    if (report.ok)
    {
      stream << "Build passed: " << report.input << '\n';
    }
    else
    {
      stream << "Build failed: " << report.input << '\n';
    }

    stream << "output = " << report.output << '\n';

    if (options.details)
    {
      stream << "kind       = " << to_string(report.kind) << '\n';
      stream << "entry      = " << report.entry << '\n';
      stream << "type       = " << source_type_name(report.source_type) << '\n';
      stream << "size       = " << report.source_size << " bytes\n";
      stream << "minify     = " << (report.minify ? "yes" : "no") << '\n';
      stream << "source map = " << (report.source_maps ? "yes" : "no") << '\n';
    }

    for (const auto &message : report.messages)
    {
      stream << "- " << message << '\n';
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

      stream << "Would build " << options.value().input
             << " into " << make_output_path(options.value()).string();

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
    info.summary = "Build a source file or project";
    info.description =
        "Build a JavaScript or TypeScript source file, or a Kordex project directory.";
    info.usage =
        "kordex build <file|project> [--project] [--out-dir dist] [--out name.js]";
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

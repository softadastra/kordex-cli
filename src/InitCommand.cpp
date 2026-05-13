/**
 *
 *  @file InitCommand.cpp
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

#include <kordex/cli/InitCommand.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool is_flag(
        const ::std::string &value) noexcept
    {
      return value.size() > 1 && value.front() == '-';
    }

    [[nodiscard]] ::std::string sanitize_project_name(
        ::std::string value)
    {
      for (auto &character : value)
      {
        if (character == ' ' || character == '\\' || character == '/')
        {
          character = '-';
        }
      }

      return value;
    }

    [[nodiscard]] Error write_file(
        const ::std::filesystem::path &path,
        const ::std::string &content,
        bool force)
    {
      if (::std::filesystem::exists(path) && !force)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            "file already exists: " + path.string());
      }

      ::std::filesystem::create_directories(path.parent_path());

      ::std::ofstream file(path, ::std::ios::binary | ::std::ios::trunc);
      if (!file)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            "failed to open file for writing: " + path.string());
      }

      file << content;

      if (!file)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            "failed to write file: " + path.string());
      }

      return ok();
    }

    [[nodiscard]] ::std::string package_json_content(
        const InitProjectOptions &options)
    {
      ::std::ostringstream stream;

      stream << "{\n";
      stream << "  \"name\": \"" << options.name << "\",\n";
      stream << "  \"version\": \"0.1.0\",\n";
      stream << "  \"type\": \"module\",\n";
      stream << "  \"scripts\": {\n";
      stream << "    \"dev\": \"kordex run\",\n";
      stream << "    \"start\": \"kordex run\",\n";
      stream << "    \"build\": \"kordex build\",\n";
      stream << "    \"check\": \"kordex check src/main.js\"\n";
      stream << "  }\n";
      stream << "}\n";

      return stream.str();
    }

    [[nodiscard]] ::std::string kordex_json_content(
        const InitProjectOptions &options)
    {
      ::std::ostringstream stream;

      stream << "{\n";
      stream << "  \"name\": \"" << options.name << "\",\n";
      stream << "  \"version\": \"0.1.0\",\n";
      stream << "  \"entry\": \"src/main.js\",\n";
      stream << "  \"scripts\": {\n";
      stream << "    \"dev\": \"kordex run\",\n";
      stream << "    \"start\": \"kordex run\",\n";
      stream << "    \"build\": \"kordex build\"\n";
      stream << "  },\n";
      stream << "  \"runtime\": {\n";
      stream << "    \"mode\": \"development\",\n";
      stream << "    \"permissions\": \"strict\"\n";
      stream << "  }\n";
      stream << "}\n";

      return stream.str();
    }

    [[nodiscard]] ::std::string main_js_content(
        const InitProjectOptions &options)
    {
      ::std::ostringstream stream;

      stream << "const appName = \"" << options.name << "\";\n";
      stream << "\n";
      stream << "export default \"Hello from \" + appName;\n";

      return stream.str();
    }

    [[nodiscard]] ::std::string readme_content(
        const InitProjectOptions &options)
    {
      ::std::ostringstream stream;

      stream << "# " << options.name << "\n\n";
      stream << "Kordex application.\n\n";

      stream << "## Run\n\n";
      stream << "```sh\n";
      stream << "kordex run\n";
      stream << "```\n\n";

      stream << "## Build\n\n";
      stream << "```sh\n";
      stream << "kordex build\n";
      stream << "```\n\n";

      stream << "## Run build output\n\n";
      stream << "```sh\n";
      stream << "kordex run dist/main.js\n";
      stream << "```\n";

      return stream.str();
    }

    [[nodiscard]] Error create_project_files(
        const InitProjectOptions &options)
    {
      const ::std::filesystem::path root = options.directory;

      const auto readme_error = write_file(
          root / "README.md",
          readme_content(options),
          options.force);

      if (readme_error)
      {
        return readme_error;
      }

      if (options.package_json)
      {
        const auto package_error = write_file(
            root / "package.json",
            package_json_content(options),
            options.force);

        if (package_error)
        {
          return package_error;
        }
      }

      if (options.kordex_json)
      {
        const auto kordex_error = write_file(
            root / "kordex.json",
            kordex_json_content(options),
            options.force);

        if (kordex_error)
        {
          return kordex_error;
        }
      }

      if (options.main_file)
      {
        const auto main_error = write_file(
            root / "src" / "main.js",
            main_js_content(options),
            options.force);

        if (main_error)
        {
          return main_error;
        }
      }

      return ok();
    }
  } // namespace

  bool InitProjectOptions::has_name() const noexcept
  {
    return !name.empty();
  }

  bool InitProjectOptions::has_directory() const noexcept
  {
    return !directory.empty();
  }

  Result<InitProjectOptions> parse_init_options(
      const CommandContext &context)
  {
    InitProjectOptions options;

    for (::std::size_t index = 0; index < context.args.size(); ++index)
    {
      const auto &arg = context.args[index];

      if (arg == "--force" || arg == "-f")
      {
        options.force = true;
        continue;
      }

      if (arg == "--no-package-json")
      {
        options.package_json = false;
        continue;
      }

      if (arg == "--no-kordex-json")
      {
        options.kordex_json = false;
        continue;
      }

      if (arg == "--no-main")
      {
        options.main_file = false;
        continue;
      }

      if (arg == "--name")
      {
        if (index + 1 >= context.args.size())
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "missing value for --name");
        }

        options.name = context.args[++index];
        continue;
      }

      if (arg == "--dir")
      {
        if (index + 1 >= context.args.size())
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "missing value for --dir");
        }

        options.directory = context.args[++index];
        continue;
      }

      if (is_flag(arg))
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "unknown init option: " + arg);
      }

      if (options.name == "app")
      {
        options.name = arg;
        continue;
      }

      if (options.directory.empty())
      {
        options.directory = arg;
        continue;
      }

      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "unexpected init argument: " + arg);
    }

    options.name = sanitize_project_name(options.name);

    if (options.directory.empty())
    {
      options.directory = options.name;
    }

    const auto validation = validate_init_options(options);
    if (validation)
    {
      return validation;
    }

    return options;
  }

  Error validate_init_options(
      const InitProjectOptions &options)
  {
    if (options.name.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "project name cannot be empty");
    }

    if (options.directory.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "project directory cannot be empty");
    }

    if (is_flag(options.name))
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "project name cannot start with '-'");
    }

    if (!options.package_json &&
        !options.kordex_json &&
        !options.main_file)
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "init command must generate at least one project file");
    }

    return ok();
  }

  CliResult run_init_command(
      const CommandContext &context)
  {
    auto options = parse_init_options(context);
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    if (context.config.dry_run)
    {
      ::std::ostringstream stream;
      stream << "Would create Kordex project '" << options.value().name
             << "' in " << options.value().directory;

      return CliResult::success(stream.str());
    }

    try
    {
      const auto create_error = create_project_files(options.value());
      if (create_error)
      {
        return CliResult::failure(create_error, 1);
      }
    }
    catch (const ::std::filesystem::filesystem_error &exception)
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::IoError,
              exception.what()),
          1);
    }
    catch (const ::std::exception &exception)
    {
      return CliResult::failure(
          make_cli_error(
              CliErrorCode::InternalError,
              exception.what()),
          1);
    }

    ::std::ostringstream stream;
    stream << "Created Kordex project '" << options.value().name
           << "' in " << options.value().directory;

    return CliResult::success(stream.str());
  }

  Result<Command> create_init_command()
  {
    CommandInfo info;
    info.name = "init";
    info.aliases = {"new"};
    info.summary = "Create a new Kordex project";
    info.description =
        "Create a new Kordex project with package.json, kordex.json, and src/main.js.";
    info.usage = "kordex init <name> [--force]";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        ::std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return run_init_command(context);
        });
  }

} // namespace kordex::cli

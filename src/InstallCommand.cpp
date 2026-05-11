/**
 *
 *  @file InstallCommand.cpp
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
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <kordex/cli/InstallCommand.hpp>
#include <kordex/cli/ProjectDiscovery.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool is_flag(
        const std::string &value) noexcept
    {
      return value.size() > 1 && value.front() == '-';
    }

    [[nodiscard]] std::string read_text_file(
        const std::filesystem::path &path)
    {
      std::ifstream input(path, std::ios::binary);
      if (!input)
      {
        return {};
      }

      std::ostringstream stream;
      stream << input.rdbuf();

      return stream.str();
    }

    [[nodiscard]] Error write_text_file(
        const std::filesystem::path &path,
        const std::string &content)
    {
      try
      {
        const auto parent = path.parent_path();
        if (!parent.empty())
        {
          std::filesystem::create_directories(parent);
        }

        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        if (!output)
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "failed to open file for writing: " + path.string());
        }

        output << content;

        if (!output)
        {
          return make_cli_error(
              CliErrorCode::IoError,
              "failed to write file: " + path.string());
        }
      }
      catch (const std::filesystem::filesystem_error &exception)
      {
        return make_cli_error(
            CliErrorCode::IoError,
            exception.what());
      }

      return ok();
    }

    [[nodiscard]] std::string json_escape(
        const std::string &value)
    {
      std::string result;
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

    [[nodiscard]] std::string extract_object_block(
        const std::string &json,
        const std::string &key)
    {
      const std::size_t key_position = json.find("\"" + key + "\"");
      if (key_position == std::string::npos)
      {
        return {};
      }

      const std::size_t colon_position = json.find(':', key_position);
      if (colon_position == std::string::npos)
      {
        return {};
      }

      const std::size_t object_start = json.find('{', colon_position);
      if (object_start == std::string::npos)
      {
        return {};
      }

      int depth = 0;
      bool in_string = false;
      bool escaped = false;

      for (std::size_t index = object_start; index < json.size(); ++index)
      {
        const char character = json[index];

        if (escaped)
        {
          escaped = false;
          continue;
        }

        if (character == '\\')
        {
          escaped = true;
          continue;
        }

        if (character == '"')
        {
          in_string = !in_string;
          continue;
        }

        if (in_string)
        {
          continue;
        }

        if (character == '{')
        {
          ++depth;
          continue;
        }

        if (character == '}')
        {
          --depth;

          if (depth == 0)
          {
            return json.substr(
                object_start + 1,
                index - object_start - 1);
          }
        }
      }

      return {};
    }

    [[nodiscard]] ::std::vector<PackageDependency> extract_dependencies(
        const ::std::string &json,
        const ::std::string &registry_url)
    {
      ::std::vector<PackageDependency> dependencies;

      const ::std::string block = extract_object_block(
          json,
          "dependencies");

      if (block.empty())
      {
        return dependencies;
      }

      const ::std::regex dependency_pattern(
          "\"([^\"]+)\"\\s*:\\s*\"([^\"]+)\"");

      for (::std::sregex_iterator it(
               block.begin(),
               block.end(),
               dependency_pattern),
           end;
           it != end;
           ++it)
      {
        PackageDependency dependency;
        dependency.name = (*it)[1].str();
        dependency.version = (*it)[2].str();
        dependency.source = registry_url;

        dependencies.push_back(::std::move(dependency));
      }

      return dependencies;
    }

    [[nodiscard]] std::string build_lock_json(
        const PackageInstallReport &report)
    {
      std::ostringstream stream;

      stream << "{\n";
      stream << "  \"version\": 1,\n";
      stream << "  \"registry\": \""
             << json_escape(report.registry_url)
             << "\",\n";
      stream << "  \"packages\": {\n";

      for (std::size_t index = 0; index < report.dependencies.size(); ++index)
      {
        const auto &dependency = report.dependencies[index];

        stream << "    \""
               << json_escape(dependency.name)
               << "\": {\n";

        stream << "      \"version\": \""
               << json_escape(dependency.version)
               << "\",\n";

        stream << "      \"source\": \""
               << json_escape(dependency.source)
               << "\"\n";

        stream << "    }";

        if (index + 1 < report.dependencies.size())
        {
          stream << ",";
        }

        stream << "\n";
      }

      stream << "  }\n";
      stream << "}\n";

      return stream.str();
    }

    [[nodiscard]] Result<ProjectManifest> discover_project(
        const std::string &start_directory)
    {
      ProjectDiscoveryOptions options;
      options.start_directory = start_directory.empty()
                                    ? "."
                                    : start_directory;
      options.search_parents = true;

      ProjectDiscovery discovery(options);

      return discovery.discover();
    }

    [[nodiscard]] std::string default_registry_url(
        const std::string &manifest_json)
    {
      auto registry = extract_json_string(
          manifest_json,
          "registry");

      if (!registry.empty())
      {
        return registry;
      }

      return "https://registry.vixcpp.com";
    }

    [[nodiscard]] PackageDependency make_single_dependency(
        const PackageInstallOptions &options,
        const std::string &registry_url)
    {
      PackageDependency dependency;
      dependency.name = options.package;
      dependency.version = options.version.empty()
                               ? "latest"
                               : options.version;
      dependency.source = registry_url;
      return dependency;
    }
  } // namespace

  bool PackageDependency::valid() const noexcept
  {
    return !name.empty() && !version.empty();
  }

  bool PackageInstallOptions::has_package() const noexcept
  {
    return !package.empty();
  }

  bool PackageInstallReport::has_dependencies() const noexcept
  {
    return !dependencies.empty();
  }

  bool PackageInstallReport::has_messages() const noexcept
  {
    return !messages.empty();
  }

  Result<PackageInstallOptions> parse_install_options(
      const CommandContext &context)
  {
    PackageInstallOptions options;
    options.project_root = context.config.working_directory.empty()
                               ? "."
                               : context.config.working_directory;
    options.dry_run = context.config.dry_run;

    for (std::size_t index = 0; index < context.args.size(); ++index)
    {
      const auto &arg = context.args[index];

      if (arg.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "install arguments cannot contain empty entries");
      }

      if (arg == "--registry")
      {
        if (index + 1 >= context.args.size())
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "missing value for --registry");
        }

        options.registry_url = context.args[++index];
        continue;
      }

      if (arg == "--project")
      {
        if (index + 1 >= context.args.size())
        {
          return make_cli_error(
              CliErrorCode::InvalidArgument,
              "missing value for --project");
        }

        options.project_root = context.args[++index];
        continue;
      }

      if (arg == "--no-save")
      {
        options.save = false;
        continue;
      }

      if (arg == "--force" || arg == "-f")
      {
        options.force = true;
        continue;
      }

      if (arg == "--dry-run")
      {
        options.dry_run = true;
        continue;
      }

      if (is_flag(arg))
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "unknown install option: " + arg);
      }

      if (options.package.empty())
      {
        const auto at = arg.find('@');

        if (at != std::string::npos && at > 0)
        {
          options.package = arg.substr(0, at);
          options.version = arg.substr(at + 1);
        }
        else
        {
          options.package = arg;
        }

        continue;
      }

      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "unexpected install argument: " + arg);
    }

    const auto validation = validate_install_options(options);
    if (validation)
    {
      return validation;
    }

    return options;
  }

  Error validate_install_options(
      const PackageInstallOptions &options)
  {
    if (!options.registry_url.empty() &&
        options.registry_url.find("://") == std::string::npos)
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "registry URL must include a scheme, for example https://");
    }

    if (!options.version.empty() &&
        options.version.find(' ') != std::string::npos)
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "package version cannot contain spaces");
    }

    return ok();
  }

  Result<PackageInstallReport> install_packages(
      const PackageInstallOptions &options)
  {
    auto manifest = discover_project(options.project_root);
    if (!manifest)
    {
      return manifest.error();
    }

    const std::string manifest_json = read_text_file(
        manifest.value().path);

    if (manifest_json.empty())
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "manifest is empty or unreadable: " + manifest.value().path);
    }

    PackageInstallReport report;
    report.project_root = manifest.value().root;
    report.manifest_path = manifest.value().path;
    report.lock_path =
        (std::filesystem::path(report.project_root) / "kordex.lock").string();

    report.registry_url = options.registry_url.empty()
                              ? default_registry_url(manifest_json)
                              : options.registry_url;

    if (options.has_package())
    {
      report.dependencies.push_back(
          make_single_dependency(options, report.registry_url));

      report.messages.push_back(
          "resolved package: " +
          report.dependencies.front().name +
          "@" +
          report.dependencies.front().version);
    }
    else
    {
      report.dependencies = extract_dependencies(
          manifest_json,
          report.registry_url);

      report.messages.push_back(
          "resolved dependencies from manifest");
    }

    if (report.dependencies.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "no package or dependencies found to install");
    }

    for (const auto &dependency : report.dependencies)
    {
      if (!dependency.valid())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "invalid dependency entry");
      }
    }

    if (!options.dry_run && options.save)
    {
      const auto write_error = write_text_file(
          report.lock_path,
          build_lock_json(report));

      if (write_error)
      {
        return write_error;
      }

      report.messages.push_back(
          "lock file written: " + report.lock_path);
    }
    else if (options.dry_run)
    {
      report.messages.push_back("dry run, lock file not written");
    }
    else
    {
      report.messages.push_back("no-save enabled, lock file not written");
    }

    report.ok = true;

    return report;
  }

  std::string render_install_report(
      const PackageInstallReport &report)
  {
    std::ostringstream stream;

    stream << "Install completed\n";
    stream << "project  = " << report.project_root << '\n';
    stream << "manifest = " << report.manifest_path << '\n';
    stream << "registry = " << report.registry_url << '\n';

    if (!report.lock_path.empty())
    {
      stream << "lock     = " << report.lock_path << '\n';
    }

    stream << "packages = " << report.dependencies.size();

    for (const auto &dependency : report.dependencies)
    {
      stream << '\n';
      stream << "- " << dependency.name
             << '@'
             << dependency.version;
    }

    if (report.has_messages())
    {
      for (const auto &message : report.messages)
      {
        stream << '\n'
               << message;
      }
    }

    return stream.str();
  }

  CliResult run_install_command(
      const CommandContext &context)
  {
    auto options = parse_install_options(context);
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    auto report = install_packages(options.value());
    if (!report)
    {
      return CliResult::failure(report.error(), 1);
    }

    return CliResult::success(
        render_install_report(report.value()));
  }

  Result<Command> create_install_command()
  {
    CommandInfo info;
    info.name = "install";
    info.aliases = {"i"};
    info.summary = "Install project packages";
    info.description =
        "Resolve project dependencies from kordex.json or package.json and write kordex.lock.";
    info.usage =
        "kordex install [package[@version]] [--registry <url>] [--project <dir>] [--dry-run] [--no-save]";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return run_install_command(context);
        });
  }

} // namespace kordex::cli

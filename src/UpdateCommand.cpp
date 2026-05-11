/**
 *
 *  @file UpdateCommand.cpp
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

#include <sstream>
#include <string>
#include <utility>

#include <kordex/cli/UpdateCommand.hpp>

namespace kordex::cli
{
  namespace
  {
    [[nodiscard]] bool is_flag(
        const std::string &value) noexcept
    {
      return value.size() > 1 && value.front() == '-';
    }
  } // namespace

  bool PackageUpdateOptions::has_package() const noexcept
  {
    return !package.empty();
  }

  bool PackageUpdateReport::has_dependencies() const noexcept
  {
    return !dependencies.empty();
  }

  bool PackageUpdateReport::has_messages() const noexcept
  {
    return !messages.empty();
  }

  Result<PackageUpdateOptions> parse_update_options(
      const CommandContext &context)
  {
    PackageUpdateOptions options;
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
            "update arguments cannot contain empty entries");
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
            "unknown update option: " + arg);
      }

      if (options.package.empty())
      {
        options.package = arg;
        continue;
      }

      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "unexpected update argument: " + arg);
    }

    const auto validation = validate_update_options(options);
    if (validation)
    {
      return validation;
    }

    return options;
  }

  Error validate_update_options(
      const PackageUpdateOptions &options)
  {
    if (!options.registry_url.empty() &&
        options.registry_url.find("://") == std::string::npos)
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "registry URL must include a scheme, for example https://");
    }

    return ok();
  }

  Result<PackageUpdateReport> update_packages(
      const PackageUpdateOptions &options)
  {
    PackageInstallOptions install_options;
    install_options.package = options.package;
    install_options.version = "latest";
    install_options.registry_url = options.registry_url;
    install_options.project_root = options.project_root;
    install_options.save = true;
    install_options.dry_run = options.dry_run;
    install_options.force = options.force;

    auto install_report = install_packages(install_options);
    if (!install_report)
    {
      return install_report.error();
    }

    PackageUpdateReport report;
    report.project_root = install_report.value().project_root;
    report.manifest_path = install_report.value().manifest_path;
    report.lock_path = install_report.value().lock_path;
    report.registry_url = install_report.value().registry_url;
    report.dependencies = install_report.value().dependencies;
    report.messages = install_report.value().messages;
    report.messages.push_back("update resolved latest versions");
    report.ok = true;

    return report;
  }

  std::string render_update_report(
      const PackageUpdateReport &report)
  {
    std::ostringstream stream;

    stream << "Update completed\n";
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

  CliResult run_update_command(
      const CommandContext &context)
  {
    auto options = parse_update_options(context);
    if (!options)
    {
      return CliResult::failure(options.error(), 1);
    }

    auto report = update_packages(options.value());
    if (!report)
    {
      return CliResult::failure(report.error(), 1);
    }

    return CliResult::success(
        render_update_report(report.value()));
  }

  Result<Command> create_update_command()
  {
    CommandInfo info;
    info.name = "update";
    info.aliases = {"up"};
    info.summary = "Update project packages";
    info.description =
        "Refresh project dependency versions and rewrite kordex.lock.";
    info.usage =
        "kordex update [package] [--registry <url>] [--project <dir>] [--dry-run]";
    info.hidden = false;
    info.enabled = true;

    return Command::create(
        std::move(info),
        [](const CommandContext &context) -> CliResult
        {
          return run_update_command(context);
        });
  }

} // namespace kordex::cli

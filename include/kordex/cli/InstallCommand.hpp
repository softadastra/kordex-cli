/**
 *
 *  @file InstallCommand.hpp
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

#ifndef KORDEX_CLI_INSTALL_COMMAND_HPP
#define KORDEX_CLI_INSTALL_COMMAND_HPP

#include <map>
#include <string>
#include <vector>

#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  struct PackageDependency
  {
    std::string name{};
    std::string version{};
    std::string source{};

    [[nodiscard]] bool valid() const noexcept;
  };

  struct PackageInstallOptions
  {
    std::string package{};
    std::string version{};
    std::string registry_url{};
    std::string project_root{"."};

    bool save{true};
    bool dry_run{false};
    bool force{false};

    [[nodiscard]] bool has_package() const noexcept;
  };

  struct PackageInstallReport
  {
    std::string project_root{};
    std::string manifest_path{};
    std::string lock_path{};
    std::string registry_url{};

    std::vector<PackageDependency> dependencies{};
    std::vector<std::string> messages{};

    bool ok{false};

    [[nodiscard]] bool has_dependencies() const noexcept;
    [[nodiscard]] bool has_messages() const noexcept;
  };

  [[nodiscard]] Result<PackageInstallOptions> parse_install_options(
      const CommandContext &context);

  [[nodiscard]] Error validate_install_options(
      const PackageInstallOptions &options);

  [[nodiscard]] Result<PackageInstallReport> install_packages(
      const PackageInstallOptions &options);

  [[nodiscard]] std::string render_install_report(
      const PackageInstallReport &report);

  [[nodiscard]] CliResult run_install_command(
      const CommandContext &context);

  [[nodiscard]] Result<Command> create_install_command();

} // namespace kordex::cli

#endif // KORDEX_CLI_INSTALL_COMMAND_HPP

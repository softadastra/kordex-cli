/**
 *
 *  @file UpdateCommand.hpp
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

#ifndef KORDEX_CLI_UPDATE_COMMAND_HPP
#define KORDEX_CLI_UPDATE_COMMAND_HPP

#include <string>
#include <vector>

#include <kordex/cli/CliResult.hpp>
#include <kordex/cli/Command.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/InstallCommand.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  struct PackageUpdateOptions
  {
    std::string package{};
    std::string registry_url{};
    std::string project_root{"."};

    bool dry_run{false};
    bool force{false};

    [[nodiscard]] bool has_package() const noexcept;
  };

  struct PackageUpdateReport
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

  [[nodiscard]] Result<PackageUpdateOptions> parse_update_options(
      const CommandContext &context);

  [[nodiscard]] Error validate_update_options(
      const PackageUpdateOptions &options);

  [[nodiscard]] Result<PackageUpdateReport> update_packages(
      const PackageUpdateOptions &options);

  [[nodiscard]] std::string render_update_report(
      const PackageUpdateReport &report);

  [[nodiscard]] CliResult run_update_command(
      const CommandContext &context);

  [[nodiscard]] Result<Command> create_update_command();

} // namespace kordex::cli

#endif // KORDEX_CLI_UPDATE_COMMAND_HPP

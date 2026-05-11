/**
 *
 *  @file ProjectDiscovery.hpp
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

#ifndef KORDEX_CLI_PROJECT_DISCOVERY_HPP
#define KORDEX_CLI_PROJECT_DISCOVERY_HPP

#include <map>
#include <string>
#include <vector>

#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  enum class ProjectManifestKind
  {
    None,
    KordexJson,
    PackageJson
  };

  struct ProjectManifest
  {
    ProjectManifestKind kind{ProjectManifestKind::None};

    std::string path{};
    std::string root{};
    std::string name{};
    std::string version{};
    std::string entry{};

    std::map<std::string, std::string> scripts{};

    [[nodiscard]] bool found() const noexcept;
    [[nodiscard]] bool has_entry() const noexcept;
    [[nodiscard]] bool has_scripts() const noexcept;
    [[nodiscard]] bool has_script(const std::string &name) const noexcept;
  };

  struct ProjectDiscoveryOptions
  {
    std::string start_directory{"."};

    bool search_parents{true};
    bool prefer_kordex_json{true};
    bool allow_package_json{true};
  };

  class ProjectDiscovery
  {
  public:
    explicit ProjectDiscovery(
        ProjectDiscoveryOptions options = {});

    [[nodiscard]] const ProjectDiscoveryOptions &options() const noexcept;

    [[nodiscard]] Result<ProjectManifest> discover() const;

    [[nodiscard]] Result<std::string> resolve_entry() const;

    [[nodiscard]] Result<std::string> resolve_script(
        const std::string &name) const;

    [[nodiscard]] static Result<ProjectManifest> load_manifest(
        const std::string &path);

    [[nodiscard]] static Result<ProjectManifest> discover_from(
        const std::string &start_directory);

    [[nodiscard]] static Result<std::string> find_project_root(
        const std::string &start_directory);

    [[nodiscard]] static const char *to_string(
        ProjectManifestKind kind) noexcept;

  private:
    [[nodiscard]] Result<ProjectManifest> discover_in_directory(
        const std::string &directory) const;

    ProjectDiscoveryOptions options_{};
  };

} // namespace kordex::cli

#endif // KORDEX_CLI_PROJECT_DISCOVERY_HPP

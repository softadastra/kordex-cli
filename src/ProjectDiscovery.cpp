/**
 *
 *  @file ProjectDiscovery.cpp
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
#include <regex>
#include <sstream>
#include <string>
#include <utility>

#include <kordex/cli/ProjectDiscovery.hpp>

namespace kordex::cli
{
  namespace
  {
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

    [[nodiscard]] std::string normalize_path(
        const std::filesystem::path &path)
    {
      try
      {
        return std::filesystem::weakly_canonical(path).string();
      }
      catch (...)
      {
        return path.lexically_normal().string();
      }
    }

    [[nodiscard]] bool exists_regular_file(
        const std::filesystem::path &path)
    {
      try
      {
        return std::filesystem::exists(path) &&
               std::filesystem::is_regular_file(path);
      }
      catch (...)
      {
        return false;
      }
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

    [[nodiscard]] ::std::map<::std::string, ::std::string> extract_scripts(
        const ::std::string &json)
    {
      ::std::map<::std::string, ::std::string> scripts;

      const ::std::regex scripts_block_pattern(
          "\"scripts\"\\s*:\\s*\\{([^}]*)\\}");

      ::std::smatch block_match;

      if (!::std::regex_search(json, block_match, scripts_block_pattern) ||
          block_match.size() < 2)
      {
        return scripts;
      }

      const ::std::string block = block_match[1].str();

      const ::std::regex script_pattern(
          "\"([^\"]+)\"\\s*:\\s*\"([^\"]+)\"");

      for (::std::sregex_iterator it(
               block.begin(),
               block.end(),
               script_pattern),
           end;
           it != end;
           ++it)
      {
        scripts[(*it)[1].str()] = (*it)[2].str();
      }

      return scripts;
    }

    [[nodiscard]] ProjectManifestKind detect_manifest_kind(
        const std::filesystem::path &path)
    {
      const auto filename = path.filename().string();

      if (filename == "kordex.json")
      {
        return ProjectManifestKind::KordexJson;
      }

      if (filename == "package.json")
      {
        return ProjectManifestKind::PackageJson;
      }

      return ProjectManifestKind::None;
    }

    [[nodiscard]] std::string resolve_manifest_entry(
        const ProjectManifestKind kind,
        const std::string &json)
    {
      if (kind == ProjectManifestKind::KordexJson)
      {
        auto entry = extract_json_string(json, "entry");
        if (!entry.empty())
        {
          return entry;
        }

        entry = extract_json_string(json, "main");
        if (!entry.empty())
        {
          return entry;
        }
      }

      if (kind == ProjectManifestKind::PackageJson)
      {
        auto entry = extract_json_string(json, "kordex");
        if (!entry.empty())
        {
          return entry;
        }

        entry = extract_json_string(json, "module");
        if (!entry.empty())
        {
          return entry;
        }

        entry = extract_json_string(json, "main");
        if (!entry.empty())
        {
          return entry;
        }
      }

      return {};
    }

    [[nodiscard]] std::string find_default_entry(
        const std::filesystem::path &root)
    {
      const std::filesystem::path candidates[] = {
          root / "src" / "main.ts",
          root / "src" / "main.mts",
          root / "src" / "main.js",
          root / "src" / "index.ts",
          root / "src" / "index.js",
          root / "main.ts",
          root / "main.js",
          root / "index.ts",
          root / "index.js"};

      for (const auto &candidate : candidates)
      {
        if (exists_regular_file(candidate))
        {
          return normalize_path(candidate);
        }
      }

      return {};
    }

    [[nodiscard]] std::filesystem::path absolute_directory(
        const std::string &directory)
    {
      try
      {
        return std::filesystem::absolute(directory).lexically_normal();
      }
      catch (...)
      {
        return std::filesystem::path(directory).lexically_normal();
      }
    }
  } // namespace

  bool ProjectManifest::found() const noexcept
  {
    return kind != ProjectManifestKind::None && !path.empty();
  }

  bool ProjectManifest::has_entry() const noexcept
  {
    return !entry.empty();
  }

  bool ProjectManifest::has_scripts() const noexcept
  {
    return !scripts.empty();
  }

  bool ProjectManifest::has_script(
      const std::string &name) const noexcept
  {
    return scripts.find(name) != scripts.end();
  }

  ProjectDiscovery::ProjectDiscovery(
      ProjectDiscoveryOptions options)
      : options_(std::move(options))
  {
  }

  const ProjectDiscoveryOptions &ProjectDiscovery::options() const noexcept
  {
    return options_;
  }

  Result<ProjectManifest> ProjectDiscovery::discover() const
  {
    std::filesystem::path current = absolute_directory(
        options_.start_directory);

    if (exists_regular_file(current))
    {
      current = current.parent_path();
    }

    while (true)
    {
      auto manifest = discover_in_directory(current.string());
      if (manifest && manifest.value().found())
      {
        return manifest;
      }

      if (!options_.search_parents)
      {
        break;
      }

      const auto parent = current.parent_path();

      if (parent.empty() || parent == current)
      {
        break;
      }

      current = parent;
    }

    return make_cli_error(
        CliErrorCode::IoError,
        "could not find kordex.json or package.json from: " +
            options_.start_directory);
  }

  Result<std::string> ProjectDiscovery::resolve_entry() const
  {
    auto manifest = discover();
    if (!manifest)
    {
      return manifest.error();
    }

    if (!manifest.value().entry.empty())
    {
      const auto entry_path =
          std::filesystem::path(manifest.value().root) /
          manifest.value().entry;

      if (exists_regular_file(entry_path))
      {
        return normalize_path(entry_path);
      }

      return make_cli_error(
          CliErrorCode::IoError,
          "project entry does not exist: " + entry_path.string());
    }

    const auto fallback = find_default_entry(manifest.value().root);
    if (!fallback.empty())
    {
      return fallback;
    }

    return make_cli_error(
        CliErrorCode::IoError,
        "project entry was not found in: " + manifest.value().root);
  }

  Result<std::string> ProjectDiscovery::resolve_script(
      const std::string &name) const
  {
    if (name.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "script name cannot be empty");
    }

    auto manifest = discover();
    if (!manifest)
    {
      return manifest.error();
    }

    const auto found = manifest.value().scripts.find(name);
    if (found == manifest.value().scripts.end())
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "project script was not found: " + name);
    }

    return found->second;
  }

  Result<ProjectManifest> ProjectDiscovery::load_manifest(
      const std::string &path)
  {
    const std::filesystem::path manifest_path(path);

    if (!exists_regular_file(manifest_path))
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "manifest file does not exist: " + path);
    }

    const auto json = read_text_file(manifest_path);
    if (json.empty())
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "manifest file is empty or unreadable: " + path);
    }

    ProjectManifest manifest;
    manifest.kind = detect_manifest_kind(manifest_path);
    manifest.path = normalize_path(manifest_path);
    manifest.root = normalize_path(manifest_path.parent_path());

    if (manifest.kind == ProjectManifestKind::None)
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "unsupported project manifest: " + path);
    }

    manifest.name = extract_json_string(json, "name");
    manifest.version = extract_json_string(json, "version");
    manifest.entry = resolve_manifest_entry(manifest.kind, json);
    manifest.scripts = extract_scripts(json);

    return manifest;
  }

  Result<ProjectManifest> ProjectDiscovery::discover_from(
      const std::string &start_directory)
  {
    ProjectDiscoveryOptions options;
    options.start_directory = start_directory;

    ProjectDiscovery discovery(options);

    return discovery.discover();
  }

  Result<std::string> ProjectDiscovery::find_project_root(
      const std::string &start_directory)
  {
    auto manifest = discover_from(start_directory);
    if (!manifest)
    {
      return manifest.error();
    }

    return manifest.value().root;
  }

  Result<ProjectManifest> ProjectDiscovery::discover_in_directory(
      const std::string &directory) const
  {
    const std::filesystem::path root(directory);

    const auto kordex_json = root / "kordex.json";
    const auto package_json = root / "package.json";

    if (options_.prefer_kordex_json && exists_regular_file(kordex_json))
    {
      return load_manifest(kordex_json.string());
    }

    if (options_.allow_package_json && exists_regular_file(package_json))
    {
      return load_manifest(package_json.string());
    }

    if (!options_.prefer_kordex_json && exists_regular_file(kordex_json))
    {
      return load_manifest(kordex_json.string());
    }

    ProjectManifest manifest;
    manifest.root = normalize_path(root);

    return manifest;
  }

  const char *ProjectDiscovery::to_string(
      ProjectManifestKind kind) noexcept
  {
    switch (kind)
    {
    case ProjectManifestKind::None:
      return "none";
    case ProjectManifestKind::KordexJson:
      return "kordex_json";
    case ProjectManifestKind::PackageJson:
      return "package_json";
    }

    return "none";
  }

} // namespace kordex::cli

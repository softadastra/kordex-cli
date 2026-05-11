/**
 *
 *  @file PluginCommandRegistry.hpp
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

#ifndef KORDEX_CLI_PLUGIN_COMMAND_REGISTRY_HPP
#define KORDEX_CLI_PLUGIN_COMMAND_REGISTRY_HPP

#include <string>
#include <vector>

#include <kordex/cli/Command.hpp>
#include <kordex/cli/CommandRegistry.hpp>
#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  struct PluginPermissions
  {
    bool allow_fs{false};
    bool allow_env{false};
    bool allow_net{false};
    bool allow_process{false};
  };

  struct PluginCommandDefinition
  {
    std::string name{};
    std::string summary{};
    std::string description{};
    std::string usage{};
    std::string run{};

    std::vector<std::string> aliases{};
    PluginPermissions permissions{};

    bool enabled{true};
    bool hidden{false};

    [[nodiscard]] bool has_name() const noexcept;
    [[nodiscard]] bool has_run() const noexcept;
  };

  struct PluginCommandLoadOptions
  {
    std::string start_directory{"."};

    bool search_parents{true};
    bool allow_disabled{false};
  };

  class PluginCommandRegistry
  {
  public:
    PluginCommandRegistry() = default;

    [[nodiscard]] Error register_plugin_command(
        PluginCommandDefinition definition);

    [[nodiscard]] Error register_into(
        CommandRegistry &registry) const;

    void clear();

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

    [[nodiscard]] const std::vector<PluginCommandDefinition> &commands() const noexcept;

    [[nodiscard]] static Result<PluginCommandRegistry> load_from_project(
        const PluginCommandLoadOptions &options = {});

    [[nodiscard]] static Result<std::vector<PluginCommandDefinition>> load_definitions_from_manifest(
        const std::string &manifest_path);

  private:
    [[nodiscard]] static Result<Command> create_command(
        const PluginCommandDefinition &definition);

    [[nodiscard]] static Error validate_definition(
        const PluginCommandDefinition &definition);

    std::vector<PluginCommandDefinition> commands_{};
  };

} // namespace kordex::cli

#endif // KORDEX_CLI_PLUGIN_COMMAND_REGISTRY_HPP

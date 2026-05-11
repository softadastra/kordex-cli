/**
 *
 *  @file PluginCommandRegistry.cpp
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
#include <map>

#include <kordex/cli/PluginCommandRegistry.hpp>
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

    [[nodiscard]] bool extract_json_bool(
        const std::string &json,
        const std::string &key,
        bool fallback = false)
    {
      const std::regex pattern(
          "\"" + key + R"("\s*:\s*(true|false))");

      std::smatch match;

      if (std::regex_search(json, match, pattern) &&
          match.size() >= 2)
      {
        return match[1].str() == "true";
      }

      return fallback;
    }

    [[nodiscard]] ::std::vector<::std::string> extract_json_string_array(
        const ::std::string &json,
        const ::std::string &key)
    {
      ::std::vector<::std::string> values;

      const ::std::regex array_pattern(
          "\"" + key + "\"\\s*:\\s*\\[([^\\]]*)\\]");

      ::std::smatch array_match;

      if (!::std::regex_search(json, array_match, array_pattern) ||
          array_match.size() < 2)
      {
        return values;
      }

      const ::std::string block = array_match[1].str();

      const ::std::regex value_pattern(
          "\"([^\"]+)\"");

      for (::std::sregex_iterator it(
               block.begin(),
               block.end(),
               value_pattern),
           end;
           it != end;
           ++it)
      {
        values.push_back((*it)[1].str());
      }

      return values;
    }

    [[nodiscard]] std::string extract_plugins_commands_block(
        const std::string &json)
    {
      const std::size_t plugins_pos = json.find("\"plugins\"");
      if (plugins_pos == std::string::npos)
      {
        return {};
      }

      const std::size_t commands_pos = json.find("\"commands\"", plugins_pos);
      if (commands_pos == std::string::npos)
      {
        return {};
      }

      const std::size_t array_start = json.find('[', commands_pos);
      if (array_start == std::string::npos)
      {
        return {};
      }

      int depth = 0;
      bool in_string = false;
      bool escaped = false;

      for (std::size_t index = array_start; index < json.size(); ++index)
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

        if (character == '[')
        {
          ++depth;
          continue;
        }

        if (character == ']')
        {
          --depth;

          if (depth == 0)
          {
            return json.substr(
                array_start + 1,
                index - array_start - 1);
          }
        }
      }

      return {};
    }

    [[nodiscard]] std::vector<std::string> split_top_level_objects(
        const std::string &array_content)
    {
      std::vector<std::string> objects;

      int depth = 0;
      bool in_string = false;
      bool escaped = false;
      std::size_t object_start = std::string::npos;

      for (std::size_t index = 0; index < array_content.size(); ++index)
      {
        const char character = array_content[index];

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
          if (depth == 0)
          {
            object_start = index;
          }

          ++depth;
          continue;
        }

        if (character == '}')
        {
          --depth;

          if (depth == 0 && object_start != std::string::npos)
          {
            objects.push_back(
                array_content.substr(
                    object_start,
                    index - object_start + 1));

            object_start = std::string::npos;
          }
        }
      }

      return objects;
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

    [[nodiscard]] PluginPermissions parse_permissions(
        const std::string &object)
    {
      PluginPermissions permissions;

      const std::string block = extract_object_block(
          object,
          "permissions");

      if (block.empty())
      {
        return permissions;
      }

      permissions.allow_fs = extract_json_bool(block, "fs", false);
      permissions.allow_env = extract_json_bool(block, "env", false);
      permissions.allow_net = extract_json_bool(block, "net", false);
      permissions.allow_process = extract_json_bool(block, "process", false);

      return permissions;
    }

    [[nodiscard]] PluginCommandDefinition parse_plugin_definition(
        const std::string &object)
    {
      PluginCommandDefinition definition;

      definition.name = extract_json_string(object, "name");
      definition.summary = extract_json_string(object, "summary");
      definition.description = extract_json_string(object, "description");
      definition.usage = extract_json_string(object, "usage");
      definition.run = extract_json_string(object, "run");
      definition.aliases = extract_json_string_array(object, "aliases");
      definition.permissions = parse_permissions(object);
      definition.enabled = extract_json_bool(object, "enabled", true);
      definition.hidden = extract_json_bool(object, "hidden", false);

      return definition;
    }

    [[nodiscard]] bool contains_alias(
        const std::vector<std::string> &aliases,
        const std::string &value)
    {
      return std::find(
                 aliases.begin(),
                 aliases.end(),
                 value) != aliases.end();
    }

    [[nodiscard]] std::string permissions_summary(
        const PluginPermissions &permissions)
    {
      std::ostringstream stream;

      stream << "permissions: ";
      stream << "fs=" << (permissions.allow_fs ? "allow" : "deny");
      stream << ", env=" << (permissions.allow_env ? "allow" : "deny");
      stream << ", net=" << (permissions.allow_net ? "allow" : "deny");
      stream << ", process=" << (permissions.allow_process ? "allow" : "deny");

      return stream.str();
    }
  } // namespace

  bool PluginCommandDefinition::has_name() const noexcept
  {
    return !name.empty();
  }

  bool PluginCommandDefinition::has_run() const noexcept
  {
    return !run.empty();
  }

  Error PluginCommandRegistry::register_plugin_command(
      PluginCommandDefinition definition)
  {
    const auto validation = validate_definition(definition);
    if (validation)
    {
      return validation;
    }

    for (const auto &existing : commands_)
    {
      if (existing.name == definition.name ||
          contains_alias(existing.aliases, definition.name))
      {
        return make_cli_error(
            CliErrorCode::CommandAlreadyExists,
            "plugin command already exists: " + definition.name);
      }

      for (const auto &alias : definition.aliases)
      {
        if (existing.name == alias ||
            contains_alias(existing.aliases, alias))
        {
          return make_cli_error(
              CliErrorCode::CommandAlreadyExists,
              "plugin command alias already exists: " + alias);
        }
      }
    }

    commands_.push_back(std::move(definition));

    return ok();
  }

  Error PluginCommandRegistry::register_into(
      CommandRegistry &registry) const
  {
    for (const auto &definition : commands_)
    {
      if (!definition.enabled)
      {
        continue;
      }

      auto command = create_command(definition);
      if (!command)
      {
        return command.error();
      }

      const auto error = registry.register_command(command.value());
      if (error)
      {
        return error;
      }
    }

    return ok();
  }

  void PluginCommandRegistry::clear()
  {
    commands_.clear();
  }

  bool PluginCommandRegistry::empty() const noexcept
  {
    return commands_.empty();
  }

  std::size_t PluginCommandRegistry::size() const noexcept
  {
    return commands_.size();
  }

  const std::vector<PluginCommandDefinition> &PluginCommandRegistry::commands() const noexcept
  {
    return commands_;
  }

  Result<PluginCommandRegistry> PluginCommandRegistry::load_from_project(
      const PluginCommandLoadOptions &options)
  {
    ProjectDiscoveryOptions discovery_options;
    discovery_options.start_directory = options.start_directory;
    discovery_options.search_parents = options.search_parents;

    ProjectDiscovery discovery(discovery_options);

    auto manifest = discovery.discover();
    if (!manifest)
    {
      return make_cli_error(
          CliErrorCode::IoError,
          std::string(manifest.error().message()));
    }

    auto definitions = load_definitions_from_manifest(
        manifest.value().path);

    if (!definitions)
    {
      return definitions.error();
    }

    PluginCommandRegistry registry;

    for (auto &definition : definitions.value())
    {
      if (!definition.enabled && !options.allow_disabled)
      {
        continue;
      }

      const auto error = registry.register_plugin_command(
          std::move(definition));

      if (error)
      {
        return error;
      }
    }

    return registry;
  }

  Result<std::vector<PluginCommandDefinition>> PluginCommandRegistry::load_definitions_from_manifest(
      const std::string &manifest_path)
  {
    const std::string json = read_text_file(manifest_path);
    if (json.empty())
    {
      return make_cli_error(
          CliErrorCode::IoError,
          "manifest is empty or unreadable: " + manifest_path);
    }

    const std::string commands_block = extract_plugins_commands_block(json);

    std::vector<PluginCommandDefinition> definitions;

    if (commands_block.empty())
    {
      return definitions;
    }

    for (const auto &object : split_top_level_objects(commands_block))
    {
      definitions.push_back(
          parse_plugin_definition(object));
    }

    return definitions;
  }

  Result<Command> PluginCommandRegistry::create_command(
      const PluginCommandDefinition &definition)
  {
    const auto validation = validate_definition(definition);
    if (validation)
    {
      return validation;
    }

    CommandInfo info;
    info.name = definition.name;
    info.aliases = definition.aliases;
    info.summary = definition.summary.empty()
                       ? "Run plugin command"
                       : definition.summary;
    info.description = definition.description.empty()
                           ? "Project plugin command declared in kordex.json or package.json."
                           : definition.description;
    info.usage = definition.usage.empty()
                     ? "kordex " + definition.name
                     : definition.usage;
    info.hidden = definition.hidden;
    info.enabled = definition.enabled;

    return Command::create(
        std::move(info),
        [definition](const CommandContext &context) -> CliResult
        {
          (void)context;

          std::ostringstream stream;

          stream << "Resolved plugin command `" << definition.name << "`\n";
          stream << "run = " << definition.run << '\n';
          stream << permissions_summary(definition.permissions);

          return CliResult::success(stream.str());
        });
  }

  Error PluginCommandRegistry::validate_definition(
      const PluginCommandDefinition &definition)
  {
    if (definition.name.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "plugin command name cannot be empty");
    }

    if (definition.run.empty())
    {
      return make_cli_error(
          CliErrorCode::InvalidArgument,
          "plugin command run value cannot be empty: " + definition.name);
    }

    if (definition.name == "help" ||
        definition.name == "run" ||
        definition.name == "build" ||
        definition.name == "check" ||
        definition.name == "repl" ||
        definition.name == "init" ||
        definition.name == "version")
    {
      return make_cli_error(
          CliErrorCode::CommandAlreadyExists,
          "plugin command cannot override builtin command: " + definition.name);
    }

    for (const auto &alias : definition.aliases)
    {
      if (alias.empty())
      {
        return make_cli_error(
            CliErrorCode::InvalidArgument,
            "plugin command alias cannot be empty: " + definition.name);
      }
    }

    return ok();
  }

} // namespace kordex::cli

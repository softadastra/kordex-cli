/**
 *
 *  @file PermissionBridge.hpp
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

#ifndef KORDEX_CLI_PERMISSION_BRIDGE_HPP
#define KORDEX_CLI_PERMISSION_BRIDGE_HPP

#include <kordex/bindings/Engine.hpp>

#include <kordex/runtime/RuntimeConfig.hpp>
#include <kordex/runtime/RuntimeOptions.hpp>

#include <kordex/std/StdOptions.hpp>

#include <kordex/cli/Error.hpp>
#include <kordex/cli/Result.hpp>

namespace kordex::cli
{
  /**
   * @brief Convert final runtime config to standard module options.
   *
   * This is the central permission bridge:
   * - runtime.allow_fs      -> std.enable_fs
   * - runtime.allow_env     -> std.enable_env
   * - runtime.allow_process -> std.enable_process
   * - runtime.allow_net     -> std.enable_http
   */
  [[nodiscard]] kordex::standard::StdOptions std_options_from_runtime_config(
      const kordex::runtime::RuntimeConfig &config,
      kordex::standard::StdOptions base = kordex::standard::StdOptions::production());

  /**
   * @brief Build StdOptions from RuntimeOptions.
   */
  [[nodiscard]] Result<kordex::standard::StdOptions> std_options_from_runtime_options(
      const kordex::runtime::RuntimeOptions &options,
      kordex::standard::StdOptions base = kordex::standard::StdOptions::production());

  /**
   * @brief Install standard modules according to runtime permissions.
   */
  [[nodiscard]] Error install_std_modules_for_runtime(
      kordex::bindings::Engine &engine,
      const kordex::runtime::RuntimeOptions &options,
      kordex::standard::StdOptions base = kordex::standard::StdOptions::production());

} // namespace kordex::cli

#endif // KORDEX_CLI_PERMISSION_BRIDGE_HPP

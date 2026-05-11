/**
 *
 *  @file PermissionBridge.cpp
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

#include <string>
#include <utility>

#include <kordex/std/Std.hpp>

#include <kordex/cli/PermissionBridge.hpp>

namespace kordex::cli
{
  kordex::standard::StdOptions std_options_from_runtime_config(
      const kordex::runtime::RuntimeConfig &config,
      kordex::standard::StdOptions base)
  {
    base.enabled = true;

    /*
     * Always keep safe utility modules enabled.
     * Capability modules are controlled by RuntimeConfig permissions below.
     */
    base.enable_console = true;
    base.enable_path = true;
    base.enable_timer = true;
    base.enable_crypto = true;

    /*
     * Permission-gated modules.
     */
    base.enable_fs = config.allow_fs;
    base.enable_env = config.allow_env;
    base.enable_process = config.allow_process;
    base.enable_http = config.allow_net;

    /*
     * Native module/function creation must remain aligned with the runtime
     * policy. Strict mode does not disable std; it only means capabilities
     * must be explicitly enabled.
     */
    base.allow_native_modules = true;
    base.allow_native_functions = true;

    return base;
  }

  Result<kordex::standard::StdOptions> std_options_from_runtime_options(
      const kordex::runtime::RuntimeOptions &options,
      kordex::standard::StdOptions base)
  {
    auto config = kordex::runtime::RuntimeConfig::from_options(options);
    if (!config)
    {
      return make_cli_error(
          CliErrorCode::RuntimeError,
          std::string(config.error().message()));
    }

    return std_options_from_runtime_config(
        config.value(),
        std::move(base));
  }

  Error install_std_modules_for_runtime(
      kordex::bindings::Engine &engine,
      const kordex::runtime::RuntimeOptions &options,
      kordex::standard::StdOptions base)
  {
    auto std_options = std_options_from_runtime_options(
        options,
        std::move(base));

    if (!std_options)
    {
      return std_options.error();
    }

    const auto std_error = kordex::standard::install(
        engine,
        std_options.value());

    if (std_error)
    {
      return make_cli_error(
          CliErrorCode::StdError,
          std::string(std_error.message()));
    }

    return ok();
  }

} // namespace kordex::cli

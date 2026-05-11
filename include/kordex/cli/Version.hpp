/**
 *
 *  @file Version.hpp
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

#ifndef KORDEX_CLI_VERSION_HPP
#define KORDEX_CLI_VERSION_HPP

#include <string_view>

namespace kordex::cli
{
  /**
   * @brief Kordex CLI semantic version numbers.
   */
  inline constexpr int version_major_value = 0;
  inline constexpr int version_minor_value = 1;
  inline constexpr int version_patch_value = 0;

  /**
   * @brief Kordex CLI version as a stable string.
   */
  inline constexpr std::string_view version_string = "0.1.0";

  /**
   * @brief Return the Kordex CLI major version.
   */
  [[nodiscard]] constexpr int version_major() noexcept
  {
    return version_major_value;
  }

  /**
   * @brief Return the Kordex CLI minor version.
   */
  [[nodiscard]] constexpr int version_minor() noexcept
  {
    return version_minor_value;
  }

  /**
   * @brief Return the Kordex CLI patch version.
   */
  [[nodiscard]] constexpr int version_patch() noexcept
  {
    return version_patch_value;
  }

  /**
   * @brief Return the Kordex CLI version string.
   */
  [[nodiscard]] constexpr std::string_view version() noexcept
  {
    return version_string;
  }

} // namespace kordex::cli

#endif // KORDEX_CLI_VERSION_HPP

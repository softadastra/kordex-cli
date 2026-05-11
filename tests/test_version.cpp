/**
 *
 *  @file test_version.cpp
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

#include <string_view>

#include <kordex/cli/Version.hpp>

namespace
{
  [[nodiscard]] bool expect_true(
      bool condition,
      const char *message)
  {
    if (!condition)
    {
      (void)message;
      return false;
    }

    return true;
  }

  [[nodiscard]] bool test_version_numbers()
  {
    return expect_true(
               kordex::cli::version_major() == 0,
               "major version should be 0") &&
           expect_true(
               kordex::cli::version_minor() == 1,
               "minor version should be 1") &&
           expect_true(
               kordex::cli::version_patch() == 0,
               "patch version should be 0");
  }

  [[nodiscard]] bool test_version_string()
  {
    constexpr std::string_view expected = "0.1.0";

    return expect_true(
               kordex::cli::version() == expected,
               "version string should be 0.1.0") &&
           expect_true(
               kordex::cli::version_string == expected,
               "version constant should be 0.1.0");
  }

  [[nodiscard]] bool test_version_constants()
  {
    return expect_true(
               kordex::cli::version_major_value ==
                   kordex::cli::version_major(),
               "major constant should match version_major") &&
           expect_true(
               kordex::cli::version_minor_value ==
                   kordex::cli::version_minor(),
               "minor constant should match version_minor") &&
           expect_true(
               kordex::cli::version_patch_value ==
                   kordex::cli::version_patch(),
               "patch constant should match version_patch");
  }
} // namespace

int main()
{
  const bool ok =
      test_version_numbers() &&
      test_version_string() &&
      test_version_constants();

  return ok ? 0 : 1;
}

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
    return expect_true(
        kordex::cli::version() == std::string_view("0.1.0"),
        "version string should be 0.1.0");
  }

  [[nodiscard]] bool test_version_constants()
  {
    return expect_true(
               kordex::cli::KORDEX_CLI_VERSION_MAJOR_VALUE == 0,
               "major version constant should be 0") &&
           expect_true(
               kordex::cli::KORDEX_CLI_VERSION_MINOR_VALUE == 1,
               "minor version constant should be 1") &&
           expect_true(
               kordex::cli::KORDEX_CLI_VERSION_PATCH_VALUE == 0,
               "patch version constant should be 0") &&
           expect_true(
               kordex::cli::KORDEX_CLI_VERSION == std::string_view("0.1.0"),
               "version constant should be 0.1.0");
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

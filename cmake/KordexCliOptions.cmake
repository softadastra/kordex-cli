#  @file KordexCliOptions.cmake
#  @author Softadastra
#
#  Copyright 2026, Softadastra. All rights reserved.
#  https://github.com/softadastra/kordex-cli
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex CLI - Build Options
# ====================================================================

# ifndef(KORDEX_CLI_OPTIONS_INCLUDED)
set(KORDEX_CLI_OPTIONS_INCLUDED ON)

# --------------------------------------------------------------------
# Build options
# --------------------------------------------------------------------
option(KORDEX_CLI_BUILD_APP "Build the Kordex CLI executable" ON)
option(KORDEX_CLI_BUILD_TESTS "Build Kordex CLI tests" OFF)
option(KORDEX_CLI_BUILD_EXAMPLES "Build Kordex CLI examples" OFF)

# --------------------------------------------------------------------
# Developer options
# --------------------------------------------------------------------
option(KORDEX_CLI_ENABLE_WARNINGS "Enable compiler warnings for Kordex CLI" ON)
option(KORDEX_CLI_ENABLE_SANITIZERS "Enable sanitizers for Kordex CLI" OFF)

# --------------------------------------------------------------------
# Command options
# --------------------------------------------------------------------
option(KORDEX_CLI_ENABLE_INIT_COMMAND "Enable kordex init command" ON)
option(KORDEX_CLI_ENABLE_RUN_COMMAND "Enable kordex run command" ON)
option(KORDEX_CLI_ENABLE_CHECK_COMMAND "Enable kordex check command" ON)
option(KORDEX_CLI_ENABLE_BUILD_COMMAND "Enable kordex build command" ON)
option(KORDEX_CLI_ENABLE_REPL_COMMAND "Enable kordex repl command" ON)
option(KORDEX_CLI_ENABLE_VERSION_COMMAND "Enable kordex version command" ON)

# --------------------------------------------------------------------
# Dependency version policy
# --------------------------------------------------------------------
set(KORDEX_VIX_GIT_TAG
    "main"
    CACHE STRING
    "Git tag or branch used for Vix dependencies")

set(KORDEX_RUNTIME_GIT_TAG
    "main"
    CACHE STRING
    "Git tag or branch used for kordex runtime")

set(KORDEX_BINDINGS_GIT_TAG
    "main"
    CACHE STRING
    "Git tag or branch used for kordex bindings")

set(KORDEX_STD_GIT_TAG
    "main"
    CACHE STRING
    "Git tag or branch used for kordex std")

# --------------------------------------------------------------------
# Dependency fetch options
# --------------------------------------------------------------------
option(KORDEX_CLI_FETCH_RUNTIME "Auto-fetch kordex::runtime if missing" ON)
option(KORDEX_CLI_FETCH_BINDINGS "Auto-fetch kordex::bindings if missing" ON)
option(KORDEX_CLI_FETCH_STD "Auto-fetch kordex::std if missing" ON)

option(KORDEX_CLI_FETCH_ERROR "Auto-fetch vix::error if missing" ON)
option(KORDEX_CLI_FETCH_LOG "Auto-fetch vix::log if missing" ON)
option(KORDEX_CLI_FETCH_JSON "Auto-fetch vix::json if missing" ON)
option(KORDEX_CLI_FETCH_FS "Auto-fetch vix::fs if missing" ON)
option(KORDEX_CLI_FETCH_PATH "Auto-fetch vix::path if missing" ON)
option(KORDEX_CLI_FETCH_ENV "Auto-fetch vix::env if missing" ON)
option(KORDEX_CLI_FETCH_PROCESS "Auto-fetch vix::process if missing" ON)
option(KORDEX_CLI_FETCH_TIME "Auto-fetch vix::time if missing" ON)
option(KORDEX_CLI_FETCH_TESTS "Auto-fetch vix::tests if missing" ON)

# --------------------------------------------------------------------
# Umbrella build policy
# --------------------------------------------------------------------
# When Kordex CLI is built inside the kordex umbrella repository,
# dependencies should be provided by the umbrella build.
#
# In that mode, CLI must not fetch dependencies by itself.
# The root project is responsible for add_subdirectory order.
# --------------------------------------------------------------------
if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
  set(KORDEX_CLI_FETCH_RUNTIME OFF CACHE BOOL "Auto-fetch kordex::runtime if missing" FORCE)
  set(KORDEX_CLI_FETCH_BINDINGS OFF CACHE BOOL "Auto-fetch kordex::bindings if missing" FORCE)
  set(KORDEX_CLI_FETCH_STD OFF CACHE BOOL "Auto-fetch kordex::std if missing" FORCE)

  set(KORDEX_CLI_FETCH_ERROR OFF CACHE BOOL "Auto-fetch vix::error if missing" FORCE)
  set(KORDEX_CLI_FETCH_LOG OFF CACHE BOOL "Auto-fetch vix::log if missing" FORCE)
  set(KORDEX_CLI_FETCH_JSON OFF CACHE BOOL "Auto-fetch vix::json if missing" FORCE)
  set(KORDEX_CLI_FETCH_FS OFF CACHE BOOL "Auto-fetch vix::fs if missing" FORCE)
  set(KORDEX_CLI_FETCH_PATH OFF CACHE BOOL "Auto-fetch vix::path if missing" FORCE)
  set(KORDEX_CLI_FETCH_ENV OFF CACHE BOOL "Auto-fetch vix::env if missing" FORCE)
  set(KORDEX_CLI_FETCH_PROCESS OFF CACHE BOOL "Auto-fetch vix::process if missing" FORCE)
  set(KORDEX_CLI_FETCH_TIME OFF CACHE BOOL "Auto-fetch vix::time if missing" FORCE)
  set(KORDEX_CLI_FETCH_TESTS OFF CACHE BOOL "Auto-fetch vix::tests if missing" FORCE)
endif()

# endif()

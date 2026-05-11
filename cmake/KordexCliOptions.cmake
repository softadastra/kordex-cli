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

include_guard(GLOBAL)

set(KORDEX_CLI_OPTIONS_INCLUDED ON)

# --------------------------------------------------------------------
# Build options
# --------------------------------------------------------------------
option(KORDEX_CLI_BUILD_APP "Build the Kordex CLI executable" ON)
option(KORDEX_CLI_BUILD_TESTS "Build Kordex CLI tests" OFF)
option(KORDEX_CLI_BUILD_EXAMPLES "Build Kordex CLI examples" OFF)

# --------------------------------------------------------------------
# Install options
# --------------------------------------------------------------------
option(KORDEX_CLI_ENABLE_INSTALL
    "Generate install/export rules for Kordex CLI"
    OFF)

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
    "Git tag or branch used for Kordex Runtime")

set(KORDEX_BINDINGS_GIT_TAG
    "main"
    CACHE STRING
    "Git tag or branch used for Kordex Bindings")

set(KORDEX_STD_GIT_TAG
    "main"
    CACHE STRING
    "Git tag or branch used for Kordex Std")

# --------------------------------------------------------------------
# Dependency fetch policy
# --------------------------------------------------------------------
option(KORDEX_CLI_FETCH_KORDEX_DEPS "Auto-fetch missing Kordex dependencies" ON)
option(KORDEX_CLI_FETCH_VIX_DEPS "Auto-fetch missing Vix dependencies" ON)
option(KORDEX_CLI_FETCH_TESTS "Auto-fetch vix::tests if missing" ON)

set(KORDEX_CLI_FETCH_RUNTIME
    ${KORDEX_CLI_FETCH_KORDEX_DEPS}
    CACHE BOOL
    "Auto-fetch kordex::runtime if missing")

set(KORDEX_CLI_FETCH_BINDINGS
    ${KORDEX_CLI_FETCH_KORDEX_DEPS}
    CACHE BOOL
    "Auto-fetch kordex::bindings if missing")

set(KORDEX_CLI_FETCH_STD
    ${KORDEX_CLI_FETCH_KORDEX_DEPS}
    CACHE BOOL
    "Auto-fetch kordex::std if missing")

set(KORDEX_CLI_FETCH_ERROR
    ${KORDEX_CLI_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::error if missing")

set(KORDEX_CLI_FETCH_LOG
    ${KORDEX_CLI_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::log if missing")

set(KORDEX_CLI_FETCH_JSON
    ${KORDEX_CLI_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::json if missing")

set(KORDEX_CLI_FETCH_FS
    ${KORDEX_CLI_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::fs if missing")

set(KORDEX_CLI_FETCH_PATH
    ${KORDEX_CLI_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::path if missing")

set(KORDEX_CLI_FETCH_ENV
    ${KORDEX_CLI_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::env if missing")

set(KORDEX_CLI_FETCH_PROCESS
    ${KORDEX_CLI_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::process if missing")

set(KORDEX_CLI_FETCH_TIME
    ${KORDEX_CLI_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::time if missing")

# --------------------------------------------------------------------
# Umbrella build policy
# --------------------------------------------------------------------
if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
  set(KORDEX_CLI_FETCH_KORDEX_DEPS OFF CACHE BOOL "Auto-fetch missing Kordex dependencies" FORCE)
  set(KORDEX_CLI_FETCH_VIX_DEPS OFF CACHE BOOL "Auto-fetch missing Vix dependencies" FORCE)
  set(KORDEX_CLI_FETCH_TESTS OFF CACHE BOOL "Auto-fetch vix::tests if missing" FORCE)

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
endif()

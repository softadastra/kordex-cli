#  @file CMakeLists.txt
#  @author Softadastra
#
#  Copyright 2026, Softadastra. All rights reserved.
#  https://github.com/softadastra/kordex-cli
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex CLI
# ====================================================================

cmake_minimum_required(VERSION 3.24)

project(kordex_cli
    VERSION 0.1.0
    DESCRIPTION "Official command line interface for Kordex"
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

include(cmake/KordexCliOptions.cmake)

# --------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------
function(kordex_cli_fetch_dependency dependency_name repository tag)
  FetchContent_Declare(
      ${dependency_name}
      GIT_REPOSITORY ${repository}
      GIT_TAG ${tag}
      GIT_SHALLOW TRUE)

  FetchContent_MakeAvailable(${dependency_name})
endfunction()

function(kordex_cli_require_target target_name package_name dependency_name fetch_option repository tag)
  if(TARGET ${target_name})
    return()
  endif()

  find_package(${package_name} CONFIG QUIET)

  if(TARGET ${target_name})
    return()
  endif()

  if(${fetch_option})
    kordex_cli_fetch_dependency(
        ${dependency_name}
        ${repository}
        ${tag})

    if(TARGET ${target_name})
      return()
    endif()
  endif()

  message(FATAL_ERROR
      "Required dependency target '${target_name}' was not found. "
      "Install package '${package_name}' or enable ${fetch_option}.")
endfunction()

# --------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------
kordex_cli_require_target(
    kordex::runtime
    kordex_runtime
    kordex_runtime
    KORDEX_CLI_FETCH_RUNTIME
    https://github.com/kordexjs/runtime.git
    ${KORDEX_RUNTIME_GIT_TAG})

kordex_cli_require_target(
    kordex::bindings
    kordex_bindings
    kordex_bindings
    KORDEX_CLI_FETCH_BINDINGS
    https://github.com/kordexjs/bindings.git
    ${KORDEX_BINDINGS_GIT_TAG})

kordex_cli_require_target(
    kordex::std
    kordex_std
    kordex_std
    KORDEX_CLI_FETCH_STD
    https://github.com/kordexjs/std.git
    ${KORDEX_STD_GIT_TAG})

kordex_cli_require_target(
    vix::error
    vix_error
    vix_error
    KORDEX_CLI_FETCH_ERROR
    https://github.com/vixcpp/error.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::log
    vix_log
    vix_log
    KORDEX_CLI_FETCH_LOG
    https://github.com/vixcpp/log.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::json
    vix_json
    vix_json
    KORDEX_CLI_FETCH_JSON
    https://github.com/vixcpp/json.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::fs
    vix_fs
    vix_fs
    KORDEX_CLI_FETCH_FS
    https://github.com/vixcpp/fs.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::path
    vix_path
    vix_path
    KORDEX_CLI_FETCH_PATH
    https://github.com/vixcpp/path.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::env
    vix_env
    vix_env
    KORDEX_CLI_FETCH_ENV
    https://github.com/vixcpp/env.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::process
    vix_process
    vix_process
    KORDEX_CLI_FETCH_PROCESS
    https://github.com/vixcpp/process.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::time
    vix_time
    vix_time
    KORDEX_CLI_FETCH_TIME
    https://github.com/vixcpp/time.git
    ${KORDEX_VIX_GIT_TAG})

# --------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------
set(KORDEX_CLI_HEADERS
    include/kordex/cli/Version.hpp
    include/kordex/cli/Error.hpp
    include/kordex/cli/Result.hpp
    include/kordex/cli/CliOptions.hpp
    include/kordex/cli/CliConfig.hpp
    include/kordex/cli/CliResult.hpp
    include/kordex/cli/Command.hpp
    include/kordex/cli/CommandRegistry.hpp
    include/kordex/cli/ArgumentParser.hpp
    include/kordex/cli/HelpFormatter.hpp
    include/kordex/cli/Output.hpp
    include/kordex/cli/Commands.hpp
    include/kordex/cli/InitCommand.hpp
    include/kordex/cli/RunCommand.hpp
    include/kordex/cli/CheckCommand.hpp
    include/kordex/cli/BuildCommand.hpp
    include/kordex/cli/ReplCommand.hpp
    include/kordex/cli/VersionCommand.hpp
    include/kordex/cli/Cli.hpp)

set(KORDEX_CLI_SOURCES
    src/Version.cpp
    src/Error.cpp
    src/CliOptions.cpp
    src/CliConfig.cpp
    src/CliResult.cpp
    src/Command.cpp
    src/CommandRegistry.cpp
    src/ArgumentParser.cpp
    src/HelpFormatter.cpp
    src/Output.cpp
    src/Commands.cpp
    src/InitCommand.cpp
    src/RunCommand.cpp
    src/CheckCommand.cpp
    src/BuildCommand.cpp
    src/ReplCommand.cpp
    src/VersionCommand.cpp
    src/Cli.cpp)

# --------------------------------------------------------------------
# Library
# --------------------------------------------------------------------
add_library(kordex_cli STATIC
    ${KORDEX_CLI_HEADERS}
    ${KORDEX_CLI_SOURCES})

add_library(kordex::cli ALIAS kordex_cli)

target_include_directories(kordex_cli
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(kordex_cli
    PUBLIC
      kordex::runtime
      kordex::bindings
      kordex::std
      vix::error
      vix::log
      vix::json
      vix::fs
      vix::path
      vix::env
      vix::process
      vix::time)

target_compile_definitions(kordex_cli
    PUBLIC
      KORDEX_CLI_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
      KORDEX_CLI_VERSION_MINOR=${PROJECT_VERSION_MINOR}
      KORDEX_CLI_VERSION_PATCH=${PROJECT_VERSION_PATCH}

      KORDEX_CLI_ENABLE_INIT_COMMAND=$<BOOL:${KORDEX_CLI_ENABLE_INIT_COMMAND}>
      KORDEX_CLI_ENABLE_RUN_COMMAND=$<BOOL:${KORDEX_CLI_ENABLE_RUN_COMMAND}>
      KORDEX_CLI_ENABLE_CHECK_COMMAND=$<BOOL:${KORDEX_CLI_ENABLE_CHECK_COMMAND}>
      KORDEX_CLI_ENABLE_BUILD_COMMAND=$<BOOL:${KORDEX_CLI_ENABLE_BUILD_COMMAND}>
      KORDEX_CLI_ENABLE_REPL_COMMAND=$<BOOL:${KORDEX_CLI_ENABLE_REPL_COMMAND}>
      KORDEX_CLI_ENABLE_VERSION_COMMAND=$<BOOL:${KORDEX_CLI_ENABLE_VERSION_COMMAND}>)

if(KORDEX_CLI_ENABLE_WARNINGS)
  target_compile_options(kordex_cli
      PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>)
endif()

if(KORDEX_CLI_ENABLE_SANITIZERS AND NOT MSVC)
  target_compile_options(kordex_cli PRIVATE -fsanitize=address,undefined)
  target_link_options(kordex_cli PRIVATE -fsanitize=address,undefined)
endif()

# --------------------------------------------------------------------
# App
# --------------------------------------------------------------------
if(KORDEX_CLI_BUILD_APP)
  add_executable(kordex
      app/main.cpp)

  target_link_libraries(kordex
      PRIVATE
        kordex::cli)

  install(
      TARGETS kordex
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
if(KORDEX_CLI_BUILD_TESTS)
  enable_testing()

  kordex_cli_require_target(
      vix::tests
      vix_tests
      vix_tests
      KORDEX_CLI_FETCH_TESTS
      https://github.com/vixcpp/tests.git
      ${KORDEX_VIX_GIT_TAG})

  set(KORDEX_CLI_TESTS
      test_version
      test_error
      test_cli_options
      test_cli_config
      test_cli_result
      test_command
      test_command_registry
      test_argument_parser
      test_help_formatter
      test_output
      test_cli)

  foreach(test_name IN LISTS KORDEX_CLI_TESTS)
    add_executable(${test_name}
        tests/${test_name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
          kordex::cli
          vix::tests)

    add_test(
        NAME ${test_name}
        COMMAND ${test_name})
  endforeach()
endif()

# --------------------------------------------------------------------
# Examples
# --------------------------------------------------------------------
if(KORDEX_CLI_BUILD_EXAMPLES)
  set(KORDEX_CLI_EXAMPLES
      parse_args
      register_command
      run_cli)

  foreach(example_name IN LISTS KORDEX_CLI_EXAMPLES)
    add_executable(kordex_cli_${example_name}
        examples/${example_name}.cpp)

    target_link_libraries(kordex_cli_${example_name}
        PRIVATE
          kordex::cli)
  endforeach()
endif()

# --------------------------------------------------------------------
# Install
# --------------------------------------------------------------------
install(
    TARGETS kordex_cli
    EXPORT kordex_cliTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    EXPORT kordex_cliTargets
    FILE kordex_cliTargets.cmake
    NAMESPACE kordex::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kordex_cli)

configure_package_config_file(
    cmake/KordexCliConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/kordex_cliConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kordex_cli)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/kordex_cliConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/kordex_cliConfig.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/kordex_cliConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kordex_cli)

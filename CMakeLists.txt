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

cmake_minimum_required(VERSION 3.20)

project(kordex_cli
    VERSION 0.1.0
    DESCRIPTION "Official command line interface for Kordex."
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/KordexCliOptions.cmake")

# --------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------------------------------------
# Export set selection
# --------------------------------------------------------------------
if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
  set(KORDEX_CLI_EXPORT_SET KordexTargets)
else()
  set(KORDEX_CLI_EXPORT_SET kordex_cliTargets)
endif()

# --------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------
function(kordex_cli_require_target target_name package_name sibling_dir fetch_option repository tag)
  if(TARGET ${target_name})
    return()
  endif()

  if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
    message(FATAL_ERROR
        "[cli] Umbrella build requires target '${target_name}' to be provided by the root project.")
  endif()

  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../${sibling_dir}/CMakeLists.txt")
    message(STATUS "[cli] Adding ${target_name} from sibling: ../${sibling_dir}")

    add_subdirectory(
        "${CMAKE_CURRENT_LIST_DIR}/../${sibling_dir}"
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/${sibling_dir}")
  else()
    find_package(${package_name} CONFIG QUIET)
  endif()

  if(TARGET ${target_name})
    return()
  endif()

  if(${fetch_option})
    include(FetchContent)

    message(STATUS "[cli] Fetching ${target_name}")

    FetchContent_Declare(
        ${package_name}
        GIT_REPOSITORY ${repository}
        GIT_TAG ${tag}
        GIT_SHALLOW TRUE)

    FetchContent_MakeAvailable(${package_name})
  endif()

  if(NOT TARGET ${target_name})
    message(FATAL_ERROR
        "[cli] Required dependency target '${target_name}' was not found.\n"
        "Install package '${package_name}', provide sibling '../${sibling_dir}', "
        "or enable ${fetch_option}.")
  endif()
endfunction()

# --------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------
kordex_cli_require_target(
    kordex::runtime
    kordex_runtime
    runtime
    KORDEX_CLI_FETCH_RUNTIME
    https://github.com/kordexjs/runtime.git
    ${KORDEX_RUNTIME_GIT_TAG})

kordex_cli_require_target(
    kordex::bindings
    kordex_bindings
    bindings
    KORDEX_CLI_FETCH_BINDINGS
    https://github.com/kordexjs/bindings.git
    ${KORDEX_BINDINGS_GIT_TAG})

kordex_cli_require_target(
    kordex::std
    kordex_std
    std
    KORDEX_CLI_FETCH_STD
    https://github.com/kordexjs/std.git
    ${KORDEX_STD_GIT_TAG})

kordex_cli_require_target(
    vix::error
    vix_error
    error
    KORDEX_CLI_FETCH_ERROR
    https://github.com/vixcpp/error.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::log
    vix_log
    log
    KORDEX_CLI_FETCH_LOG
    https://github.com/vixcpp/log.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::json
    vix_json
    json
    KORDEX_CLI_FETCH_JSON
    https://github.com/vixcpp/json.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::fs
    vix_fs
    fs
    KORDEX_CLI_FETCH_FS
    https://github.com/vixcpp/fs.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::path
    vix_path
    path
    KORDEX_CLI_FETCH_PATH
    https://github.com/vixcpp/path.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::env
    vix_env
    env
    KORDEX_CLI_FETCH_ENV
    https://github.com/vixcpp/env.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::process
    vix_process
    process
    KORDEX_CLI_FETCH_PROCESS
    https://github.com/vixcpp/process.git
    ${KORDEX_VIX_GIT_TAG})

kordex_cli_require_target(
    vix::time
    vix_time
    time
    KORDEX_CLI_FETCH_TIME
    https://github.com/vixcpp/time.git
    ${KORDEX_VIX_GIT_TAG})

set(KORDEX_CLI_DEPENDENCIES
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

target_compile_features(kordex_cli
    PUBLIC
      cxx_std_20)

target_include_directories(kordex_cli
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(kordex_cli
    PUBLIC
      ${KORDEX_CLI_DEPENDENCIES})

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
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>)
endif()

if(KORDEX_CLI_ENABLE_SANITIZERS)
  if(MSVC)
    message(WARNING "[cli] Sanitizers are not enabled for MSVC in this CMake configuration.")
  else()
    target_compile_options(kordex_cli
        PRIVATE
          -fno-omit-frame-pointer
          -fsanitize=address,undefined)

    target_link_options(kordex_cli
        PRIVATE
          -fsanitize=address,undefined)
  endif()
endif()

set_target_properties(kordex_cli PROPERTIES
    OUTPUT_NAME kordex_cli
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME cli)

# --------------------------------------------------------------------
# App
# --------------------------------------------------------------------
if(KORDEX_CLI_BUILD_APP)
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/app/main.cpp")
    message(FATAL_ERROR "[cli] app/main.cpp is required when KORDEX_CLI_BUILD_APP=ON")
  endif()

  add_executable(kordex
      app/main.cpp)

  target_link_libraries(kordex
      PRIVATE
        kordex::cli)

  target_compile_features(kordex
      PRIVATE
        cxx_std_20)

  install(
      TARGETS kordex
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# --------------------------------------------------------------------
# Tests dependency
# --------------------------------------------------------------------
if(KORDEX_CLI_BUILD_TESTS)
  kordex_cli_require_target(
      vix::tests
      vix_tests
      tests
      KORDEX_CLI_FETCH_TESTS
      https://github.com/vixcpp/tests.git
      ${KORDEX_VIX_GIT_TAG})
endif()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
if(KORDEX_CLI_BUILD_TESTS)
  include(CTest)
  enable_testing()

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
    set(test_source "${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_name}.cpp")

    if(EXISTS "${test_source}")
      add_executable(${test_name}
          "${test_source}")

      target_link_libraries(${test_name}
          PRIVATE
            kordex::cli
            vix::tests)

      add_test(
          NAME ${test_name}
          COMMAND ${test_name})
    endif()
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
    set(example_source "${CMAKE_CURRENT_SOURCE_DIR}/examples/${example_name}.cpp")

    if(EXISTS "${example_source}")
      add_executable(kordex_cli_${example_name}
          "${example_source}")

      target_link_libraries(kordex_cli_${example_name}
          PRIVATE
            kordex::cli)
    endif()
  endforeach()
endif()

# --------------------------------------------------------------------
# Install
# --------------------------------------------------------------------
install(
    TARGETS kordex_cli
    EXPORT ${KORDEX_CLI_EXPORT_SET}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
      PATTERN "*.hpp"
      PATTERN "*.h")

# --------------------------------------------------------------------
# Standalone package config/export
# --------------------------------------------------------------------
if(NOT (DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD))
  configure_package_config_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/KordexCliConfig.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/kordex_cliConfig.cmake"
      INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_cli")

  write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/kordex_cliConfigVersion.cmake"
      VERSION ${PROJECT_VERSION}
      COMPATIBILITY SameMajorVersion)

  install(
      FILES
        "${CMAKE_CURRENT_BINARY_DIR}/kordex_cliConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/kordex_cliConfigVersion.cmake"
      DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_cli")

  install(
      EXPORT kordex_cliTargets
      FILE kordex_cliTargets.cmake
      NAMESPACE kordex::
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_cli")
endif()

# --------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------
message(STATUS "")
message(STATUS "Kordex CLI")
message(STATUS "  version        : ${PROJECT_VERSION}")
message(STATUS "  target         : kordex::cli")
message(STATUS "  export         : ${KORDEX_CLI_EXPORT_SET}")
message(STATUS "  compiler       : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  build          : ${CMAKE_BUILD_TYPE}")
message(STATUS "  app            : ${KORDEX_CLI_BUILD_APP}")
message(STATUS "  tests          : ${KORDEX_CLI_BUILD_TESTS}")
message(STATUS "  examples       : ${KORDEX_CLI_BUILD_EXAMPLES}")
message(STATUS "  warnings       : ${KORDEX_CLI_ENABLE_WARNINGS}")
message(STATUS "  sanitizers     : ${KORDEX_CLI_ENABLE_SANITIZERS}")
message(STATUS "  init command   : ${KORDEX_CLI_ENABLE_INIT_COMMAND}")
message(STATUS "  run command    : ${KORDEX_CLI_ENABLE_RUN_COMMAND}")
message(STATUS "  check command  : ${KORDEX_CLI_ENABLE_CHECK_COMMAND}")
message(STATUS "  build command  : ${KORDEX_CLI_ENABLE_BUILD_COMMAND}")
message(STATUS "  repl command   : ${KORDEX_CLI_ENABLE_REPL_COMMAND}")
message(STATUS "  version command: ${KORDEX_CLI_ENABLE_VERSION_COMMAND}")
message(STATUS "")

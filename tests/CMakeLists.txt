#  @file tests/CMakeLists.txt
#  @author Softadastra
#
#  Copyright 2026, Softadastra. All rights reserved.
#  https://github.com/softadastra/kordex-cli
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex CLI - Tests
# ====================================================================

include_guard(GLOBAL)

# --------------------------------------------------------------------
# Validation
# --------------------------------------------------------------------
if(NOT TARGET kordex::cli)
  message(FATAL_ERROR
      "[cli/tests] Required target 'kordex::cli' was not found. "
      "Tests must be added from the parent kordex-cli CMake project.")
endif()

function(kordex_cli_add_integration_test target_name source_file)
  add_executable(${target_name}
      ${source_file})

  target_compile_features(${target_name}
      PRIVATE
        cxx_std_20)

  target_link_libraries(${target_name}
      PRIVATE
        kordex::cli)

  if(TARGET vix::tests)
    target_link_libraries(${target_name}
        PRIVATE
          vix::tests)
  endif()

  if(KORDEX_CLI_ENABLE_WARNINGS)
    target_compile_options(${target_name}
        PRIVATE
          $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
          $<$<CXX_COMPILER_ID:MSVC>:/W4>)
  endif()

  if(TARGET kordex)
    add_test(
        NAME ${target_name}
        COMMAND ${target_name} $<TARGET_FILE:kordex>)

    set_tests_properties(${target_name}
        PROPERTIES
          DEPENDS kordex)
  else()
    message(STATUS
        "[cli/tests] Skipping ${target_name}: target 'kordex' is not built. "
        "Enable KORDEX_CLI_BUILD_APP=ON to run integration tests.")
  endif()
endfunction()

# --------------------------------------------------------------------
# Helper
# --------------------------------------------------------------------
function(kordex_cli_add_test target_name source_file)
  add_executable(${target_name}
      ${source_file})

  target_compile_features(${target_name}
      PRIVATE
        cxx_std_20)

  target_link_libraries(${target_name}
      PRIVATE
        kordex::cli)

  if(TARGET vix::tests)
    target_link_libraries(${target_name}
        PRIVATE
          vix::tests)
  endif()

  if(KORDEX_CLI_ENABLE_WARNINGS)
    target_compile_options(${target_name}
        PRIVATE
          $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
          $<$<CXX_COMPILER_ID:MSVC>:/W4>)
  endif()

  add_test(
      NAME ${target_name}
      COMMAND ${target_name})
endfunction()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
kordex_cli_add_test(kordex_cli_test_argument_parser test_argument_parser.cpp)
kordex_cli_add_test(kordex_cli_test_cli test_cli.cpp)
kordex_cli_add_test(kordex_cli_test_cli_config test_cli_config.cpp)
kordex_cli_add_integration_test(kordex_cli_test_cli_integration test_cli_integration.cpp)
kordex_cli_add_test(kordex_cli_test_cli_options test_cli_options.cpp)
kordex_cli_add_test(kordex_cli_test_cli_result test_cli_result.cpp)
kordex_cli_add_test(kordex_cli_test_command test_command.cpp)
kordex_cli_add_test(kordex_cli_test_command_registry test_command_registry.cpp)
kordex_cli_add_test(kordex_cli_test_error test_error.cpp)
kordex_cli_add_test(kordex_cli_test_help_formatter test_help_formatter.cpp)
kordex_cli_add_test(kordex_cli_test_output test_output.cpp)
kordex_cli_add_test(kordex_cli_test_version test_version.cpp)

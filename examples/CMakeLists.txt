#  @file examples/CMakeLists.txt
#  @author Softadastra
#
#  Copyright 2026, Softadastra. All rights reserved.
#  https://github.com/softadastra/kordex-cli
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex CLI - Examples
# ====================================================================

include_guard(GLOBAL)

# --------------------------------------------------------------------
# Validation
# --------------------------------------------------------------------
if(NOT TARGET kordex::cli)
  message(FATAL_ERROR
      "[cli/examples] Required target 'kordex::cli' was not found. "
      "Examples must be added from the parent kordex-cli CMake project.")
endif()

# --------------------------------------------------------------------
# Helper
# --------------------------------------------------------------------
function(kordex_cli_add_example target_name source_file)
  add_executable(${target_name}
      ${source_file})

  target_compile_features(${target_name}
      PRIVATE
        cxx_std_20)

  target_link_libraries(${target_name}
      PRIVATE
        kordex::cli)

  if(KORDEX_CLI_ENABLE_WARNINGS)
    target_compile_options(${target_name}
        PRIVATE
          $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
          $<$<CXX_COMPILER_ID:MSVC>:/W4>)
  endif()
endfunction()

# --------------------------------------------------------------------
# Examples
# --------------------------------------------------------------------
kordex_cli_add_example(kordex_cli_parse_args_example parse_args.cpp)
kordex_cli_add_example(kordex_cli_register_command_example register_command.cpp)
kordex_cli_add_example(kordex_cli_run_cli_example run_cli.cpp)

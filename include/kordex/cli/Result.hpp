/**
 *
 *  @file Result.hpp
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

#ifndef KORDEX_CLI_RESULT_HPP
#define KORDEX_CLI_RESULT_HPP

#include <string>

#include <vix/error/Result.hpp>
#include <kordex/cli/Error.hpp>

namespace kordex::cli
{
  /**
   * @brief Kordex CLI result type.
   */
  template <typename T>
  using Result = vix::error::Result<T>;

  /**
   * @brief Result type for boolean-producing CLI operations.
   */
  using BoolResult = Result<bool>;

  /**
   * @brief Result type for string-producing CLI operations.
   */
  using StringResult = Result<::std::string>;

  /**
   * @brief Result type for integer-producing CLI operations.
   */
  using IntResult = Result<int>;

} // namespace kordex::cli

#endif // KORDEX_CLI_RESULT_HPP

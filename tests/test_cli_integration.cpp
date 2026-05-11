/**
 *
 *  @file test_cli_integration.cpp
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

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>

namespace
{
  struct CommandResult
  {
    int exit_code{0};
    std::string output{};
    std::string error{};
  };

  [[nodiscard]] std::string read_text_file(
      const std::filesystem::path &path)
  {
    std::ifstream input(path, std::ios::binary);
    if (!input)
    {
      return {};
    }

    std::ostringstream stream;
    stream << input.rdbuf();

    return stream.str();
  }

  void write_text_file(
      const std::filesystem::path &path,
      const std::string &content)
  {
    const auto parent = path.parent_path();
    if (!parent.empty())
    {
      std::filesystem::create_directories(parent);
    }

    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    assert(output);

    output << content;
    assert(output);
  }

  [[nodiscard]] bool contains(
      const std::string &text,
      const std::string &needle)
  {
    return text.find(needle) != std::string::npos;
  }

  [[nodiscard]] std::string shell_quote(
      const std::string &value)
  {
    std::string result = "'";

    for (char character : value)
    {
      if (character == '\'')
      {
        result += "'\\''";
      }
      else
      {
        result += character;
      }
    }

    result += "'";

    return result;
  }

  [[nodiscard]] int normalize_exit_code(
      int status)
  {
    if (WIFEXITED(status))
    {
      return WEXITSTATUS(status);
    }

    if (WIFSIGNALED(status))
    {
      return 128 + WTERMSIG(status);
    }

    return status;
  }

  [[nodiscard]] CommandResult run_command(
      const std::string &command,
      const std::filesystem::path &work_dir)
  {
    const auto output_path = work_dir / "command.out";
    const auto error_path = work_dir / "command.err";

    const std::string full_command =
        command +
        " > " +
        shell_quote(output_path.string()) +
        " 2> " +
        shell_quote(error_path.string());

    const int status = std::system(full_command.c_str());

    CommandResult result;
    result.exit_code = normalize_exit_code(status);
    result.output = read_text_file(output_path);
    result.error = read_text_file(error_path);

    return result;
  }

  void expect_success(
      const CommandResult &result,
      const std::string &name)
  {
    if (result.exit_code != 0)
    {
      std::cerr << "\nTest failed: " << name << "\n";
      std::cerr << "exit_code = " << result.exit_code << "\n";
      std::cerr << "stdout:\n"
                << result.output << "\n";
      std::cerr << "stderr:\n"
                << result.error << "\n";
    }

    assert(result.exit_code == 0);
  }

  void expect_failure(
      const CommandResult &result,
      const std::string &name)
  {
    if (result.exit_code == 0)
    {
      std::cerr << "\nTest failed: " << name << "\n";
      std::cerr << "expected failure but command succeeded\n";
      std::cerr << "stdout:\n"
                << result.output << "\n";
      std::cerr << "stderr:\n"
                << result.error << "\n";
    }

    assert(result.exit_code != 0);
  }

  [[nodiscard]] std::filesystem::path make_test_root()
  {
    auto root =
        std::filesystem::temp_directory_path() /
        "kordex_cli_integration_test";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    return root;
  }

  void test_repl_eval(
      const std::string &kordex,
      const std::filesystem::path &root)
  {
    const auto result = run_command(
        shell_quote(kordex) + " repl --eval '1 + 2'",
        root);

    expect_success(result, "repl eval");
    assert(contains(result.output, "3"));
  }

  void test_run_file(
      const std::string &kordex,
      const std::filesystem::path &root)
  {
    const auto project = root / "run-file";
    std::filesystem::create_directories(project);

    write_text_file(
        project / "main.js",
        "40 + 2;\n");

    const auto result = run_command(
        shell_quote(kordex) + " run " + shell_quote((project / "main.js").string()),
        project);

    expect_success(result, "run file");
    assert(contains(result.output, "42"));
  }

  void test_imports(
      const std::string &kordex,
      const std::filesystem::path &root)
  {
    const auto project = root / "imports";
    std::filesystem::create_directories(project / "lib");

    write_text_file(
        project / "lib" / "message.js",
        "export const message = \"Hello import\";\n");

    write_text_file(
        project / "main.js",
        "import { message } from \"./lib/message.js\";\n"
        "message;\n");

    const auto result = run_command(
        shell_quote(kordex) + " run " + shell_quote((project / "main.js").string()),
        project);

    expect_success(result, "imports");
    assert(contains(result.output, "Hello import"));
  }

  void test_json_imports(
      const std::string &kordex,
      const std::filesystem::path &root)
  {
    const auto project = root / "json-imports";
    std::filesystem::create_directories(project / "data");

    write_text_file(
        project / "data" / "user.json",
        "{ \"name\": \"Kordex\" }\n");

    write_text_file(
        project / "main.js",
        "import user from \"./data/user.json\";\n"
        "\"Hello \" + user.name;\n");

    const auto result = run_command(
        shell_quote(kordex) + " run " + shell_quote((project / "main.js").string()),
        project);

    expect_success(result, "json imports");
    assert(contains(result.output, "Hello Kordex"));
  }

  void test_std_path_module(
      const std::string &kordex,
      const std::filesystem::path &root)
  {
    const auto project = root / "std-path";
    std::filesystem::create_directories(project);

    write_text_file(
        project / "main.js",
        "import { join } from \"kordex:path\";\n"
        "join(\"/tmp\", \"kordex\", \"std\");\n");

    const auto result = run_command(
        shell_quote(kordex) + " run " + shell_quote((project / "main.js").string()),
        project);

    expect_success(result, "std path module");
    assert(contains(result.output, "/tmp/kordex/std"));
  }

  void test_build(
      const std::string &kordex,
      const std::filesystem::path &root)
  {
    const auto project = root / "build";
    std::filesystem::create_directories(project / "src");
    std::filesystem::create_directories(project / "src" / "lib");

    write_text_file(
        project / "kordex.json",
        "{\n"
        "  \"name\": \"build-test\",\n"
        "  \"entry\": \"src/main.ts\"\n"
        "}\n");

    write_text_file(
        project / "src" / "lib" / "message.ts",
        "export const message: string = \"Hello bundled\";\n");

    write_text_file(
        project / "src" / "main.ts",
        "import { message } from \"./lib/message\";\n"
        "message;\n");

    const auto build_result = run_command(
        shell_quote(kordex) +
            " build " +
            shell_quote(project.string()) +
            " --project --out-dir " +
            shell_quote((project / "dist").string()) +
            " --out-file main.js --force --details",
        project);

    expect_success(build_result, "build command");

    const auto output_file = project / "dist" / "main.js";
    assert(std::filesystem::exists(output_file));

    const auto run_result = run_command(
        shell_quote(kordex) + " run " + shell_quote(output_file.string()),
        project);

    expect_success(run_result, "run bundled output");
    assert(contains(run_result.output, "Hello bundled"));
  }

  void test_permissions(
      const std::string &kordex,
      const std::filesystem::path &root)
  {
    const auto project = root / "permissions";
    std::filesystem::create_directories(project);

    write_text_file(
        project / "fs.js",
        "import { exists } from \"kordex:fs\";\n"
        "exists(\"/tmp\");\n");

    const auto denied_result = run_command(
        shell_quote(kordex) + " run " + shell_quote((project / "fs.js").string()),
        project);

    expect_failure(denied_result, "fs denied by default");

    const auto allowed_result = run_command(
        shell_quote(kordex) +
            " run " +
            shell_quote((project / "fs.js").string()) +
            " --allow-fs",
        project);

    expect_success(allowed_result, "fs allowed with flag");
    assert(contains(allowed_result.output, "true"));
  }
} // namespace

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "usage: test_cli_integration <path-to-kordex>\n";
    return 1;
  }

  const std::string kordex = argv[1];

  if (!std::filesystem::exists(kordex))
  {
    std::cerr << "kordex binary does not exist: " << kordex << "\n";
    return 1;
  }

  const auto root = make_test_root();

  test_repl_eval(kordex, root);
  test_run_file(kordex, root);
  test_imports(kordex, root);
  test_json_imports(kordex, root);
  test_std_path_module(kordex, root);
  test_build(kordex, root);
  test_permissions(kordex, root);

  std::filesystem::remove_all(root);

  std::cout << "Kordex CLI integration tests passed\n";

  return 0;
}

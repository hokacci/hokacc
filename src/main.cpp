#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "token.hpp"
#include "parser.hpp"
#include "generator.hpp"

using namespace yhok::hokacc;


int main(int argc, char* argv[]) {
    auto err_logger = spdlog::stderr_color_mt("stderr");
    spdlog::set_default_logger(err_logger);
    spdlog::set_level(spdlog::level::debug);

    if (argc < 2) {
        fmt::print("Usage: {} <string>\n", argv[0]);
        return 1;
    }
    auto tokens = tokenize(argv[1]);
    Parser parser(*tokens, argv[1]);

    fmt::print(".intel_syntax noprefix\n");
    fmt::print(".global main\n");
    fmt::print("\n");
    fmt::print("main:\n");

    // Prologue
    fmt::print("\tpush rbp\n");
    fmt::print("\tmov rbp, rsp\n");
    fmt::print("\tsub rsp, {}\n", 8 * 26);

    // Generate code
    for (const auto& c : parser.code) {
        generate(*c);
        fmt::print("\tpop rax\n");
    }

    // Epilogue
    fmt::print("\tmov rsp, rbp\n");
    fmt::print("\tpop rbp\n");
    fmt::print("\tret\n");

    return 0;
}

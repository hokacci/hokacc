#pragma once

#include <string>
#include <string_view>
#include <forward_list>
#include <memory>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "parser.hpp"

namespace yhok::hokacc {


inline void generate_lvar(const Node& node) {
    if (node.kind != NodeKind::LVar) {
        spdlog::error("Expected LVar, but got {}", to_string(node));
        std::exit(1);
    }
    fmt::print("\tmov rax, rbp\n");
    fmt::print("\tsub rax, {}\n", node.offset);
    fmt::print("\tpush rax\n");
}


inline void generate(const Node& node) {
    switch (node.kind) {
    case NodeKind::Num:
        fmt::print("\tpush {}\n", node.val);
        return;
    case NodeKind::LVar:
        generate_lvar(node);
        fmt::print("\tpop rax\n");
        fmt::print("\tmov rax, [rax]\n");
        fmt::print("\tpush rax\n");
        return;
    case NodeKind::Assign:
        generate_lvar(*node.lhs);
        generate(*node.rhs);
        fmt::print("\tpop rdi\n");
        fmt::print("\tpop rax\n");
        fmt::print("\tmov [rax], rdi\n");
        fmt::print("\tpush rdi\n");
        return;
    default:
        break;
    }

    generate(*node.lhs);
    generate(*node.rhs);

    fmt::print("\tpop rdi\n");
    fmt::print("\tpop rax\n");

    switch (node.kind) {

    case NodeKind::Add:
        fmt::print("\tadd rax, rdi\n");
        break;

    case NodeKind::Sub:
        fmt::print("\tsub rax, rdi\n");
        break;

    case NodeKind::Mul:
        fmt::print("\timul rax, rdi\n");
        break;

    case NodeKind::Div:
        fmt::print("\tcqo\n");
        fmt::print("\tidiv rdi\n");
        break;

    case NodeKind::Equal:
        fmt::print("\tcmp rax, rdi\n");
        fmt::print("\tsete al\n");
        fmt::print("\tmovzb rax, al\n");
        break;

    case NodeKind::NotEqual:
        fmt::print("\tcmp rax, rdi\n");
        fmt::print("\tsetne al\n");
        fmt::print("\tmovzb rax, al\n");
        break;

    case NodeKind::Less:
        fmt::print("\tcmp rax, rdi\n");
        fmt::print("\tsetl al\n");
        fmt::print("\tmovzb rax, al\n");
        break;

    case NodeKind::LessEqual:
        fmt::print("\tcmp rax, rdi\n");
        fmt::print("\tsetle al\n");
        fmt::print("\tmovzb rax, al\n");
        break;

    default:
        spdlog::error("Unknown node kind: {}", to_string(node));
        std::exit(1);
    }

    fmt::print("\tpush rax\n");
}

}
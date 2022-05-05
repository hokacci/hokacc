#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <forward_list>
#include <memory>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "token.hpp"

namespace yhok::hokacc {

enum struct NodeKind {
    Add,
    Sub,
    Mul,
    Div,

    Equal,
    NotEqual,

    Less,
    LessEqual,

    Assign,

    LVar,

    Num
};


inline std::string to_string(NodeKind kind) {
    switch (kind) {
    case NodeKind::Add: return "Add";
    case NodeKind::Sub: return "Sub";
    case NodeKind::Mul: return "Mul";
    case NodeKind::Div: return "Div";
    case NodeKind::Equal: return "Equal";
    case NodeKind::NotEqual: return "NotEqual";
    case NodeKind::Less: return "Less";
    case NodeKind::LessEqual: return "LessEqual";
    case NodeKind::Assign: return "Assign";
    case NodeKind::LVar: return "LVar";
    case NodeKind::Num: return "Num";
    default: return "Unknown";
    }
}


struct Node {
    NodeKind kind;
    std::unique_ptr<Node> lhs = nullptr;
    std::unique_ptr<Node> rhs = nullptr;
    int val;  // for Num
    std::size_t offset;  // for LVar

    static std::unique_ptr<Node> new_number(int val) {
        auto node = std::make_unique<Node>();
        node->kind = NodeKind::Num;
        node->val = val;
        return node;
    }

    static std::unique_ptr<Node> new_binary_op(NodeKind kind, std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) {
        auto node = std::make_unique<Node>();
        node->kind = kind;
        node->lhs = std::move(lhs);
        node->rhs = std::move(rhs);
        return node;
    }

    static std::unique_ptr<Node> new_lvar(int offset) {
        auto node = std::make_unique<Node>();
        node->kind = NodeKind::LVar;
        node->offset = offset;
        return node;
    }
};


inline std::string to_string(const Node& node) {
    return fmt::format("Node(addr: {}, kind: {}, lhs: {}, rhs: {}, val: {})",
                       (void*)&node,
                       to_string(node.kind),
                       (void*)node.lhs.get(),
                       (void*)node.rhs.get(),
                       node.val);
}


struct LVar {
    std::string_view name;
    std::size_t offset;
};


struct Parser {
    TokenConsumer consumer;
    std::vector<std::unique_ptr<Node>> code;
    std::unordered_map<std::string_view, LVar> lvars;

    Parser(TokenConsumer consumer) : consumer(consumer) {
        program();
    }

    Parser(const std::forward_list<Token>& tokens, std::string_view origin)
        : consumer(tokens, origin) {
        program();
    }

    void program() {
        while (!consumer.at_eof()) {
            code.push_back(stmt());
        }
    }

    std::unique_ptr<Node> stmt() {
        auto node = expr();
        consumer.expect(";");
        spdlog::debug("stmt: {}", to_string(*node));
        return node;
    }

    std::unique_ptr<Node> expr() {
        auto node = assign();
        spdlog::debug("expr: {}", to_string(*node));
        return node;
    }

    std::unique_ptr<Node> assign() {
        auto node = equality();
        if (consumer.consume("=")) {
            node = Node::new_binary_op(NodeKind::Assign, std::move(node), assign());
        }
        spdlog::debug("assign: {}", to_string(*node));
        return node;
    }

    std::unique_ptr<Node> equality() {
        auto node = relational();
        for (;;) {
            if (consumer.consume("==")) {
                node = Node::new_binary_op(NodeKind::Equal, std::move(node), relational());
            } else if (consumer.consume("!=")) {
                node = Node::new_binary_op(NodeKind::NotEqual, std::move(node), relational());
            } else {
                break;
            }
        }
        spdlog::debug("equality: {}", to_string(*node));
        return node;
    }

    std::unique_ptr<Node> relational() {
        auto node = add();
        for (;;) {
            if (consumer.consume("<")) {
                node = Node::new_binary_op(NodeKind::Less, std::move(node), add());
            } else if (consumer.consume("<=")) {
                node = Node::new_binary_op(NodeKind::LessEqual, std::move(node), add());
            } else if (consumer.consume(">")) {
                node = Node::new_binary_op(NodeKind::Less, add(), std::move(node));
            } else if (consumer.consume(">=")) {
                node = Node::new_binary_op(NodeKind::LessEqual, add(), std::move(node));
            } else {
                break;
            }
        }
        spdlog::debug("relational: {}", to_string(*node));
        return node;
    }

    std::unique_ptr<Node> add() {
        auto node = mul();
        for (;;) {
            if (consumer.consume("+")) {
                node = Node::new_binary_op(NodeKind::Add, std::move(node), mul());
            } else if (consumer.consume("-")) {
                node = Node::new_binary_op(NodeKind::Sub, std::move(node), mul());
            } else {
                break;
            }
        }
        spdlog::debug("add: {}", to_string(*node));
        return node;
    }

    std::unique_ptr<Node> mul() {
        auto node = unary();
        for (;;) {
            if (consumer.consume("*")) {
                node = Node::new_binary_op(NodeKind::Mul, std::move(node), unary());
            } else if (consumer.consume("/")) {
                node = Node::new_binary_op(NodeKind::Div, std::move(node), unary());
            } else {
                break;
            }
        }
        spdlog::debug("mul: {}", to_string(*node));
        return node;
    }

    std::unique_ptr<Node> primary() {
        std::unique_ptr<Node> node = nullptr;
        if (consumer.consume("(")) {
            node = expr();
            consumer.expect(")");
        } else if (auto id = consumer.consume_identifier(); id){
            auto it = lvars.find(*id);
            if (it == lvars.end()) {
                std::size_t offset = (lvars.size() + 1) * 8;
                lvars.insert({*id, {*id, offset}});
                node = Node::new_lvar(offset);
            } else {
                node = Node::new_lvar(it->second.offset);
            }
        } else {
            node = Node::new_number(consumer.expect_number());
        }

        spdlog::debug("primary: {}", to_string(*node));
        return node;
    }

    std::unique_ptr<Node> unary() {
        std::unique_ptr<Node> node;
        if (consumer.consume("+")) {
            node = primary();
        } else if (consumer.consume("-")) {
            node = Node::new_binary_op(NodeKind::Sub, Node::new_number(0), primary());
        } else {
            node = primary();
        }

        spdlog::debug("unary: {}", to_string(*node));
        return node;
    }
};


}


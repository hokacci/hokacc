#pragma once

#include <string>
#include <string_view>
#include <forward_list>
#include <memory>
#include <optional>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

namespace yhok::hokacc {

enum struct TokenKind {
    Reserved,    // 記号
    Identifier,  // 識別子
    Number,      // 整数
    Return,      // return
    EndOfFile    // EOF
};

inline std::string to_string(TokenKind kind) {
    switch (kind) {
    case TokenKind::Reserved: return "Reserved";
    case TokenKind::Identifier: return "Identifier";
    case TokenKind::Number: return "Number";
    case TokenKind::Return: return "Return";
    case TokenKind::EndOfFile: return "EndOfFile";
    default: return "Unknown";
    }
}

struct Token {
    TokenKind kind;
    std::size_t loc;
    int value;
    std::string_view str;
};


inline std::string to_string(const Token& token) {
    return fmt::format("Token(kind: {}, loc: {}, value: {}, str: {})", to_string(token.kind), token.loc, token.value, token.str);
}


inline std::unique_ptr<std::forward_list<Token>> tokenize(std::string_view str) {
    auto tokens = std::make_unique<std::forward_list<Token>>();
    // ダミーの先頭トークン
    tokens->push_front(Token{});

    // 最後尾のトークンのイテレータ
    auto it = tokens->begin();

    spdlog::debug("Tokenizing: {}", str);

    std::size_t loc = 0;
    auto c = str.begin();
    while (c != str.end()) {
        loc = c - str.begin();
        if (std::isspace(*c)) {
            ++c;
            continue;
        }
        if (*c == '!') {
            if ((c + 1) != str.end() && *(c + 1) == '=') {
                it = tokens->insert_after(it, Token{TokenKind::Reserved, loc, 0, std::string_view(c, 2)});
                c = c + 2;
                continue;
            }
        }
        if (*c == '=') {
            if ((c + 1) != str.end() && *(c + 1) == '=') {
                it = tokens->insert_after(it, Token{TokenKind::Reserved, loc, 0, std::string_view(c, 2)});
                c = c + 2;
                continue;
            } else {
                it = tokens->insert_after(it, Token{TokenKind::Reserved, loc, 0, std::string_view(c, 1)});
                ++c;
                continue;
            }
        }
        if (*c == '<') {
            if ((c + 1) != str.end() && *(c + 1) == '=') {
                it = tokens->insert_after(it, Token{TokenKind::Reserved, loc, 0, std::string_view(c, 2)});
                c = c + 2;
                continue;
            } else {
                it = tokens->insert_after(it, Token{TokenKind::Reserved, loc, 0, std::string_view(c, 1)});
                ++c;
                continue;
            }
        }
        if (*c == '>') {
            if ((c + 1) != str.end() && *(c + 1) == '=') {
                it = tokens->insert_after(it, Token{TokenKind::Reserved, loc, 0, std::string_view(c, 2)});
                c = c + 2;
                continue;
            } else {
                it = tokens->insert_after(it, Token{TokenKind::Reserved, loc, 0, std::string_view(c, 1)});
                ++c;
                continue;
            }
        }
        if (*c == '+' || *c == '-' || *c == '*' || *c == '/' || *c == '(' || *c == ')' || *c == ';') {
            it = tokens->insert_after(it, Token{TokenKind::Reserved, loc, 0, std::string_view(c, 1)});
            ++c;
            continue;
        }
        if (std::isdigit(*c)) {
            std::size_t idx;
            int n = std::stoi(c, &idx);
            it = tokens->insert_after(it, Token{TokenKind::Number, loc, n, std::string_view(c, idx)});
            c += idx;
            continue;
        }
        if (std::isalpha(*c)) {
            if (str.substr(loc, 6) == "return" && !(c + 6 != str.end() && (std::isalpha(*(c + 6)) || std::isdigit(*(c + 6)) || *(c + 6) == '_'))) {
                it = tokens->insert_after(it, Token{TokenKind::Return, loc, 0, std::string_view(c, 6)});
                c += 6;
                continue;
            } else {
                auto* p = c + 1;
                while (p != str.end() && (std::isalpha(*p) || std::isdigit(*p) || *p == '_')) {
                    ++p;
                }
                std::size_t idx = p - c;
                it = tokens->insert_after(it, Token{TokenKind::Identifier, loc, 0, std::string_view(c, idx)});
                c += idx;
                continue;
            }
        }
        spdlog::error("Failed to tokenize:");
        spdlog::error("{}", str);
        spdlog::error("{:>{}}^ Failed to tokenize", "", loc);
        std::exit(1);
    }

    it = tokens->insert_after(it, Token{TokenKind::EndOfFile, loc + 1, 0, std::string_view()});

    // ダミーの先頭トークンを削除
    tokens->pop_front();

    spdlog::debug("Finished tokenizing:");
    for (const auto& token : *tokens) {
        spdlog::debug("{}", to_string(token));
    }

    return tokens;
}


struct TokenConsumer {
    const std::forward_list<Token>& tokens;
    std::string_view origin;
    std::forward_list<Token>::const_iterator it;

    TokenConsumer(const std::forward_list<Token>& tokens, std::string_view origin)
        : tokens(tokens), origin(origin), it(tokens.begin()) {}

    bool consume(std::string_view keyword) {
        switch (it->kind) {
        case TokenKind::Reserved:
        case TokenKind::Return:
            if (it->str == keyword) {
                ++it;
                return true;
            }
            break;
        default:
            break;
        }
        return false;
    }

    void expect(std::string_view keyword) {
        switch (it->kind) {
        case TokenKind::Reserved:
        case TokenKind::Return:
            if (it->str == keyword) {
                ++it;
                return;
            }
            break;
        default:
            break;
        }
        spdlog::error("Expected {}, but got {}", keyword, to_string(*it));
        spdlog::error("{}", origin);
        spdlog::error("{:>{}}^ Expected {}", "", it->loc, keyword);
        std::exit(1);
    }

    std::optional<int> consume_number() {
        if (it->kind != TokenKind::Number) {
            return std::nullopt;
        }
        int val = it->value;
        ++it;
        return val;
    }

    int expect_number() {
        if (it->kind != TokenKind::Number) {
            spdlog::error("Expected number, but got {}", to_string(*it));
            spdlog::error("{}", origin);
            spdlog::error("{:>{}}^ Expected number", "", it->loc);
            std::exit(1);
        }
        int val = it->value;
        ++it;
        return val;
    }

    std::optional<std::string_view> consume_identifier() {
        if (it->kind != TokenKind::Identifier) {
            return std::nullopt;
        }
        std::string_view val = it->str;
        ++it;
        return val;
    }

    std::string_view expect_identifier() {
        if (it->kind != TokenKind::Identifier) {
            spdlog::error("Expected identifier, but got {}", to_string(*it));
            spdlog::error("{}", origin);
            spdlog::error("{:>{}}^ Expected identifier", "", it->loc);
            std::exit(1);
        }
        std::string_view val = it->str;
        ++it;
        return val;
    }

    bool at_eof() {
        return it->kind == TokenKind::EndOfFile;
    }
};

}

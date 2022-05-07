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
    // Reserved
    LParen,
    RParen,
    Assign,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    SemiColon,
    Plus,
    Minus,
    Slash,
    Star,
    Return,      // return

    Identifier,  // 識別子
    Number,      // 整数
    EndOfFile    // EOF
};

inline std::string to_string(TokenKind kind) {
    switch (kind) {
    case TokenKind::LParen: return "LParen";
    case TokenKind::RParen: return "RParen";
    case TokenKind::Assign: return "Assign";
    case TokenKind::Equal: return "Equal";
    case TokenKind::NotEqual: return "NotEqual";
    case TokenKind::Less: return "Less";
    case TokenKind::LessEqual: return "LessEqual";
    case TokenKind::Greater: return "Greater";
    case TokenKind::GreaterEqual: return "GreaterEqual";
    case TokenKind::SemiColon: return "SemiColon";
    case TokenKind::Plus: return "Plus";
    case TokenKind::Minus: return "Minus";
    case TokenKind::Slash: return "Slash";
    case TokenKind::Star: return "Star";
    case TokenKind::Return: return "Return";
    case TokenKind::Identifier: return "Identifier";
    case TokenKind::Number: return "Number";
    case TokenKind::EndOfFile: return "EndOfFile";
    default: return "Unknown";
    }
}

inline std::string to_literal_string(TokenKind kind) {
    switch (kind) {
    case TokenKind::LParen: return "(";
    case TokenKind::RParen: return ")";
    case TokenKind::Assign: return "=";
    case TokenKind::Equal: return "==";
    case TokenKind::NotEqual: return "!=";
    case TokenKind::Less: return "<";
    case TokenKind::LessEqual: return "<=";
    case TokenKind::Greater: return ">";
    case TokenKind::GreaterEqual: return ">=";
    case TokenKind::SemiColon: return ";";
    case TokenKind::Plus: return "+";
    case TokenKind::Minus: return "-";
    case TokenKind::Slash: return "/";
    case TokenKind::Star: return "*";
    case TokenKind::Return: return "return";
    case TokenKind::Identifier: return "Identifier";
    case TokenKind::Number: return "Number";
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
                it = tokens->insert_after(it, Token{TokenKind::NotEqual, loc, 0, std::string_view(c, 2)});
                c = c + 2;
                continue;
            }
        }
        if (*c == '=') {
            if ((c + 1) != str.end() && *(c + 1) == '=') {
                it = tokens->insert_after(it, Token{TokenKind::Equal, loc, 0, std::string_view(c, 2)});
                c = c + 2;
                continue;
            } else {
                it = tokens->insert_after(it, Token{TokenKind::Assign, loc, 0, std::string_view(c, 1)});
                ++c;
                continue;
            }
        }
        if (*c == '<') {
            if ((c + 1) != str.end() && *(c + 1) == '=') {
                it = tokens->insert_after(it, Token{TokenKind::LessEqual, loc, 0, std::string_view(c, 2)});
                c = c + 2;
                continue;
            } else {
                it = tokens->insert_after(it, Token{TokenKind::Less, loc, 0, std::string_view(c, 1)});
                ++c;
                continue;
            }
        }
        if (*c == '>') {
            if ((c + 1) != str.end() && *(c + 1) == '=') {
                it = tokens->insert_after(it, Token{TokenKind::GreaterEqual, loc, 0, std::string_view(c, 2)});
                c = c + 2;
                continue;
            } else {
                it = tokens->insert_after(it, Token{TokenKind::Greater, loc, 0, std::string_view(c, 1)});
                ++c;
                continue;
            }
        }
        if (*c == '+') {
            it = tokens->insert_after(it, Token{TokenKind::Plus, loc, 0, std::string_view(c, 1)});
            ++c;
            continue;
        }
        if (*c == '-') {
            it = tokens->insert_after(it, Token{TokenKind::Minus, loc, 0, std::string_view(c, 1)});
            ++c;
            continue;
        }
        if (*c == '/') {
            it = tokens->insert_after(it, Token{TokenKind::Slash, loc, 0, std::string_view(c, 1)});
            ++c;
            continue;
        }
        if (*c == '*') {
            it = tokens->insert_after(it, Token{TokenKind::Star, loc, 0, std::string_view(c, 1)});
            ++c;
            continue;
        }
        if (*c == ';') {
            it = tokens->insert_after(it, Token{TokenKind::SemiColon, loc, 0, std::string_view(c, 1)});
            ++c;
            continue;
        }
        if (*c == '(') {
            it = tokens->insert_after(it, Token{TokenKind::LParen, loc, 0, std::string_view(c, 1)});
            ++c;
            continue;
        }
        if (*c == ')') {
            it = tokens->insert_after(it, Token{TokenKind::RParen, loc, 0, std::string_view(c, 1)});
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

    loc = c - str.begin();
    it = tokens->insert_after(it, Token{TokenKind::EndOfFile, loc, 0, std::string_view(" ")});

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

    bool consume(TokenKind kind) {
        if (it->kind != kind) {
            return false;
        }
        ++it;
        return true;
    }

    void expect(TokenKind kind) {
        if (it->kind != kind) {
            auto keyword = to_literal_string(kind);
            spdlog::error("Expected {}, but got {}", keyword, to_string(*it));
            spdlog::error("{}", origin);
            spdlog::error("{:>{}}^{:~>{}} Expected {}", "", it->loc, "", it->str.size() - 1, keyword);
            std::exit(1);
        }
        ++it;
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
            spdlog::error("{:>{}}^{:~>{}} Expected number", "", it->loc, "", it->str.size() - 1);
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
            spdlog::error("{:>{}}^{:~>{}} Expected identifier", "", it->loc, "", it->str.size() - 1);
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

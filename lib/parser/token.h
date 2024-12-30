#pragma once

#include <util.h>

#include <string>
#include <variant>

#define FORALL_TOKEN_KINDS_SEP(op, sep) \
    op(Eof) sep op(Def)                 \
    sep op(Extern)                      \
    sep op(Ident)                       \
    sep op(Number)                      \
    sep op(Bracket)                     \
    sep op(Comma)                       \
    sep op(Semicolon)                   \
    sep op(Operator)

#define FORALL_TOKEN_KINDS_COMMA(op) FORALL_TOKEN_KINDS_SEP(op, JUST(, ))
#define FORALL_TOKEN_KINDS(op) FORALL_TOKEN_KINDS_SEP(op, JUST())

namespace token {

enum class TokenKind {
#define TOKEN_VARIANT(name) k##name,
    FORALL_TOKEN_KINDS(TOKEN_VARIANT)
#undef TOKEN_VARIANT
};

struct Eof {
    bool operator==(const Eof&) const = default;
};

struct Def {
    bool operator==(const Def&) const = default;
};

struct Extern {
    bool operator==(const Extern&) const = default;
};

struct Ident {
    std::string value;

    bool operator==(const Ident&) const = default;
};

struct Number {
    double value;

    bool operator==(const Number&) const = default;
};

enum class BracketKind {
    kOpen,
    kClose,
};

struct Bracket {
    BracketKind kind;

    bool operator==(const Bracket&) const = default;
};

struct Comma {
    bool operator==(const Comma&) const = default;
};

struct Semicolon {
    bool operator==(const Semicolon&) const = default;
};

struct Operator {
    std::string op;

    bool operator==(const Operator&) const = default;
};

}  // namespace token

#define TOKEN_VARIANT(name) token::name
using Token = std::variant<FORALL_TOKEN_KINDS_COMMA(TOKEN_VARIANT)>;
#undef TOKEN_VARIANT

token::TokenKind GetTokenKind(const Token& token);

#include "token.h"

#include <overloaded.h>

token::TokenKind GetTokenKind(const Token &token) {
    return std::visit(
        Overloaded{
#define TOKEN_KIND(name) [](const token::name &) { return token::TokenKind::k##name; },
            FORALL_TOKEN_KINDS(TOKEN_KIND)
#undef TOKEN_KIND
        },
        token);
}

#include "parser.h"

#include <overloaded.h>
#include <parser/token.h>
#include <utility>

using namespace token;

ast::NodePtr Parser::ParseIdentifierExpr() {
    auto pident = std::get_if<Ident>(&tokenizer_.Get());
    if (!pident) {
        return LogError("Identifier expected");
    }
    std::string ident = pident->value;
    tokenizer_.Next();

    if (tokenizer_.Get() != Token{Bracket{BracketKind::kOpen}}) {
        return ast::MakeNodePtr(std::in_place_type<ast::Variable>, std::move(ident));
    }
    tokenizer_.Next();

    std::vector<ast::NodePtr> args;
    if (tokenizer_.Get() != Token{Bracket{BracketKind::kClose}}) {
        while (true) {
            if (auto arg = ParseExpression()) {
                args.push_back(std::move(arg));
            } else {
                return nullptr;
            }

            if (tokenizer_.Get() == Token{Bracket{BracketKind::kClose}}) {
                break;
            }
            if (tokenizer_.Get() != Token{Comma{}}) {
                return LogError("Expected ')' or ',' in argument list");
            }
            tokenizer_.Next();
        }
    }
    tokenizer_.Next();

    return ast::MakeNodePtr(std::in_place_type<ast::CallExpression>, std::move(ident),
                            std::move(args));
}

ast::NodePtr Parser::ParsePrimary() {
    constexpr auto msg = "unknown token when expecting an expression";
    return std::visit(Overloaded{
                          [this](Ident ident) { return ParseIdentifierExpr(); },
                          [this](Number number) { return ParseNumberExpr(); },
                          [this](Bracket bracket) -> ast::NodePtr {
                              if (bracket.kind != BracketKind::kOpen) {
                                  return LogError(msg);
                              }
                              return ParseParenExpr();
                          },
                          [](const auto&) -> ast::NodePtr { return LogError(msg); },
                      },
                      tokenizer_.Get());
}

ast::NodePtr Parser::ParseNumberExpr() {
    auto number = std::get_if<Number>(&tokenizer_.Get());
    if (!number) {
        return LogError("This shouldn't happen");
    }
    auto value = number->value;
    tokenizer_.Next();
    return ast::MakeNodePtr(std::in_place_type<ast::Number>, value);
}

ast::NodePtr Parser::ParseParenExpr() {
    tokenizer_.Next();
    auto v = ParseExpression();
    if (!v) {
        return nullptr;
    }
    if (tokenizer_.Get() != Token{Bracket{BracketKind::kClose}}) {
        return LogError("expected ')'");
    }
    tokenizer_.Next();
    return v;
}

ast::NodePtr Parser::ParseBinOpRHS(uint8_t lhs_prec, ast::NodePtr lhs) {
    auto operator_not_found = [](std::string_view op) {
        return LogError(std::format("Operator {} not found", op));
    };

    while (true) {
        auto op = GetNextOperator();
        if (!op) {
            return lhs;
        }
        auto op_prec = GetBinopPrecedence(*op);
        if (!op_prec) {
            return operator_not_found(*op);
        }
        if (*op_prec < lhs_prec) {
            return lhs;
        }

        tokenizer_.Next();
        auto rhs = ParsePrimary();
        if (!rhs) {
            return nullptr;
        }

        auto next_op = GetNextOperator();
        auto next_op_prec = std::invoke([this, next_op]() -> std::optional<uint8_t> {
            if (!next_op) {
                return 0;
            }
            return GetBinopPrecedence(*next_op);
        });
        if (!next_op_prec) {
            return operator_not_found(*next_op);
        }

        if (*op_prec < *next_op_prec) {
            rhs = ParseBinOpRHS(*op_prec + 1, std::move(rhs));
            if (!rhs) {
                return nullptr;
            }
        }

        lhs = ast::MakeNodePtr(std::in_place_type<ast::BinaryOp>, *op, std::move(lhs),
                               std::move(rhs));
    }
}

ast::NodePtr Parser::ParseExpression() {
    auto lhs = ParsePrimary();
    if (!lhs) {
        return nullptr;
    }
    return ParseBinOpRHS(0, std::move(lhs));
}

std::unique_ptr<ast::Prototype> Parser::ParsePrototype() {
    tokenizer_.Next();
    auto ident = std::get_if<Ident>(&tokenizer_.Get());
    if (!ident) {
        return LogError("Expected function name in prototype");
    }
    auto fn_name = ident->value;
    tokenizer_.Next();

    if (tokenizer_.Get() != Token{Bracket{BracketKind::kOpen}}) {
        return LogError("Expected '(' in prototype");
    }
    tokenizer_.Next();

    std::vector<std::string> arg_names;
    while (auto ident = std::get_if<Ident>(&tokenizer_.Get())) {
        arg_names.push_back(ident->value);
        tokenizer_.Next();
    }
    if (tokenizer_.Get() != Token{Bracket{BracketKind::kClose}}) {
        return LogError("Expected ')' in prototype");
    }
    tokenizer_.Next();

    return std::make_unique<ast::Prototype>(std::move(fn_name), std::move(arg_names));
}

std::unique_ptr<ast::Function> Parser::ParseDefinition() {
    auto proto = ParsePrototype();
    if (!proto) {
        return nullptr;
    }

    auto body = ParseExpression();
    if (!body) {
        return nullptr;
    }

    return std::make_unique<ast::Function>(std::move(*proto), std::move(body));
}

std::unique_ptr<ast::Function> Parser::ParseTopLevelExpr() {
    auto expr = ParseExpression();
    if (!expr) {
        return nullptr;
    }
    auto proto = std::make_unique<ast::Prototype>("__anon_expr", std::vector<std::string>{});
    return std::make_unique<ast::Function>(
        ast::Prototype{
            .name = "__anon_expr",
            .args = {},
        },
        std::move(expr));
}

std::unique_ptr<ast::Prototype> Parser::ParseExtern() {
    tokenizer_.Next();
    return ParsePrototype();
}

std::optional<uint8_t> Parser::GetBinopPrecedence(const std::string& op) const {
    if (auto it = precedence_.find(op); it != precedence_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::string> Parser::GetNextOperator() const {
    return std::visit(Overloaded{
                          [](const Operator& op) -> std::optional<std::string> { return op.op; },
                          [](const auto&) -> std::optional<std::string> { return std::nullopt; },
                      },
                      tokenizer_.Get());
}

Tokenizer* Parser::GetTokenizer() {
    return &tokenizer_;
}

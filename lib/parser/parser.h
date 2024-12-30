#pragma once

#include "ast.h"
#include "tokenizer.h"

#include <cstdint>
#include <map>

struct Parser {
    Parser(std::map<std::string, uint8_t> precedence, std::istream* in)
        : precedence_(std::move(precedence)), tokenizer_(in) {
    }

    ast::NodePtr ParseExpression();
    std::unique_ptr<ast::Function> ParseDefinition();
    std::unique_ptr<ast::Function> ParseTopLevelExpr();
    std::unique_ptr<ast::Prototype> ParseExtern();

    Tokenizer* GetTokenizer();

private:
    ast::NodePtr ParseIdentifierExpr();
    ast::NodePtr ParseParenExpr();
    ast::NodePtr ParseNumberExpr();
    ast::NodePtr ParsePrimary();
    ast::NodePtr ParseBinOpRHS(uint8_t expr_prec, ast::NodePtr lhs);
    std::unique_ptr<ast::Prototype> ParsePrototype();

    std::optional<uint8_t> GetBinopPrecedence(const std::string& op) const;
    std::optional<std::string> GetNextOperator() const;

    std::map<std::string, uint8_t> precedence_;
    Tokenizer tokenizer_;
};

#include <parser/parser.h>
#include <parser/ast.h>
#include <parser/debug.h>

#include <gtest/gtest.h>

namespace {

const std::map<std::string, uint8_t> kDefaultPrecedence = {
    {"<", 10},
    {"+", 20},
    {"-", 20},
    {"*", 40},
};

template <class Expr>
void CheckParsedExpression(std::string input, Expr (Parser::*parserMethod)(),
                           std::string_view expected,
                           std::map<std::string, uint8_t> precedence = kDefaultPrecedence) {
    std::istringstream iss(std::move(input));
    Parser p{std::move(precedence), &iss};

    std::ostringstream actual;
    auto expr = (p.*parserMethod)();
    ASSERT_TRUE(expr);
    actual << debug::Debug{expr.get()};
    EXPECT_EQ(actual.str(), expected);
}

// TODO: Move these to tests/debug.cpp
ast::NodePtr Number(double value) {
    return ast::MakeNodePtr(std::in_place_type<ast::Number>, value);
}

ast::NodePtr Variable(std::string name) {
    return ast::MakeNodePtr(std::in_place_type<ast::Variable>, std::move(name));
}

ast::NodePtr Operator(std::string op, ast::NodePtr lhs, ast::NodePtr rhs) {
    return ast::MakeNodePtr(std::in_place_type<ast::BinaryOp>, std::move(op), std::move(lhs),
                            std::move(rhs));
}

}  // namespace

TEST(Parser, SimpleExpression) {
    CheckParsedExpression("1 + 2 * 3", &Parser::ParseExpression, "(+ 1 (* 2 3))");
    CheckParsedExpression("1 - 2 + 3", &Parser::ParseExpression, "(+ (- 1 2) 3)");
    CheckParsedExpression("a * 2 + 3", &Parser::ParseExpression, "(+ (* a 2) 3)");
}

TEST(Parser, Precedence) {
    // Bracketless expressions
    CheckParsedExpression("2 * 1 < 1 + x + a * x * 3 * 2 + y * x * 2", &Parser::ParseExpression,
                          "(< (* 2 1) (+ (+ (+ 1 x) (* (* (* a x) 3) 2)) (* (* y x) 2)))");
    CheckParsedExpression("a * 2 * y + x * 3 + y + x < 1 < 3 < 1 * y", &Parser::ParseExpression,
                          "(< (< (< (+ (+ (+ (* (* a 2) y) (* x 3)) y) x) 1) 3) (* 1 y))");

    // Bracket expressions
    CheckParsedExpression("2 < y * (a * (1 * 1) * (3 - 1) * a)", &Parser::ParseExpression,
                          "(< 2 (* y (* (* (* a (* 1 1)) (- 3 1)) a)))");
    CheckParsedExpression("((1 * ((3 < 3 < 1 < 3 * 3) < y)) < 3 - 2) * a", &Parser::ParseExpression,
                          "(* (< (* 1 (< (< (< (< 3 3) 1) (* 3 3)) y)) (- 3 2)) a)");
    CheckParsedExpression("y - 3 * (a - (2 * a))", &Parser::ParseExpression,
                          "(- y (* 3 (- a (* 2 a))))");
    CheckParsedExpression("y - 3 < (3 - (y * x * 1) * 3) < 2", &Parser::ParseExpression,
                          "(< (< (- y 3) (- 3 (* (* (* y x) 1) 3))) 2)");
    CheckParsedExpression("1 * a * 3 < 2 * (2 * x + 3 * 3 < 2 * y)", &Parser::ParseExpression,
                          "(< (* (* 1 a) 3) (* 2 (< (+ (* 2 x) (* 3 3)) (* 2 y))))");
    CheckParsedExpression("((a - y) - x < 3) < 1", &Parser::ParseExpression,
                          "(< (< (- (- a y) x) 3) 1)");
}

TEST(Parser, Prototype) {
    CheckParsedExpression("extern def f();", &Parser::ParseExtern, "(<proto> f)");
    CheckParsedExpression("extern def abc(df ghi jkl)", &Parser::ParseExtern,
                          "(<proto> abc df ghi jkl)");
}

TEST(Parser, Call) {
    CheckParsedExpression("fa(a, b) * (fb(a, c) + fc)", &Parser::ParseExpression,
                          "(* (<call> fa a b) (+ (<call> fb a c) fc))");
}

TEST(Parser, Definition) {
    CheckParsedExpression("def ff(a b c) a + b * 1.2 < c", &Parser::ParseDefinition,
                          "(<func> (<proto> ff a b c) (< (+ a (* b 1.2)) c))");
    CheckParsedExpression("def f() 42", &Parser::ParseDefinition, "(<func> (<proto> f) 42)");
    CheckParsedExpression("def f(x) x;", &Parser::ParseDefinition, "(<func> (<proto> f x) x)");
}

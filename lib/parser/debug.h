#pragma once

#include <ostream>
#include "ast.h"

namespace debug {

template <class T>
struct Debug {
    const T* inner;
};

std::ostream& operator<<(std::ostream& out, Debug<ast::Variable> var);
std::ostream& operator<<(std::ostream& out, Debug<ast::Number> number);
std::ostream& operator<<(std::ostream& out, Debug<ast::BinaryOp> op);
std::ostream& operator<<(std::ostream& out, Debug<ast::CallExpression> call);
std::ostream& operator<<(std::ostream& out, Debug<ast::Node> node);
std::ostream& operator<<(std::ostream& out, Debug<ast::Prototype> proto);
std::ostream& operator<<(std::ostream& out, Debug<ast::Function> func);

}  // namespace debug

#include "debug.h"
#include "parser/ast.h"

namespace debug {

std::ostream& operator<<(std::ostream& out, Debug<ast::Variable> var) {
    return out << var.inner->name;
}

std::ostream& operator<<(std::ostream& out, Debug<ast::Number> number) {
    return out << number.inner->value;
}

std::ostream& operator<<(std::ostream& out, Debug<ast::BinaryOp> op) {
    return out << "(" << op.inner->op << " " << Debug{op.inner->lhs.get()} << " "
               << Debug{op.inner->rhs.get()} << ")";
}

std::ostream& operator<<(std::ostream& out, Debug<ast::CallExpression> call) {
    out << "(<call> " << call.inner->callee;
    for (const auto& arg : call.inner->args) {
        out << " " << Debug{arg.get()};
    }
    return out << ")";
}

std::ostream& operator<<(std::ostream& out, Debug<ast::Node> node) {
    std::visit([&out](const auto& inner) { out << Debug{&inner}; }, *node.inner);
    return out;
}

std::ostream& operator<<(std::ostream& out, Debug<ast::Prototype> proto) {
    out << "(<proto> " << proto.inner->name;
    for (const auto& arg : proto.inner->args) {
        out << " " << arg;
    }
    out << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, Debug<ast::Function> func) {
    return out << "(<func> " << Debug{&func.inner->proto} << " " << Debug{func.inner->body.get()}
               << ")";
}

}  // namespace debug

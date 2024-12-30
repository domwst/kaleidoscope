#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace ast {

struct Number;
struct Variable;
struct BinaryOp;
struct CallExpression;

using Node = std::variant<Number, Variable, BinaryOp, CallExpression>;
using NodePtr = std::unique_ptr<Node>;

struct Number {
    double value;
};

struct Variable {
    std::string name;
};

struct BinaryOp {
    std::string op;
    NodePtr lhs;
    NodePtr rhs;
};

struct CallExpression {
    std::string callee;
    std::vector<NodePtr> args;
};

struct Prototype {
    std::string name;
    std::vector<std::string> args;
};

struct Function {
    Prototype proto;
    NodePtr body;

    const std::string& GetName() const {
        return proto.name;
    }
};

template <class T>
NodePtr MakeNodePtr(T&& value) {
    return std::make_unique<Node>(std::forward<T>(value));
}

template <class T, class... Args>
NodePtr MakeNodePtr(std::in_place_type_t<T>, Args&&... args) {
    return MakeNodePtr(T{std::forward<Args>(args)...});
}

}  // namespace ast

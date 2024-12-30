#include "codegen.h"

#include <format>

llvm::Value* Codegen(const ast::Number& number, CodegenCtx* ctx) {
    return llvm::ConstantFP::get(ctx->context, llvm::APFloat(number.value));
}

llvm::Value* Codegen(const ast::Variable& var, CodegenCtx* ctx) {
    auto it = ctx->named_values.find(var.name);
    if (it == ctx->named_values.end()) {
        return LogError(std::format("Unknown variable name {}", var.name));
    }
    return it->second;
}

llvm::Value* Codegen(const ast::BinaryOp& op, CodegenCtx* ctx) {
    auto l = Codegen(*op.lhs, ctx);
    auto r = Codegen(*op.rhs, ctx);
    if (!l || !r) {
        return nullptr;
    }

    if (op.op == "+") {
        return ctx->builder.CreateFAdd(l, r, "addtmp");
    } else if (op.op == "-") {
        return ctx->builder.CreateFSub(l, r, "subtmp");
    } else if (op.op == "*") {
        return ctx->builder.CreateFMul(l, r, "multmp");
    } else if (op.op == "<") {
        auto result = ctx->builder.CreateFCmpULT(l, r, "cmptmp");
        return ctx->builder.CreateUIToFP(result, llvm::Type::getDoubleTy(ctx->context), "booltmp");
    } else {
        return LogError(std::format("Invalid binary operator {}", op.op));
    }
}

llvm::Value* Codegen(const ast::CallExpression& expr, CodegenCtx* ctx) {
    llvm::Function* callee = ctx->module.getFunction(expr.callee);
    if (!callee) {
        return LogError(std::format("Unknown function {}", expr.callee));
    }
    if (callee->arg_size() != expr.args.size()) {
        return LogError(std::format("Invalid number of arguments passed ({} while expected {})",
                                    expr.args.size(), callee->arg_size()));
    }

    std::vector<llvm::Value*> args;
    for (auto& arg : expr.args) {
        auto argv = Codegen(*arg, ctx);
        if (!argv) {
            return nullptr;
        }
        args.push_back(argv);
    }

    return ctx->builder.CreateCall(callee, args, "calltmp");
}

llvm::Value* Codegen(const ast::Node& node, CodegenCtx* ctx) {
    return std::visit([ctx](const auto& value) { return Codegen(value, ctx); }, node);
}

llvm::Function* Codegen(const ast::Prototype& proto, CodegenCtx* ctx) {
    std::vector<llvm::Type*> doubles(proto.args.size(), llvm::Type::getDoubleTy(ctx->context));
    llvm::FunctionType* ft =
        llvm::FunctionType::get(llvm::Type::getDoubleTy(ctx->context), doubles, false);

    llvm::Function* f =
        llvm::Function::Create(ft, llvm::Function::ExternalLinkage, proto.name, ctx->module);

    size_t idx = 0;
    for (auto& arg : f->args()) {
        arg.setName(proto.args[idx++]);
    }
    return f;
}

llvm::Function* Codegen(const ast::Function& function_expr, CodegenCtx* ctx) {
    llvm::Function* function = ctx->module.getFunction(function_expr.GetName());

    if (!function) {
        function = Codegen(function_expr.proto, ctx);
    }
    if (!function) {
        return nullptr;
    }

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(ctx->context, "entry", function);
    ctx->builder.SetInsertPoint(bb);

    ctx->named_values.clear();

    for (auto& arg : function->args()) {
        ctx->named_values[std::string{arg.getName()}] = &arg;
    }

    llvm::Value* generated = Codegen(*function_expr.body, ctx);
    if (!generated) {
        function->eraseFromParent();
        return nullptr;
    }

    ctx->builder.CreateRet(generated);
    if (llvm::verifyFunction(*function)) {
        return LogError("Function verification failed");
    }
    return function;
}

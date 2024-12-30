#pragma once

#include <codegen/codegen_ctx.h>
#include <parser/ast.h>
#include <util.h>

#include <llvm/IR/Verifier.h>

llvm::Value* Codegen(const ast::Number& number, CodegenCtx* ctx);

llvm::Value* Codegen(const ast::Variable& var, CodegenCtx* ctx);

llvm::Value* Codegen(const ast::BinaryOp& op, CodegenCtx* ctx);

llvm::Value* Codegen(const ast::CallExpression& expr, CodegenCtx* ctx);

llvm::Value* Codegen(const ast::Node& node, CodegenCtx* ctx);

llvm::Function* Codegen(const ast::Prototype& proto, CodegenCtx* ctx);

llvm::Function* Codegen(const ast::Function& function_expr, CodegenCtx* ctx);

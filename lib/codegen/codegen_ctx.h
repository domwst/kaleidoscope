#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <map>

class CodegenCtx {
public:
    CodegenCtx(std::string_view name) : context(), module(name, context), builder(context) {
    }

    llvm::LLVMContext context;
    llvm::Module module;
    llvm::IRBuilder<> builder;
    std::map<std::string, llvm::Value*> named_values;
};

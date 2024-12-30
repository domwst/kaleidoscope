#pragma once

#include <parser/parser.h>
#include <codegen/codegen_ctx.h>

class Repl {
public:
    Repl(std::istream* in, llvm::raw_ostream* out, std::map<std::string, uint8_t> binop_precedence);

    void MainLoop();

    static void Prompt(llvm::raw_ostream* out);

private:
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
    llvm::raw_ostream& Out() const;

    Parser parser_;
    CodegenCtx codegen_ctx_;

    llvm::raw_ostream* out_;
};

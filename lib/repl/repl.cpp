#include "repl.h"

#include <parser/token.h>
#include <codegen/codegen.h>
#include <util.h>

using namespace token;

Repl::Repl(std::istream* in, llvm::raw_ostream* out,
           std::map<std::string, uint8_t> binop_precedence)
    : parser_(std::move(binop_precedence), in), codegen_ctx_("my cool jit"), out_(out) {
}

void Repl::Prompt(llvm::raw_ostream* out) {
    (*out) << "ready> ";
}

void Repl::MainLoop() {
    while (true) {
        Prompt(out_);
        switch (GetTokenKind(parser_.GetTokenizer()->Get())) {
            case TokenKind::kEof:
                return;
            case TokenKind::kSemicolon:
                parser_.GetTokenizer()->Next();
                break;
            case TokenKind::kDef:
                HandleDefinition();
                break;
            case TokenKind::kExtern:
                HandleExtern();
                break;
            default:
                HandleTopLevelExpression();
                break;
        }
    }
}

void Repl::HandleDefinition() {
    auto fn = parser_.ParseDefinition();
    if (!fn) {
        LogError("Failed to parse definition");
        parser_.GetTokenizer()->Next();
        return;
    }
    auto* fn_ir = Codegen(*fn, &codegen_ctx_);
    if (!fn_ir) {
        LogError("Failed to codegen");
        return;
    }
    Out() << "Read function definition: ";
    fn_ir->print(Out());
    Out() << '\n';
}

void Repl::HandleExtern() {
    auto fn = parser_.ParseExtern();
    if (!fn) {
        parser_.GetTokenizer()->Next();
        return;
    }
    auto fn_ir = Codegen(*fn, &codegen_ctx_);
    if (!fn_ir) {
        return;
    }
    Out() << "Read extern: ";
    fn_ir->print(Out());
    Out() << '\n';
}

void Repl::HandleTopLevelExpression() {
    auto fn = parser_.ParseTopLevelExpr();
    if (!fn) {
        parser_.GetTokenizer()->Next();
        return;
    }
    auto fn_ir = Codegen(*fn, &codegen_ctx_);
    if (!fn_ir) {
        return;
    }
    Out() << "Read top-level expression: ";
    fn_ir->print(Out());
    Out() << '\n';
    fn_ir->eraseFromParent();
}

llvm::raw_ostream& Repl::Out() const {
    return *out_;
}

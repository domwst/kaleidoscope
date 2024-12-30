#include <repl/repl.h>
#include <backward.hpp>
#include <iostream>

int main() {
    backward::SignalHandling sh;

    std::map<std::string, uint8_t> binop_precedence{
        {"<", 10},
        {"+", 20},
        {"-", 20},
        {"*", 40},
    };
    auto out = &llvm::errs();
    Repl::Prompt(out);
    Repl repl{&std::cin, out, std::move(binop_precedence)};
    repl.MainLoop();
}

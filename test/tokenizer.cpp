#include <gtest/gtest.h>

#include <parser/tokenizer.h>
#include "parser/token.h"

using namespace token;

std::vector<Token> ConsumeTokenizer(Tokenizer* t) {
    std::vector<Token> result;
    while (t->Get() != Token{Eof{}}) {
        result.push_back(t->Get());
        t->Next();
    }
    return result;
}

template <class... Tokens>
void CheckTokenization(std::string s, Tokens&&... tokens) {
    std::stringstream ss{std::move(s)};
    Tokenizer t{&ss};
    auto expected = std::vector<Token>{{Token{std::forward<Tokens>(tokens)}...}};
    auto actual = ConsumeTokenizer(&t);
    ASSERT_EQ(expected, actual);
}

constexpr Bracket kOpen = Bracket{BracketKind::kOpen};
constexpr Bracket kClose = Bracket{BracketKind::kClose};

TEST(Tokenizer, Definition) {
    CheckTokenization("def f(x) x;", Def{}, Ident{"f"}, kOpen, Ident{"x"}, kClose, Ident{"x"},
                      Semicolon{});
    CheckTokenization("def abc(df ghi jkl) df + ghi", Def{}, Ident{"abc"}, kOpen, Ident{"df"},
                      Ident{"ghi"}, Ident{"jkl"}, kClose, Ident{"df"}, Operator{"+"}, Ident{"ghi"});
}

TEST(Tokenizer, Extern) {
    CheckTokenization("extern def f(x)", Extern{}, Def{}, Ident{"f"}, kOpen, Ident{"x"}, kClose);
}

TEST(Tokenizer, Expression) {
    CheckTokenization("(x + 1.0) * 123.456", kOpen, Ident{"x"}, Operator{"+"}, Number{1}, kClose,
                      Operator{"*"}, Number{123.456});
}

TEST(Tokenizer, Misc) {
    CheckTokenization("(((x + 1.5) * y) - .3) / 4.; def f(x)x^*^2;", kOpen, kOpen, kOpen,
                      Ident{"x"}, Operator{"+"}, Number{1.5}, kClose, Operator{"*"}, Ident{"y"},
                      kClose, Operator{"-"}, Number{.3}, kClose, Operator{"/"}, Number{4},
                      Semicolon{}, Def{}, Ident{"f"}, kOpen, Ident{"x"}, kClose, Ident{"x"},
                      Operator{"^*^"}, Number{2}, Semicolon{});
    CheckTokenization("x +;-# y", Ident{"x"}, Operator{"+"}, Semicolon{}, Operator{"-"});
}

TEST(Tokenizer, Comments) {
    CheckTokenization(R"(
        def f(x) #Comment here
            x +#* Would be too much
            42#Good number regardless
            # Some trailing thoughts
    )",
                      Def{}, Ident{"f"}, kOpen, Ident{"x"}, kClose, Ident{"x"}, Operator{"+"},
                      Number{42});
}

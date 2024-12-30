#include "tokenizer.h"

#include <cassert>
#include <cctype>

using namespace token;

namespace {

bool IsOperatorCharacter(char c) {
    constexpr std::string_view forbidden_characters = "(),;#";
    return !std::isspace(c) && !std::isalnum(c) &&
           forbidden_characters.find(c) == std::string_view::npos;
}

}  // namespace

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
    Next();
}

void Tokenizer::Next() {
    SkipSpacesAndComments();
    const auto cur_c = in_->peek();

    if (std::isalpha(cur_c)) {
        std::string ident;
        while (std::isalnum(in_->peek())) {
            ident += in_->get();
        }

        if (ident == "def") {
            cur_token_ = Def{};
            return;
        }
        if (ident == "extern") {
            cur_token_ = Extern{};
            return;
        }
        cur_token_ = Ident{std::move(ident)};
        return;
    }

    if (std::isdigit(cur_c) || cur_c == '.') {
        std::string num;
        while (std::isdigit(in_->peek()) || in_->peek() == '.') {
            num += in_->get();
        }

        cur_token_ = Number{strtod(num.c_str(), nullptr)};
        return;
    }

    if (cur_c == '(') {
        in_->get();
        cur_token_ = Bracket{BracketKind::kOpen};
        return;
    }
    if (cur_c == ')') {
        in_->get();
        cur_token_ = Bracket{BracketKind::kClose};
        return;
    }
    if (cur_c == ',') {
        in_->get();
        cur_token_ = Comma{};
        return;
    }
    if (cur_c == ';') {
        in_->get();
        cur_token_ = Semicolon{};
        return;
    }
    if (cur_c == EOF) {
        cur_token_ = Eof{};
        return;
    }

    std::string op;
    while (IsOperatorCharacter(in_->peek())) {
        op += in_->get();
    }
    assert(!op.empty());

    cur_token_ = Operator{std::move(op)};
    return;
}

const Token& Tokenizer::Get() const {
    return cur_token_;
}

void Tokenizer::SkipSpacesAndComments() {
    while (std::isspace(in_->peek())) {
        in_->get();
    }
    if (in_->peek() != '#') {
        return;
    }
    in_->get();
    while (in_->peek() != '\n' && !in_->eof()) {
        in_->get();
    }
    SkipSpacesAndComments();
}

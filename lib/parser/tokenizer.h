#pragma once

#include "token.h"

#include <istream>

struct Tokenizer {
    Tokenizer(std::istream* in);

    void Next();

    const Token& Get() const;

private:
    void SkipSpacesAndComments();

    std::istream* in_;
    Token cur_token_;
};

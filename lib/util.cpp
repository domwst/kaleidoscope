#include "util.h"

#include <iostream>

std::nullptr_t LogError(std::string_view msg) {
    std::cerr << "Error: " << msg << std::endl;
    return nullptr;
}

#pragma once

#include <cstddef>
#include <string_view>

#define JUST(...) __VA_ARGS__

std::nullptr_t LogError(std::string_view);

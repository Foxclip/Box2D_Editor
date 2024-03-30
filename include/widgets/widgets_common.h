#include <cassert>
#include <string>
#include <iostream>

#ifndef NDEBUG

void _print_msg();
void _print_msg(const std::string& message);

#define wAssert(value, ...) \
	_print_msg(__VA_ARGS__); \
	assert(value);

#else

#define wAssert(value)

#endif // !NDEBUG

#include "widgets/widgets_common.h"

#ifndef NDEBUG

void _print_msg() { }

void _print_msg(const std::string& message) {
	std::cout << message << "\n";
}

#endif // !NDEBUG

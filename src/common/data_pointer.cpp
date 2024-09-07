#include "common/data_pointer.h"

std::string pointer_to_str(void* ptr) {
	ptrdiff_t ptr_address = reinterpret_cast<ptrdiff_t>(ptr);
	std::string str = std::format("0x{:0>16x}", ptr_address);
	return str;
}

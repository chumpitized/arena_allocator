#include <cstring>
#include <iostream>

//Ginger Bill implementation...


static unsigned char arena_buffer[256];
static size_t arena_buffer_length = 256;
static size_t arena_offset = 0;

bool is_power_of_two(uintptr_t x) {
	return (x & (x-1)) == 0;
}

void *arena_alloc(size_t size) {
	// Check to see if the backing memory has space left
	if (arena_offset + size <= arena_buffer_length) {
		void *ptr = &arena_buffer[arena_offset];
		arena_offset += size;

		// Zero out new memory by default
		memset(ptr, 0, size);
		return ptr;
	}
	// Return NULL if the arena is out of memory
	return NULL;
}

int main() {

	void *ptr = arena_alloc(1);
	//memset(ptr, 255, 1);
	*(uint8_t *) ptr = 256;

	uint8_t val = *(uint8_t *)ptr;

	std::cout << "The Value: " << +val << std::endl;
	std::cout << arena_offset << std::endl;
	
	return 1;
}
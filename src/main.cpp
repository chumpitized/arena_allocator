#include <cstring>
#include <iostream>

//Ginger Bill implementation...

static unsigned char *arena_buffer;
static size_t arena_buffer_length;
static size_t arena_offset;

void *arena_alloc(size_t size) {
	// Check to see if the backing memory has space left
	if (arena_offset + size <= arena_buffer_length) {
		void *ptr = &arena_buffer[arena_offset];
		arena_offset += size;

		// Zero new memory by default
		memset(ptr, 0, size);
		return ptr;
	}
	// Return NULL if the arena is out of memory
	return NULL;
}

int main() {

	std::cout << "memory allocator" << std::endl;
	std::cout << 2 * sizeof(void *) << std::endl;
	
	return 1;
}
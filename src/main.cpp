#include <assert.h>
#include <cstring>
#include <iostream>

//Ginger Bill implementation...

static unsigned char arena_buffer[256];
static size_t arena_buffer_length = 256;
static size_t arena_offset = 0;

bool is_power_of_two(uintptr_t x) {
	return (x & (x-1)) == 0;
}

uintptr_t align_forward(uintptr_t ptr, size_t align) {
	assert(is_power_of_two(align));

	uintptr_t p, a, modulo;

	p = ptr;
	a = (uintptr_t)align;
	
	//Example: 
	//p = 0x08; 0b00001000;
	//a = 0x10; 0b00010000;
	//p & (a - 1);
	//=
	//0b00001000 & (0b00001111);
	//=
	//0b00001000;
	//shift forward by 8 bits / 1 byte to align to address 16;
	modulo = p & (a - 1);

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}

void *arena_alloc(size_t size) {	
	if (arena_offset + size <= arena_buffer_length) {		
		void *ptr = &arena_buffer[arena_offset];
		arena_offset += size;

		memset(ptr, 0, size);
		return ptr;
	}

	return NULL;
}

int main() {

	void *ptr = arena_alloc(1);
	//memset(ptr, 255, 1);
	//*(uint8_t *) ptr = 255;
	*(char *) ptr = 'c';

	//uint8_t val = *(uint8_t *)ptr;
	char val = *(char *)ptr;

	//std::cout << "The Value: " << +val << std::endl;
	std::cout << "The Value: " << val << std::endl;
	std::cout << arena_offset << std::endl;
	
	return 1;
}
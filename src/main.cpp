#include <assert.h>
#include <cstring>
#include <iostream>

//Ginger Bill implementation...

typedef struct Arena Arena;
struct Arena {
	unsigned char *buf;
	size_t buf_len;
	size_t prev_offset;
	size_t curr_offset;
};

bool is_power_of_two(uintptr_t x) {
	return (x & (x-1)) == 0;
}

uintptr_t align_forward(uintptr_t ptr, size_t align) {
	assert(is_power_of_two(align));
	uintptr_t p, a, modulo;

	p = ptr;
	a = (uintptr_t)align;
	modulo = p & (a - 1);

	if (modulo != 0) {
		p += a - modulo;
	}
	return p;
}

void *arena_alloc_align(Arena *a, size_t size, size_t align) {
	//get buffer's ptr value and add the current offset's value...
	//this gives the pointer to the current_offset in the arena
	uintptr_t curr_ptr = (uintptr_t)a->buf + (uintptr_t)a->curr_offset;

	//align the curr_ptr forward to alignment boundary
	uintptr_t offset = align_forward(curr_ptr, align);
	offset -= (uintptr_t)a->buf; // Change to relative offset

	if (offset + size <= a->buf_len) {
		void *ptr = &a->buf[offset];
		a->prev_offset = offset;
		a->curr_offset = offset+size;

		memset(ptr, 0, size);
		return ptr;
	}

	return NULL;
}

int main() {

	//void *ptr = arena_alloc(1);
	////memset(ptr, 255, 1);
	////*(uint8_t *) ptr = 255;
	//*(char *) ptr = 'c';

	////uint8_t val = *(uint8_t *)ptr;
	//char val = *(char *)ptr;

	////std::cout << "The Value: " << +val << std::endl;
	//std::cout << "The Value: " << val << std::endl;
	//std::cout << arena_offset << std::endl;
	
	return 1;
}
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

void arena_init(Arena *a, void *backing_buffer, size_t backing_buffer_length) {
	a->buf = (unsigned char *)backing_buffer;
	a->buf_len = backing_buffer_length;
	a->curr_offset = 0;
	a->prev_offset = 0;
}

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

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

void *arena_alloc(Arena *a, size_t size) {
	return arena_alloc_align(a, size, DEFAULT_ALIGNMENT);
}

void *arena_resize_align(Arena *a, void *old_memory, size_t old_size, size_t new_size, size_t align) {
	unsigned char *old_mem = (unsigned char *)old_memory;

	assert(is_power_of_two(align));

	if (old_mem == NULL || old_size == 0) {
		return arena_alloc_align(a, new_size, align);
	} 
	
	if (a->buf <= old_mem && old_mem < a->buf + a->buf_len) {
		if (a->buf + a->prev_offset == old_mem) {
			if (new_size > old_size) {
				memset(&a->buf[a->curr_offset], 0, new_size - old_size);
			}
			a->curr_offset = a->prev_offset + new_size;
			return old_memory;
		} else {
			void *new_memory = arena_alloc_align(a, new_size, align);
			size_t copy_size = old_size < new_size ? old_size : new_size;

			memmove(new_memory, old_memory, copy_size);
			return new_memory;
		}

	} else {
		assert(0 && "Memory is out of bounds of the buffer in this arena");
		return NULL;
	}
}

void arena_free_all(Arena *a) {
	a->curr_offset = 0;
	a->prev_offset = 0;
}

// Because C doesn't have default parameters
void *arena_resize(Arena *a, void *old_memory, size_t old_size, size_t new_size) {
	return arena_resize_align(a, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);
}

int main() {

	//unsigned char *backing_buffer[256];
	//Arena *a = {0};
	//arena_init(a, backing_buffer, 256);
	
	return 1;
}
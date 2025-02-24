#include "raylib.h"
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
	a->buf			= (unsigned char *)backing_buffer;
	a->buf_len 		= backing_buffer_length;
	a->curr_offset 	= 0;
	a->prev_offset 	= 0;
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

void step_through_allocations(Arena *a, int &ap, int &alignment) {
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {

		if (ap == 0) {
			void *ptr = arena_alloc_align(a, sizeof(int), alignment);
			*(int *)ptr = 4;
		}

		if (ap == 1) {
			void *ptr = arena_alloc_align(a, sizeof(int), alignment);
			*(int *)ptr = 255;
		}

		if (ap == 2) {
			void *ptr = arena_alloc_align(a, sizeof(char), alignment);
			*(char *)ptr = 'c';
		}

		if (ap == 3) {
			void *ptr = arena_alloc_align(a, sizeof(char), alignment);
			*(char *)ptr = 'd';
		}

		if (ap == 4) {
			void *ptr = arena_alloc_align(a, sizeof(double), alignment);
			*(double *)ptr = 5.0;
		}

		if (ap == 5) {
			void *ptr = arena_alloc_align(a, sizeof(char), alignment);
			*(char *)ptr = 'd';
		}

		ap++;
	}
}

void draw_allocation_selector(int x, int y) {
	Vector2 v1 = Vector2{x - 10, y};
	Vector2 v2 = Vector2{x - 25, y - 10};
	Vector2 v3 = Vector2{x - 25, y + 10};
	
	DrawTriangle(v1, v2, v3, WHITE);
}

void draw_allocations(int x, int y, int font_size, int ap) {
	Color color;
	const char* allocation;
	for (int i = 0; i < 6; ++i) {
		if (i < ap) color = GREEN;
		else color = RED;

		int y_offset 	= y + ((font_size + 10) * i);
		int y_arrow		= y_offset + (font_size / 2);

		if (i == 0) allocation = "push int 4";
		if (i == 1) allocation = "push int 255";
		if (i == 2) allocation = "push char c";
		if (i == 3) allocation = "push char d";
		if (i == 4) allocation = "push double 5.0";
		if (i == 5) allocation = "push char d";

		DrawText(allocation, x, y_offset, font_size, color);
		if (i == ap) draw_allocation_selector(x, y_arrow);
	}
}

void reset_allocations(Arena *a, int &ap) {
	if (IsKeyPressed(KEY_R)) {
		arena_free_all(a);
		ap = 0;
	}
}

int main() {
	unsigned char backing_buffer[256];
	Arena a = {0};
	arena_init(&a, backing_buffer, 256);

	int window_size 		= 1000;
	int mem_size 			= 20;
	int row_size 			= 16;

	int x_arena 			= (window_size - (row_size * mem_size)) / 4;
	int y_arena 			= (window_size - (row_size * mem_size)) / 2;

	int x_allocs 			= x_arena + 400;
	int y_allocs 			= y_arena;

	int allocation_pointer 	= 0;
	int alignment 			= 8;

	InitWindow(window_size, window_size, "Arena Allocator");

	while (!WindowShouldClose()) {

		//Input
		step_through_allocations(&a, allocation_pointer, alignment);
		reset_allocations(&a, allocation_pointer);

		BeginDrawing();
			ClearBackground(BLACK);

			DrawText("Arena", x_arena, y_arena - 50, 40, WHITE);
			DrawText("Allocations", x_allocs, y_allocs - 50, 40, WHITE);
			draw_allocations(x_allocs, y_allocs, 30, allocation_pointer);
			//draw_allocation_selector(x_allocs, y_allocs);

			for (int i = 0; i < 256; ++i) {
				float rec_size = 20;

				float x = (i % 16) * rec_size;
				float y = (i / 16) * rec_size;

				Rectangle mem 	= Rectangle{x + x_arena, y + y_arena, rec_size, rec_size};
				Color mem_color = i < a.curr_offset ? GREEN : RED;

				DrawRectangleRec(mem, mem_color);
				DrawRectangleLinesEx(mem, 2, BLACK);
			}

		EndDrawing();

	}
	
	return 1;
}
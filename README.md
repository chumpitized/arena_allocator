# Arena Allocator

<img src="https://github.com/user-attachments/assets/434737fd-37e3-4dc8-872e-1315680d6e3a" alt="Alt Text" width="550" height="338">

This is a very simple arena allocator matching Ginger Bill's implementation on his [blog](https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/). I added a goofy little visualization to justify allocating things.

## Explanation
In general, the pattern of `malloc`ing and `free`ing every dynamic allocation can be burdensome to manage. The arena offers a simple solution to this problem. Here's how it works:

1. Create an `Arena` struct containing, at minimum, a backing buffer for storage and two variables for length and offset.
2. Create a function to align allocations at some memory boundary. On x64, this alignment could be 8 bytes (the size of `void *`) or higher (e.g., if you're using SIMD), but should not be less than the `word` size of your system. When data is pushed to the arena, padding will be added between the previous and current allocation to align the new data at the given boundary.
3. Return the allocation pointer. All you have to do is guarantee that pushed, properly aligned data will not exceed the arena size. Simply return a pointer to the current offset and increment the offset. As a bonus, you can use `memset` to zero the allocated memory.
4. Allocate your data. 

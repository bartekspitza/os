#include <stdint.h>

/**
 * The address of the end label (in kernel.ld) calculated by the linker.
 * Do NOT use `extern char* end;` as C generates different instructions when used
 * in this context. If char* is used, end will contain whatever is at end..end+8 bytes,
 * i.e. garbage, not what we want. We want the address of the end label itself, so [] must be used.
 */
extern char end[]; // the end label from kernel.ld

/**
 * A page
 */
struct run {
    struct run *next;
};

/**
 * The pointer to the head of the single linked list.
 * `struct run` is used here as declaring a struct (as in above) only declares a struct tag,
 * not a type in itself, the type of above struct IS 'struct run'.
 */
struct run *freelist;

/**
 * Free a page by making the first 8 bytes point to the current freelist, which may or may not be
 * an unallocated page, then set the freelist head to this page.
 * 
 * @param pa The physical address of the page to free
 */
void kfree(void* pa) {
    struct run* r = (struct run*) pa;
    r->next = freelist;
    freelist = r;
}


/**
 * Initialize the freelist by creating the singly linked list.
 * Since we iterate over memory upwards, the head at the end of this function
 * will point at a page that lives just before the memory end, and allocating pages
 * will actually yield pages at lower addresses. This doesn't matter but interesting to note.
 */
void kinit(void) {

    // Define the memend, assume 128MB
    char* mem_end = (void*) 0x80000000 + (128 * 1024 * 1024);

    char* p = (char*) end;

    for (; p + 4096 <= mem_end; p += 4096) {
        kfree(p);
    }
}

/**
 * Returns a pointer to a fresh page. Because freelist is a global variable,
 * it lives in .bss and is initialized to 0, this means that onces kinit has ran,
 * the very last page (even though this last page actually lives at the earliest memory location, i.e. end + 4096)
 * will have its .next pointer be the original freelist declaration, which is 0, therefore,
 * simply returning this nicely translates to a NULL value.
 */
void* kalloc() {
    struct run* r = freelist;

    // This null check is needed, as if the last page is allocated,
    // freelist will now be 0, NULL, and attempting to set that memory region (freelist->next)
    // would crash the program
    if (r) {
        freelist = freelist->next;
    }
    return (void*) r;
}
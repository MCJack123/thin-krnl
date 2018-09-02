#include "liballoc.h"
#define PAGE_SIZE 512

//unsigned int allocated_page_changes[4096]; // later?
void* base_ptr = (void*)0x40000;
int pages_allocated = 0;

int liballoc_lock() {
    return 0;
}

int liballoc_unlock() {
    return 0;
}

void* liballoc_alloc(int pages) {
    return (void*)&(((char*)base_ptr)[PAGE_SIZE * pages_allocated++]);
}

int liballoc_free(void* block, int pages) {
    return 0;
}
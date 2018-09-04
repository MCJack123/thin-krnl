#include "liballoc.h"
#define PAGE_SIZE 4096

//unsigned int allocated_page_changes[4096]; // later?
void* base_ptr = (void*)0x400000;
int pages_allocated = 0;

int liballoc_lock() {
    return 0;
}

int liballoc_unlock() {
    return 0;
}

void* liballoc_alloc(int pages) {
    if (&(((char*)base_ptr)[PAGE_SIZE * pages_allocated++]) > 0x00F00000 && &(((char*)base_ptr)[PAGE_SIZE * pages_allocated++]) < 0x00FFFFFF) base_ptr = (void*)0x01000000;
    return (void*)&(((char*)base_ptr)[PAGE_SIZE * pages_allocated++]);
}

int liballoc_free(void* block, int pages) {
    return 0;
}
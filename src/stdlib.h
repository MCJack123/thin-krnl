/*
 * stdlib.h
 * Useful functions
 */

#include <liballoc.h>

/* = bool defs = */
#define true 1
#define false 0
typedef _Bool bool;
/* ============= */

// Assembly bridging functions for I/O.
extern unsigned char inb(unsigned short);
extern void outb(unsigned short, unsigned char);
extern void io_wait(void);

// Returns an array of uint32's with the memory size & start position.
extern unsigned int* get_mem_size(void);

// Sleep a number of us. (broken)
//extern void usleep(int);

// Sleep a number of ??.
extern void rsleep(int);

// Copies memory.
extern void memcpy(void*, void*, unsigned int);

// Returns a string with n.
extern const char * itoa(unsigned int);

// Returns a string with the hex of n.
extern const char * htoa(unsigned int);

// Converts a char into a string.
extern const char * ctoa(char);

// Returns the length of str.
extern unsigned int strlen(const char *);

// Combines str1 and str2 and returns the result.
extern const char * strcat(const char *, const char *);

// Gets the offset for the specified x and y coordinates.
extern unsigned int getoff(unsigned int, unsigned int);

// Beep the speaker.
extern void beep(void);
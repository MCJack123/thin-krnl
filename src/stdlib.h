/*
 * stdlib.h
 * Useful functions
 */

#ifndef THINKRNL_STDLIB_H
#define THINKRNL_STDLIB_H
#include <liballoc.h>

/* = bool defs = */
#ifndef __cplusplus
#ifndef _STDLIB_H
#define true 1
#define false 0
typedef _Bool bool;
#endif
#endif
/* ============= */

typedef struct {
    int count;
    char * tokens[];
} string_tokens_t;

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

// Returns an int with the value of the string.
extern int atoi(const char *);

// Returns a string with n.
extern const char * itoa(unsigned int);

// Returns a string with the hex of n.
extern const char * htoa(unsigned int, int);

// Converts a char into a string.
extern const char * ctoa(char);

// Returns the length of str.
extern unsigned int strlen(const char *);

// Combines str1 and str2 and returns the result.
extern const char * strcat(const char *, const char *);

// Compare two strings and return if they are equal.
extern bool strcmp(const char *, const char *);

// Splits a string into an array.
extern string_tokens_t * strtok(const char *, char);

// Gets the offset for the specified x and y coordinates.
extern unsigned int getoff(unsigned int, unsigned int);

// Sound playing utilities.
extern void play_sound(unsigned int nFrequence);
extern void nosound();

// Beep the speaker.
extern void beep(void);

#endif
/*
 * stdio.h
 * Input/output functions
 */

#ifndef THINKRNL_STDIO_H
#define THINKRNL_STDIO_H
#include <stdlib.h>

// Dimensions
#define COLS 80
#define ROWS 25

// PS/2 special keyboard codes.
typedef enum ps2code_ext_t {
    PS2CODE_EXT_ESC = 0x01,
    PS2CODE_EXT_BACKSPACE = 0x0E,
    PS2CODE_EXT_TAB = 0x0F,
    PS2CODE_EXT_ENTER = 0x1C,
    PS2CODE_EXT_LEFT_CONTROL,
    PS2CODE_EXT_LEFT_SHIFT = 0x2A,
    PS2CODE_EXT_RIGHT_SHIFT = 0x36,
    PS2CODE_EXT_LEFT_ALT = 0x38,
    PS2CODE_EXT_CAPS_LOCK = 0x3A,
    PS2CODE_EXT_F1,
    PS2CODE_EXT_F2,
    PS2CODE_EXT_F3,
    PS2CODE_EXT_F4,
    PS2CODE_EXT_F5,
    PS2CODE_EXT_F6,
    PS2CODE_EXT_F7,
    PS2CODE_EXT_F8,
    PS2CODE_EXT_F9,
    PS2CODE_EXT_F10,
    PS2CODE_EXT_NUM_LOCK,
    PS2CODE_EXT_SCROLL_LOCK,
    PS2CODE_EXT_F11 = 0x57,
    PS2CODE_EXT_F12 = 0x58
} ps2code_ext_t;

// Currently set modifiers.
extern char modifiers;

// Modifier masks.
enum modifiers {
    MODIFIER_CONTROL = 2,
    MODIFIER_SHIFT = 1,
    MODIFIER_ALT = 4
};

// Gets a key from the keyboard.
extern char getch(void);

// Gets a scancode from the keyboard.
extern int getkey(void);

// Returns a ps2_ext_t for a PS/2 code.
extern ps2code_ext_t getcode(char);

// Is the converted code special?
extern bool iscode(char);

// Moves the cursor to the offset.
extern void move(int);
// Variation: Automatically calculates offset from x and y.
extern void movec(int, int);

// Clears the screen.
extern void clear(void);

// Scrolls the screen a number of lines.
extern void scroll(int);

// Prints a string on the screen.
extern void print(const char *);
// Variation: Print at specified offset. Returns the next offset.
extern unsigned int printm(const char *, unsigned int);
// Variation: Prints an error in red.
extern void error(const char *);
// Variation: Prints a warning in yellow.
extern void warn(const char *);

// Reads a line of text at offset.
extern const char * scanl();
// Variation: Prints prompt before reading.
extern const char * scanlp(const char *);
// Variation: Start at offset.
extern const char * scanlm(unsigned int);
extern const char * scanlmp(unsigned int, const char *);

#endif
/*
 * stdio.c
 * Input/output functions (implementation)
 */

#include <stdio.h>

const char* ps2codes_sc1 = "\0\0321234567890-=\032\032qwertyuiop[]\032\032asdfghjkl;'`\032\\zxcvbnm,./\032\0\032 \032\032\032\032\032\032\032\032\032\032\032\032\032\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\032\032";
const char* ps2codes_sc1_upper = "\0\032!@#$%^&*()_+\032\032QWERTYUIOP{}\032\032ASDFGHJKL:\"~\032|ZXCVBNM<>?\032\0\032 \032\032\032\032\032\032\032\032\032\032\032\032\032\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\032\032";
const char* ps2codes_sc2 = "\0\032\0\032\032\032\032\032\0\032\032\032\032\032`\0\0\032\032\0\032q1\0\0zsaw2\0\0cxde43\0\0 vftr5\0\0nbhgy6\0\0\0mju78\0\0,kio09\0\0./l;p-\0\0\0'\0[=\0\0\032\032\032]\0\\\0\0\0\0\0\0\0\0\032\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\032\032\032\0\0\0\0\0\032\0\0\0\0\032";
char modifiers = 0;
int current_offset = 0;
char current_settings = 0x07;

char convert_ps2_code(unsigned char code) {
    ps2code_ext_t c;
    if (code == 0xAA || code == 0xB6) {
        modifiers &= ~MODIFIER_SHIFT;
        return 0;
    }
    if (code == 0x9D) {
        modifiers &= ~MODIFIER_CONTROL;
        return 0;
    }
    if (code == 0xB8) {
        modifiers &= ~MODIFIER_ALT;
        return 0;
    }
    if (code > 0x58 || ps2codes_sc1[code] == 0) return 0;
    if (ps2codes_sc1[code] == '\032') {
        c = getcode(code);
        if (c == PS2CODE_EXT_LEFT_SHIFT || c == PS2CODE_EXT_RIGHT_SHIFT) modifiers |= MODIFIER_SHIFT;
        else if (c == PS2CODE_EXT_LEFT_CONTROL) modifiers |= MODIFIER_CONTROL;
        else if (c == PS2CODE_EXT_LEFT_ALT) modifiers |= MODIFIER_ALT;
        else if (c == PS2CODE_EXT_CAPS_LOCK) modifiers = (modifiers & ~MODIFIER_SHIFT) | !(modifiers & MODIFIER_SHIFT);
        else return code | 0x80;
        return 0;
    }
    return modifiers & MODIFIER_SHIFT ? ps2codes_sc1_upper[code] : ps2codes_sc1[code];
}

ps2code_ext_t getcode(char code) {
    return (ps2code_ext_t)(code & 0x7f);
}

bool iscode(char code) {
    return code & 0x80;
}

// old
unsigned char get_scancode() {
    while (!(inb(0x64) & 1));
    return inb(0x60);
}

/*unsigned char get_scancode() {
    unsigned char c=0;
    do {
        if(inb(0x60)!=c) {
            c=inb(0x60);
            if(c>0)
                return c;
        }
    } while(1);
}*/

char getch() {
    return convert_ps2_code(get_scancode());
}

unsigned int getoff(unsigned int x, unsigned int y) {
    return ((y * 80) + x) * 2;
}

void move(int pos) {
    pos /= 2;
    outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char) ((pos >> 8) & 0xFF));
    current_offset = pos;
}

void movec(int x, int y) {
	move(getoff(x, y) / 2);
}

void clear(void) {
    char *vidptr = (char*)0xb8000; 	//video mem begins here.
    unsigned int j = 0;
    /* this loops clears the screen
	* there are 25 lines each of 80 columns; each element takes 2 bytes */
	while(j < 80 * 25 * 2) {
		/* blank character */
		vidptr[j] = ' ';
		/* attribute-byte - light grey on black screen */
		vidptr[j+1] = 0x07; 		
		j = j + 2;
	}
    move(0);
    return;
}

void scroll(int lines) {
    char *vidptr = (char*)0xb8000; 	//video mem begins here.
    int i, j;
    for (i = lines; i < 25; i++) {
        for (j = 0; j < 80; j++) {
            vidptr[(((i-lines)*80)+j)*2] = vidptr[((i*80)+j)*2];
            vidptr[(((i-lines)*80)+j)*2+1] = vidptr[((i*80)+j)*2+1];
        }
    }
    for (; i < lines + 25; i++) {
        for (j = 0; j < 80; j++) {
            vidptr[(((i-lines)*80)+j)*2] = 0;
            vidptr[(((i-lines)*80)+j)*2+1] = 0x07;
        }
    }
    current_offset -= 160;
}

unsigned int printm(const char *str, unsigned int i) {
    char *vidptr = (char*)0xb8000; 	//video mem begins here.
	unsigned int j = 0;

	/* this loop writes the string to video memory */
	while(str[j] != '\0') {
		/* the character's ascii */
		vidptr[i] = str[j];
		/* attribute-byte: give character black bg and light grey fg */
		vidptr[i+1] = current_settings;
		++j;
		i = i + 2;
	}
    move(i);
	return i;
}

void print(const char * str) {
    char *vidptr = (char*)0xb8000; 	//video mem begins here.
	unsigned int j = 0;
    char control = 0;
    //char params[3];
    char color_code;

	/* this loop writes the string to video memory */
	while(str[j] != '\0') {
        //if (current_offset % 2 != 0) {
        //    beep();
        //}
        if (current_offset > (int)vidptr + (80 * 25 * 2)) scroll(1);
        if (control > 0) {
            /* ansi (not done) *
            if (control == 1) {
                switch (str[j]) {
                    case '[':
                        control = 2;
                        break;
                    case 'c':
                        current_settings = 0x07;
                    default:
                        control = 0;
                        break;
                }
            } else {
                params[control++-2] = str[j];
                if (control == 5) {

                } 
            }
            /* */
            /* colors only */
            // use 0-7 for dark, 8-f for light
            // first is background, second is foreground
            if (str[j] >= '0' && str[j] <= '9') color_code = str[j] - '0';
            else if (str[j] >= 'a' && str[j] <= 'f') color_code = str[j] - 'a' + 10;
            else if (str[j] >= 'A' && str[j] <= 'F') color_code = str[j] - 'A' + 10;
            else if ((str[j] == 's' || str[j] == 'S') && control == 1) {control = 2; j++; continue;}
            else {control = 0; j++; continue;}
            if (control++ == 1) current_settings = (current_settings & 0x0F) | (color_code << 4);
            else {current_settings = (current_settings & 0xF0) | color_code; control = 0;}
            /* */
        } else {
            if (str[j] == '\n') 
                current_offset = (current_offset / 80) * 80 + 79;
            else if (str[j] == '\0') current_offset -= 1;
            else if (str[j] == '\b') current_offset -= 2;
            else if (str[j] == '\r') current_offset = (current_offset / 80) * 80 - 1;
            else if (str[j] == '\a') beep();
            else if (str[j] == '\t') current_offset += 3;
            else if (str[j] == 0x1b) {control = 1; j++; continue;}
            else vidptr[current_offset*2] = str[j];
            /* attribute-byte: give character black bg and light grey fg */
            vidptr[current_offset*2+1] = current_settings;
            current_offset++;
        }
		++j;
	}
    move(current_offset*2);
}

const char * scanlm(unsigned int offset) {
    char ch = 0;
    ps2code_ext_t code;
    int len = 0;
    int size = 16;
    char* backup;
    char* buffer = (char*)malloc(16);
    buffer[0] = '\0';
    while (true) {
        ch = getch();
        if (ch == 0) continue;
        if (iscode(ch)) {
            code = getcode(ch);
            if (code == PS2CODE_EXT_BACKSPACE) {
                if (len > 0) {
                    buffer[--len] = '\0';
                    printm(" ", offset + ((len) * 2));
                    move(offset + ((len) * 2));
                } else beep();
            } else if (code == PS2CODE_EXT_ENTER) {
                return (const char *)buffer;
            } else {
                //print(htoa(code), offset + (len * 2));
            }
        } else {
            buffer[len++] = (char)ch;
            if (len >= size) {
                backup = malloc(size);
                memcpy((void*)backup, (void*)buffer, size);
                buffer = malloc(size + 16);
                memcpy((void*)buffer, (void*)backup, size);
                size += 16;
            }
            buffer[len] = '\0';
            printm(ctoa(ch), offset + ((len-1) * 2));
        }
    }
}

const char * scanlmp(unsigned int offset, const char * prompt) {
    return scanl(printm(prompt, offset));
}

const char * scanl() {
    char ch = 0;
    ps2code_ext_t code;
    int len = 0;
    int size = 16;
    char* backup;
    char* buffer = (char*)malloc(16);
    buffer[0] = '\0';
    //rsleep(200000000);
    while (true) {
        ch = getch();
        if (ch == 0) continue;
        if (iscode(ch)) {
            code = getcode(ch);
            if (code == PS2CODE_EXT_BACKSPACE) {
                if (len > 0) {
                    buffer[--len] = '\0';
                    print("\b \b");
                } else beep();
            } else if (code == PS2CODE_EXT_ENTER) {
                print("\n");
                return (const char *)buffer;
            } else {
                //print(htoa(code), offset + (len * 2));
            }
        } else {
            buffer[len++] = (char)ch;
            if (len >= size) {
                backup = malloc(size);
                memcpy((void*)backup, (void*)buffer, size);
                buffer = malloc(size + 16);
                memcpy((void*)buffer, (void*)backup, size);
                size += 16;
            }
            buffer[len] = '\0';
            print(ctoa(ch));
        }
        //rsleep(200000000);
    }
}

const char * scanlp(const char * prompt) {
    print(prompt);
    return scanl();
}
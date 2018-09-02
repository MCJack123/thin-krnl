/*
 * stdlib.c
 * Useful functions (implementation)
 */

#include "stdlib.h"
#define hnib(n) (n & 0xF0) >> 4
#define lnib(n) n & 0x0F

const char* hexnums = "0123456789abcdef";

void rsleep(int t) {
    for (int i = 0; i < t; i++);
}

unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void outb(unsigned short port, unsigned char val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

void memcpy(void * dest, void * src, unsigned int size) {
    char* csrc = (char*)src;
    char* cdest = (char*)dest;
    int i;
    for (i = 0; i < size; i++) cdest[i] = csrc[i];
}

const char* itoa(unsigned int n) {
    char* buffer;
    int i = 0;

    bool isNeg = n<0;

    unsigned int n1 = isNeg ? -n : n;

    buffer = (char*)malloc(50);

    while(n1!=0)
    {
        buffer[i++] = n1%10+'0';
        n1=n1/10;
    }

    if(isNeg)
        buffer[i++] = '-';

    buffer[i] = '\0';

    for(int t = 0; t < i/2; t++)
    {
        buffer[t] ^= buffer[i-t-1];
        buffer[i-t-1] ^= buffer[t];
        buffer[t] ^= buffer[i-t-1];
    }

    if(n == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
    } 
    return (const char*)buffer; // unsafe?
}

const char * htoa(unsigned int n) {
    char* bytes = (char*)&n;
    char* buffer;
    int i = 0;
    buffer = (char*)malloc(11);
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[10] = '\0';

    for (i = 0; i < 4; i++) {
        buffer[9-(i*2)] = hexnums[lnib(bytes[i])];
        buffer[8-(i*2)] = hexnums[hnib(bytes[i])];
    }

    return (const char*)buffer;
}

const char * ctoa(char c) {
    char* buf = malloc(2);
    buf[0] = c;
    buf[1] = '\0';
    return (const char*)buf;
}

unsigned int strlen(const char * str) {
    unsigned int retval = 0;
    while (str[++retval] != '\0') {}
    return retval-2;
}

const char* strcat(const char * str1, const char * str2) {
    unsigned int str1len = 0;
    unsigned int str2len = 0;
    unsigned int i = 0;
    char* buffer;

    str1len = strlen(str1);
    str2len = strlen(str2);

    buffer = (char*)malloc(str1len + str2len - 1);
    for (i = 0; i < str1len; i++) buffer[i] = str1[i];
    for (; i < str1len + str2len; i++) buffer[i] = str2[i-str1len];
    buffer[i] = '\0';

    return (const char*)buffer; // unsafe?
}
//Play sound using built in speaker
static void play_sound(unsigned int nFrequence) {
	unsigned int Div;
	unsigned char tmp;

       //Set the PIT to the desired frequency
	Div = 1193180 / nFrequence;
	outb(0x43, 0xb6);
	outb(0x42, (unsigned char) (Div) );
	outb(0x42, (unsigned char) (Div >> 8));

       //And play the sound using the PC speaker
	tmp = inb(0x61);
 	if (tmp != (tmp | 3)) {
		outb(0x61, tmp | 3);
	}
}

//make it shutup
static void nosound() {
	unsigned char tmp = inb(0x61) & 0xFC;
	outb(0x61, tmp);
}
 
//Make a beep
void beep() {
    play_sound(500);
    rsleep(5000000);
	nosound();
    //set_PIT_2(old_frequency);
}

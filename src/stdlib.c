/*
 * stdlib.c
 * Useful functions (implementation)
 */

#include <stdlib.h>
#define hnib(n) (n & 0xF0) >> 4
#define lnib(n) n & 0x0F

const char* hexnums = "0123456789abcdef";

void rsleep(int t) {
    int i;
    beep();
    for (i = 0; i < t; i++);
}

void rnsleep(int t) {
    int i;
    for (i = 0; i < t; i++);
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

void io_wait(void) {
    /* TODO: This is probably fragile. */
    asm volatile ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}

void memcpy(void * dest, void * src, unsigned int size) {
    char* csrc = (char*)src;
    char* cdest = (char*)dest;
    int i;
    for (i = 0; i < size; i++) cdest[i] = csrc[i];
}

char htoi(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    beep();
    return 0;
}

int atoi(const char * str) {
    int res = 0; // Initialize result
    int i;
    if (str[0] == '0' && str[1] == 'x') {
        for (i = 2; str[i] != '\0'; i++) 
            res = (res << 4) + htoi(str[i]);
        return res;
    } else {
    
        // Iterate through all characters of input string and
        // update result
        for (i = str[0] == '-'; str[i] != '\0'; ++i)
            res = res*10 + str[i] - '0';
    
        // return result.
        return res * (str[0] == '-' ? -1 : 1);
    }
}

const char* itoa(unsigned int n) {
    char* buffer;
    int i = 0;
    int t;

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

    for(t = 0; t < i/2; t++)
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

const char * htoa(unsigned int n, int z) {
    char* bytes = (char*)&n;
    char* buffer;
    int i = 0;
    buffer = (char*)malloc(z*2+3);
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[z*2+2] = '\0';

    for (i = 0; i < z; i++) {
        buffer[z*2+1-(i*2)] = hexnums[lnib(bytes[i])];
        buffer[z*2-(i*2)] = hexnums[hnib(bytes[i])];
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
    return retval;
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

bool strcmp(const char * str1, const char * str2) {
    int i, len = strlen(str1);
    if (len != strlen(str2)) return false;
    for (i = 0; i < len; i++) if (str1[i] != str2[i]) return false;
    return true;
}

string_tokens_t * strtok(const char * str, char delim) {
    string_tokens_t * retval;
    int token_lengths[256]; // if there's more the pointer will just overflow; unsafe but should work
    int j, offset = 0;
    int i;
    retval->count = 0;
    token_lengths[0] = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == delim) token_lengths[++retval->count] = 0;
        else token_lengths[retval->count]++;
    }
    retval->count++;
    for (j = 0; j < retval->count; j++) {
        retval->tokens[j] = (char*)malloc(token_lengths[j]);
        memcpy(retval->tokens[j], &str[offset], token_lengths[j]);
        retval->tokens[j][token_lengths[j]] = '\0';
        offset += token_lengths[j] + 1;
    }
    return retval;
}

//Play sound using built in speaker
void play_sound(unsigned int nFrequence) {
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
void nosound() {
	unsigned char tmp = inb(0x61) & 0xFC;
	outb(0x61, tmp);
}
 
//Make a beep
void beep() {
    play_sound(500);
    rnsleep(5000000);
	nosound();
    //set_PIT_2(old_frequency);
}

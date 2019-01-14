#include <interpret.h>
#include <drivers/ata.h>
#include <hexedit.h>
#define SYMBOLARR_SIZE 36
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#ifdef ENVIRONMENT32
typedef unsigned int ptr_t;
#else
typedef unsigned long long ptr_t; // for intellisense since it default checks on 64-bit
#endif

extern void callC(ptr_t addr, int argc, char * argv);
extern void kmain(void);
extern char current_settings;

void print_hex_str(char * buf, unsigned int size) {
    unsigned int * buff = (unsigned int*)buff;
    unsigned int i;
    for (i = 0; i < size / 4; i++) {
        print(htoa(buff[i], 4));
        print(" ");
    }
}

const char * call_symbols_keys[SYMBOLARR_SIZE] = {
    "ata_read_sectors",
    "ata_soft_reset",
    "atoi",
    "beep",
    "callC",
    "clear",
    "ctoa",
    "getch",
    "getkey",
    "get_mem_size",
    "getoff",
    "htoa",
    "inb",
    "io_wait",
    "iscode",
    "itoa",
    "kmain",
    "memcpy",
    "move",
    "movec",
    "nosound",
    "outb",
    "play_sound",
    "print",
    "print_hex_str",
    "printm",
    "rsleep",
    "scanl",
    "scanlm",
    "scanlmp",
    "scanlp",
    "scroll",
    "strcat",
    "strcmp",
    "strlen",
    "strtok"
};

ptr_t call_symbols_values[SYMBOLARR_SIZE] = {
    (ptr_t)ata_read_sectors,
    (ptr_t)ata_soft_reset,
    (ptr_t)atoi,
    (ptr_t)beep,
    (ptr_t)callC,
    (ptr_t)clear,
    (ptr_t)ctoa,
    (ptr_t)getch,
    (ptr_t)getkey,
    (ptr_t)get_mem_size,
    (ptr_t)getoff,
    (ptr_t)htoa,
    (ptr_t)inb,
    (ptr_t)io_wait,
    (ptr_t)iscode,
    (ptr_t)itoa,
    (ptr_t)kmain,
    (ptr_t)memcpy,
    (ptr_t)move,
    (ptr_t)movec,
    (ptr_t)nosound,
    (ptr_t)outb,
    (ptr_t)play_sound,
    (ptr_t)print,
    (ptr_t)print_hex_str,
    (ptr_t)printm,
    (ptr_t)rsleep,
    (ptr_t)scanl,
    (ptr_t)scanlm,
    (ptr_t)scanlmp,
    (ptr_t)scanlp,
    (ptr_t)scroll,
    (ptr_t)strcat,
    (ptr_t)strcmp,
    (ptr_t)strlen,
    (ptr_t)strtok
};

ptr_t map_find(const char * key) {
    int i;
    for (i = 0; i < SYMBOLARR_SIZE; i++)
        if (strcmp(key, call_symbols_keys[i])) 
            return call_symbols_values[i];
    return 0;
}

int last_return = 0;

bool run_command(const char * command) {
    string_tokens_t * tok = strtok(command, ' ');
    if (tok->count == 0 || strcmp(tok->tokens[0], "")) return false;
    if (strcmp(tok->tokens[0], "help")) last_return = command_help();
    else if (strcmp(tok->tokens[0], "retval")) last_return = command_retval();
    else if (strcmp(tok->tokens[0], "symbols")) last_return = command_symbols();
    else if (strcmp(tok->tokens[0], "echo")) last_return = command_echo(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "poke")) last_return = command_poke(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "peek")) last_return = command_peek(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "call")) last_return = command_call(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "hexedit")) last_return = command_hexedit(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "ata_read")) last_return = command_ata_read(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "reboot")) callC(0xffff0, 0x00, 0x00);
    else if (strcmp(tok->tokens[0], "exit")) return true;
    else {error("Error: Unknown command "); error(tok->tokens[0]); print("\n"); last_return = -1; return false;}
    return false;
}

int command_help() {
    print("Commands available to use:\n"\
"\tata_read <sectors> <lba> [h]: Read number of sectors starting at lba\n"\
"\tpoke <addr|'settings'> <val>: Set a memory address to a value\n"\
"\tpeek <addr|'settings'>: Get the value of the address\n"\
"\tcall <addr|*symbol> [<d|w|b|s>arg1] [<d|w|b|s>args...]: Call addr or symbol with arguments\n"\
"\techo <text...>: Print the text to the screen\n"\
"\thexedit <address>: Edits memory starting at an address\n"\
"\tsymbols: Show symbols available to call\n"\
"\tretval: Display last return value\n"\
"\thelp: Display this help\n"\
"\texit: Shutdown the OS\n"\
"\treboot: Restart the computer\n");
    return 0;
}

int command_retval() {
    print(itoa(last_return));
    print("\n");
    return last_return;
}

int command_symbols() {
    int i;
    print("Symbols available: ");
    for (i = 0; i < SYMBOLARR_SIZE; i++) {
        print(call_symbols_keys[i]);
        print(" ");
    }
    print("\n");
    return 0;
}

int command_echo(int argc, const char * argv[]) {
    int i;
    for (i = 1; i < argc; i++) {print(argv[i]); print(" ");}
    print("\n");
    return 0; 
}

int command_ata_read(int argc, const char * argv[]) {
    void * buf;
    int sectors, lba;
    if (argc < 3) {
        error("Error: Too few arguments\n");
        return 1;
    }
    sectors = atoi(argv[1]);
    lba = atoi(argv[2]);
    buf = malloc(sectors*512);
    ata_soft_reset();
    ata_read_sectors(false, (unsigned char)sectors, lba, (unsigned short *)buf);
    if (argc > 3 && argv[3][0] == 'h') print_hex_str((char*)buf, sectors*512);
    else print((const char*)buf);
    print("\n");
    return 0;
}

int command_poke(int argc, const char * argv[]) {
    char * addr;
    char val;
    if (argc < 3) {
        error("Error: Too few arguments\n");
        return 1;
    }
    if (strcmp(argv[1], "settings")) addr = &current_settings;
    else addr = (char*)atoi(argv[1]);
    val = atoi(argv[2]) % 256;
    addr[0] = val;
    return 0;
}

int command_peek(int argc, const char * argv[]) {
    char * addr;
    if (argc < 2) {
        error("Error: Too few arguments\n");
        return 1;
    }
    if (strcmp(argv[1], "settings")) addr = &current_settings;
    else addr = (char*)atoi(argv[1]);
    print(htoa(addr[0], 1));
    print("\n");
    return 0;
}

int command_call(int argc, const char * argv[]) {
    char * args;
    int total = 0;
    int i = 0;
    ptr_t addr;
    if (argc < 2) {
        error("Error: Too few arguments\n");
        return 1;
    }
    if (argv[1][0] == '*') {
        addr = map_find(&argv[1][1]);
        if (addr == 0) {
            error("Error: Invalid symbol\n");
            return 3;
        }
    } else addr = (ptr_t)atoi(argv[1]);
    for (i = 2; i < argc; i++) {
        if (argv[i][0] == 'd') total += 4;
        else if (argv[i][0] == 'w') total += 2;
        else if (argv[i][0] == 'b') total += 1;
        else if (argv[i][0] == 's') total += 4;
        else {
            error("Error: Invalid type specifier at ");
            error(argv[i]);
            error(". Expected d/w/b/s.\n");
            return 2;
        }
    }
    args = (char*)malloc(total);
    total = 0;
    for (i = 2; i < argc; i++) {
        if (argv[i][0] == 'd') {
            unsigned int * a = (unsigned int *)(&args[total]);
            a[0] = (unsigned int)atoi(&argv[i][1]);
            total += 4;
        } else if (argv[i][0] == 'w') {
            unsigned short * a = (unsigned short *)(&args[total]);
            a[0] = (unsigned short)atoi(&argv[i][1]);
            total += 2;
        } else if (argv[i][0] == 'b') {
            args[total] = (unsigned char)atoi(&argv[i][1]);
            total += 1;
        } else {
            unsigned int * a = (unsigned int *)(&args[total]);
            a[0] = (unsigned int)&argv[i][1];
            total += 4;
        }
    }
    //print(htoa(addr, 4));
    callC(addr, total, args);
    return 0;
}

int command_hexedit(int argc, const char * argv[]) {
    unsigned int offset;
    if (argc < 2) {
        error("Error: Too few arguments\n");
    }
    offset = (unsigned int)atoi(argv[1]);
    memedit((char*)offset);
    return 0;
}


#include <interpret.h>
#include <drivers/ata.h>

extern void callC(unsigned int addr, char * argv);

int last_return = 0;

void print_hex_str(char * buf, unsigned int size) {
    unsigned int * buff = (unsigned int*)buff;
    for (unsigned int i = 0; i < size / 4; i++) {
        print(htoa(buff[i], 4));
        print(" ");
    }
}

bool run_command(const char * command) {
    string_tokens_t * tok = strtok(command, ' ');
    if (tok->count == 0) return false;
    if (strcmp(tok->tokens[0], "help")) last_return = command_help();
    else if (strcmp(tok->tokens[0], "retval")) last_return = command_retval();
    else if (strcmp(tok->tokens[0], "echo")) last_return = command_echo(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "poke")) last_return = command_poke(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "peek")) last_return = command_peek(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "call")) last_return = command_call(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "ata_read")) last_return = command_ata_read(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "reboot")) callC(0xffff0, 0x00);
    else if (strcmp(tok->tokens[0], "exit")) return true;
    else {print("Error: Unknown command "); print(tok->tokens[0]); print("\n"); last_return = -1; return false;}
    return false;
}

int command_help() {
    print("Commands available to use:\n"\
"\tata_read <sectors> <lba> [h]: Read number of sectors starting at lba\n"\
"\tpoke <addr> <val>: Set a memory address to a value\n"\
"\tpeek <addr>: Get the value of the address\n"\
"\tcall <addr> [<d|w|b|s>arg1] [<d|w|b|s>args...]: Call addr with arguments\n"\
"\techo <text...>: Print the text to the screen\n"\
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

int command_echo(int argc, const char * argv[]) {
    for (int i = 1; i < argc; i++) {print(argv[i]); print(" ");}
    print("\n");
    return 0; 
}

int command_ata_read(int argc, const char * argv[]) {
    void * buf;
    int sectors, lba;
    if (argc < 3) {
        print("Error: Too few arguments\n");
        return 1;
    }
    sectors = atoi(argv[1]);
    lba = atoi(argv[2]);
    buf = malloc(sectors*512);
    ata_lba_read(buf, (unsigned short)sectors, (unsigned int)lba);
    if (argc > 3 && argv[3][0] == 'h') print_hex_str((char*)buf, sectors*512);
    else print((const char*)buf);
    print("\n");
    return 0;
}

int command_poke(int argc, const char * argv[]) {
    char * addr;
    char val;
    if (argc < 3) {
        print("Error: Too few arguments\n");
        return 1;
    }
    addr = (char*)atoi(argv[1]);
    val = atoi(argv[2]) % 256;
    addr[0] = val;
    return 0;
}

int command_peek(int argc, const char * argv[]) {
    char * addr;
    if (argc < 2) {
        print("Error: Too few arguments\n");
        return 1;
    }
    addr = (char*)atoi(argv[1]);
    print(htoa(addr[0], 1));
    print("\n");
    return 0;
}

int command_call(int argc, const char * argv[]) {
    char * args;
    int total = 0;
    int i = 0;
    unsigned int addr;
    if (argc < 2) {
        print("Error: Too few arguments\n");
        return 1;
    }
    addr = (unsigned int)atoi(argv[1]);
    for (i = 2; i < argc; i++) {
        if (argv[i][0] == 'd') total += 4;
        else if (argv[i][0] == 'w') total += 2;
        else if (argv[i][0] == 'b') total += 1;
        else if (argv[i][0] == 's') total += 4;
        else {
            print("Error: Invalid type specifier at ");
            print(argv[i]);
            print(". Expected d/w/b/s.\n");
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
            a[0] = (unsigned int)argv[i];
            total += 4;
        }
    }
    //print(htoa(addr, 4));
    callC(addr, args);
    return 0;
}


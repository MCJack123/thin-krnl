#include <interpret.h>
#include <drivers/ata.h>

int last_return = 0;

void print_hex_str(char * buf, unsigned int size) {
    unsigned int * buff = (unsigned int*)buff;
    for (unsigned int i = 0; i < size / 4; i++) {
        print(htoa(buff[i]));
        print(" ");
    }
}

bool run_command(const char * command) {
    string_tokens_t * tok = strtok(command, ' ');
    if (tok->count == 0) return false;
    if (strcmp(tok->tokens[0], "help")) last_return = command_help();
    else if (strcmp(tok->tokens[0], "retval")) last_return = command_retval();
    else if (strcmp(tok->tokens[0], "echo")) last_return = command_echo(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "ata_read")) last_return = command_ata_read(tok->count, tok->tokens);
    else if (strcmp(tok->tokens[0], "exit")) return true;
    else {print("Error: Unknown command "); print(tok->tokens[0]); print("\n"); last_return = -1; return false;}
    return false;
}

int command_help() {
    print("Commands available to use:\n\tata_read <sectors> <lba> [h]: Read number of sectors starting at lba\n\techo <text...>: Print the text to the screen\n\tretval: Display last return value\n\thelp: Display this help\n\texit: Shutdown the OS\n");
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
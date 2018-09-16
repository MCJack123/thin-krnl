#ifndef THINKRNL_INTERPRET_H
#define THINKRNL_INTERPRET_H
#include <stdio.h>

extern bool run_command(const char * command);
extern int command_help();
extern int command_retval();
extern int command_echo(int argc, const char * argv[]);
extern int command_ata_read(int argc, const char * argv[]);
extern int command_poke(int argc, const char * argv[]);
extern int command_peek(int argc, const char * argv[]);
extern int command_call(int argc, const char * argv[]);

#endif
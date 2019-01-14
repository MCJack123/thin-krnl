#ifndef THINKRNL_FILESYSTEM_FS_H
#define THINKRNL_FILESYSTEM_FS_H
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void * FILESYSTEM;
typedef unsigned int size_t;

typedef struct file {
    FILESYSTEM fs_obj;
    void * fp;
    size_t pos;
    int err;
} FILE;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// Opens a filesystem from a disk. Accepts a char with the format:
// | Null | Bus | Master/Slave | Partition |
// | 0000 |  0  |       0      |    10     |
// This example would be the 3rd partition on the primary master disk.
// Returns NULL if the disk couldn't be opened (bad fs, etc.)
extern FILESYSTEM fsopen(char partid);

// Returns a list of entries in the directory.
extern string_tokens_t listdir(FILESYSTEM fs, const char * path);

// Opens a file for reading or writing.
extern FILE * fopen(FILESYSTEM fs, const char * file, const char * mode);

// Closes a file.
extern void fclose(FILE * stream);

// Reads bytes from a file into a buffer.
extern size_t fread(void * ptr, size_t size, size_t count, FILE * stream);

// Writes bytes into a file from a buffer.
extern size_t fwrite(const void * ptr, size_t size, size_t count, FILE * stream);

// Reads a character from a file.
extern char fgetc(FILE * stream);

// Writes a character to a file.
extern char fputc(char character, FILE * stream);

// Reads a string from a file until num-1, EOF, or \n.
extern char * fgets(char * str, int num, FILE * stream);

// Writes a C-string to a file.
extern int fputs(const char * str, FILE * stream);

// Returns the current position in a file.
extern long int ftell(FILE * stream);

// Seeks to a position in a file.
extern int fseek(FILE * stream, long int offset, int origin);

// Seeks to the beginning of a file.
#define rewind(s) fseek(s, 0, SEEK_SET)

// Clears the error flags in the file.
extern void clearerr(FILE * stream);

// Returns the error flag of a file.
extern int ferror(FILE * stream);

// Returns whether the EOF bit is set in a file.
extern bool feof(FILE * stream);

#ifdef __cplusplus
}
#endif

#endif
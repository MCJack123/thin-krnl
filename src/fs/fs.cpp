#include <fs/fs.h>
#include <fs/fat.hpp>

/* 
 * How to add a filesystem:
 * 1. Write a class extending FileSystem (in fs_base.hpp).
 * 2. Add the file system type to fsopen to be able to construct the object.
 */

extern "C" {
    FILESYSTEM fsopen(char partid) {
        if (partid & 0b1000) ata_set_base(0x170);
        else ata_set_base(0x1F0);
        partition_entry_t * p = mbr_read_entries((partid & 0b0100) >> 2);
        if (p == NULL) return NULL;
        partition_entry_t part = p[partid & 0b0011];
        if (part.type == 0x0B) {
            FAT32_FileSystem * fs = new FAT32_FileSystem(partid, part);
            return (void*)fs;
        }
        // Add filesystems here 
        else {
            return NULL;
        }
    }

    // Returns a list of entries in the directory.
    string_tokens_t listdir(FILESYSTEM fs, const char * path) {
        return ((FileSystem*)fs)->list(path);
    }

    // Opens a file for reading or writing.
    FILE * fopen(FILESYSTEM fs, const char * file, const char * mode) {
        FILE retval;
        retval.fs_obj = fs;
        retval.err = 0;
        retval.pos = 0;
        retval.fp = ((FileSystem*)fs)->open(file);
        if (retval.fp == NULL) return NULL;
        return &retval;
    }

    // Closes a file.
    void fclose(FILE * stream) {
        if (stream == NULL) return;
        ((FileSystem*)stream->fs_obj)->close(stream->fp);
    }

    // Reads bytes from a file into a buffer.
    size_t fread(void * ptr, size_t size, size_t count, FILE * stream) {
        if (stream == NULL) return 0;
        size_t retval = 0;
        for (int i = 0; i < count; i++) {
            retval += ((FileSystem*)stream->fs_obj)->read(ptr, stream->pos, size, stream->fp);
            stream->pos += size;
            if (retval < size * i) {
                stream->err = 1;
                return retval;
            }
        }
        if (retval < size * count) stream->err = 1;
        return retval;
    }

    // Writes bytes into a file from a buffer.
    size_t fwrite(const void * ptr, size_t size, size_t count, FILE * stream) {
        if (stream == NULL) return 0;
        size_t retval = 0;
        for (int i = 0; i < count; i++) {
            retval += ((FileSystem*)stream->fs_obj)->write(ptr, stream->pos, size, stream->fp);
            stream->pos += size;
            if (retval < size * i) {
                stream->err = 1;
                return retval;
            }
        }
        if (retval < size * count) stream->err = 1;
        return retval;
    }

    // Reads a character from a file.
    char fgetc(FILE * stream) {
        if (stream == NULL) return 0;
        char retval;
        if (((FileSystem*)stream->fs_obj)->read(&retval, stream->pos++, 1, stream->fp) != 1) {
            stream->err = 1;
            return 0;
        }
        return retval;
    }

    // Writes a character to a file.
    char fputc(char character, FILE * stream) {
        if (stream == NULL) return 0;
        if (((FileSystem*)stream->fs_obj)->read(&character, stream->pos++, 1, stream->fp) != 1) {
            stream->err = 1;
            return 0;
        }
        return character;
    }

    // Reads a string from a file until num-1, EOF, or \n.
    char * fgets(char * str, int num, FILE * stream);

    // Writes a C-string to a file.
    int fputs(const char * str, FILE * stream) {
        if (fwrite(str, strlen(str), 1, stream) != strlen(str)) return -1;
        return 1;
    }

    // Returns the current position in a file.
    long int ftell(FILE * stream) {
        return stream->pos;
    }

    // Seeks to a position in a file.
    int fseek(FILE * stream, long int offset, int origin) {
        if (origin == SEEK_SET) stream->pos = offset;
        else if (origin == SEEK_CUR) stream->pos += offset;
        else if (origin == SEEK_END) return 2; // not implemented
        else return 1;
        return 0;
    }

    // Clears the error flags in the file.
    void clearerr(FILE * stream) {
        stream->err = 0;
    }

    // Returns the error flag of a file.
    int ferror(FILE * stream) {
        return stream->err;
    }

    // Returns whether the EOF bit is set in a file.
    bool feof(FILE * stream) {
        return stream->err & 1;
    }
}
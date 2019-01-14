#ifndef THINKRNL_FS_BASE_HPP
#define THINKRNL_FS_BASE_HPP

#include <fs/fs.h>
#include <drivers/mbr.h>

class FileSystem {
public:
    static const char name[4];
    FileSystem(char partid, partition_entry_t part);
    virtual string_tokens_t list(const char * name);
    virtual void * open(const char * path);
    virtual size_t read(void * ptr, int start, int size, void * file);
    virtual size_t write(const void * ptr, int start, int size, void * file);
    virtual void close(void * file);
};

#endif
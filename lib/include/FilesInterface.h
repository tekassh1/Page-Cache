#ifndef FILES_INTERFACE
#define FILES_INTERFACE

#include <sys/types.h>

#include <unordered_map>

#include "PageCache.h"

class FilesManager {
    PageCache page_cache;
    std::unordered_map<int, off_t> offset_map;  // <fd, offset> for opened files

   public:
    FilesManager(int cache_size);
    int f_open(const char* path);
    int f_close(int fd);
    ssize_t f_read(int fd, void* buf, size_t count);
    ssize_t f_write(int fd, const void* buf, size_t count);
    off_t f_lseek(int fd, int offset, int whence);
    int fsync(int fd);
};

#endif
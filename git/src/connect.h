#ifndef CONNECT_H
#define CONNECT_H

#define STATUS_FAIL 0
#define STATUS_SUCESS 1

typedef struct MemoryStruct {
    char* memory;
    size_t size;
    int status;
} MemoryStruct;

MemoryStruct getRefFiles(const char* url);

#endif
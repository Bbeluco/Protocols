#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "zlib_utils.h"
#include "connect.h"

typedef struct PackInfo {
    char* hash;
    char* ref;
}PackInfo;

struct ParsedPackObject {
    uint8_t *content;
    char type[10];
    uint8_t *ref;
};

static struct PackInfo packInfoObjectCreate(MemoryStruct* memStruct) {
    struct PackInfo pack;
    
    int initial_position = 0;
    char sub[memStruct->size];
    for(int i = 0; i < memStruct->size; i++) {
        memset(sub, 0, sizeof(sub));

        if(memStruct->memory[i] == '\n') {
            int SKIP_NEXT_LINE = 1;
            strncpy(sub, memStruct->memory + initial_position + SKIP_NEXT_LINE, i);
            sub[i] = '\0';
            if(
                (strstr(sub, "refs/heads/master") || 
                strstr(sub, "refs/heads/main")) &&
                strstr(sub, "003")
            ) {
                
                int length = strlen(sub) - 4;
                pack.hash = malloc(length);
                pack.ref = malloc(20); // This is for refs/heads/master OR refs/heads/main
                char *hash = strtok(sub, " ");
                strncpy(pack.hash, sub + 4, length);
                pack.hash[length] = '\0';

                char *ref = strtok(NULL, "\n");
                int ref_len = strlen(ref);
                strncpy(pack.ref, ref, ref_len);
                pack.ref[ref_len] = '\0';
                break;
            }
        }
        initial_position+=1;
    }

    return pack;
}

uint32_t readUInt32BE(uint8_t *buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

int parsePackObjectHeader(char* buffer, int i, int *type, int *size) {
    int cur = i;
    *type = (buffer[cur] & 112) >> 4;
    *size = buffer[cur] & 15;
    int offset = 4;
    while(buffer[cur] >= 128) {
        cur++;
        size += (buffer[cur] & 127) << offset;
        offset += 7;
        if(cur >= 20) {
            break;
        }
    } 

    return cur - i + 1;
}

uint8_t parsePackObject(uint8_t *buffer, int i, struct ParsedPackObject *parsedPackStruct) {
    const char* TYPE_CODES[] = {"", "commit", "tree", "blob", "", "", "", "delta"};
    int size, type;
    size_t used;
    uint8_t *decompressedData;
    

    int parsedBytes = parsePackObjectHeader(buffer, i, &type, &size);
    i += parsedBytes;
    if(type < 7 && type != 5) {
        if (decompressPackObject(buffer + i, size, &decompressedData, &used) != 0) {
            parsedPackStruct->content = decompressData;
            strcpy(parsedPackStruct->type, TYPE_CODES[type]);
            parsedPackStruct->ref = NULL;
            return parsedBytes + used;
        }
    } else if(type == 7) {
        printf("BBB");
    }

}

int clone(const char* url) {
    struct ParsedPackObject parsedPackStruct = {0};
    MemoryStruct chunk = getRefFiles(url);
    if(chunk.status == STATUS_FAIL) {
        return -1;
    }

    PackInfo pack = packInfoObjectCreate(&chunk);
    MemoryStruct packFile = getPackFileFromServer(url, pack.hash);
    uint8_t *packObjects = packFile.memory + 20;
    uint32_t packObjectCount = readUInt32BE(packObjects + 16);

    int i = 0;
    int bytesRead = parsePackObject(packObjects, i, &parsedPackStruct);
    printf("bytesRead: %i", bytesRead);
    // for(int count = 0; count < packObjectCount; count++) {
        
    // }

    printf("pack.hash: %s\n", pack.hash);
    printf("pack.ref: %s\n", pack.ref);
    return 0;
}
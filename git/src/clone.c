#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connect.h"

typedef struct PackInfo {
    char* hash;
    char* ref;
}PackInfo;

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

int clone(const char* url) {
    MemoryStruct chunk = getRefFiles(url);
    if(chunk.status == STATUS_FAIL) {
        return -1;
    }

    PackInfo pack = packInfoObjectCreate(&chunk);

    printf("pack.hash: %s\n", pack.hash);
    printf("pack.ref: %s\n", pack.ref);
    return 0;
}
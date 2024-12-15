#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include "zlib_utils.h"
#include "file_manager.h"

#define NULL_OPERATOR_LENGTH 1

void* cat_file(char* sha1_string) {
    long uncompressSize;
    long compressedSize;

    char* filepath= get_object_filepath(sha1_string);
    char *compressedFile = read_file(filepath, &compressedSize);
    if(compressedFile == NULL) {
        return NULL;
    }
    free(filepath);

    unsigned char *cat_file_result = decompressData(compressedFile, compressedSize, &uncompressSize);
    if(cat_file_result != NULL) {
        unsigned char *header_end = memchr(cat_file_result, '\0', uncompressSize);
        if (!header_end) {
            fprintf(stderr, "Failed to find Git object header end\n");
            free(cat_file_result);
            free(compressedFile);
            return NULL;
        }
        unsigned char *objectContent = header_end + NULL_OPERATOR_LENGTH;
        ulong objectSize = uncompressSize - (objectContent - cat_file_result);
        printf("%.*s", (int)objectSize, objectContent);
    }
    free(compressedFile);
    free(cat_file_result);
}
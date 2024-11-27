#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_manager.h"
#include "zlib_utils.h"

#define NULL_OPERATOR_LENGTH 1

void* ls_tree(char* sha1, int name_only) {
    long uncompressSize;
    long compressedSize;

    char* filepath= get_object_filepath(sha1);
    char *compressedFile = read_file(filepath, &compressedSize);
    if(compressedFile == NULL) {
        return NULL;
    }
    free(filepath);

    unsigned char *cat_file_result = decompressData(compressedFile, compressedSize, &uncompressSize);
    if(cat_file_result != NULL) {
        const char expected_type[] = "tree";
        for(int i = 0; i < 4; i++) {
            if(cat_file_result[i] != expected_type[i]) {
                fprintf(stderr, "Unexpected type, please select another hash\n");
                free(cat_file_result);
                free(compressedFile);
                return NULL;
            }
        }

        unsigned char *header_end = memchr(cat_file_result, '\0', uncompressSize);
        if (!header_end) {
            fprintf(stderr, "Failed to find Git object header end\n");
            free(cat_file_result);
            free(compressedFile);
            return NULL;
        }
        unsigned char *objectContent = header_end + NULL_OPERATOR_LENGTH;
        ulong objectSize = uncompressSize - (objectContent - cat_file_result);

        char *content_converted = malloc(objectSize * 2);
        sprintf(content_converted, "%s", objectContent);
        int should_print_hex = 0;
        for(int i = 8; i < objectSize; i++) {
            if(cat_file_result[i] == '\0'){
                should_print_hex = 1;
                printf(" ");
            } else {
                printf("%c", cat_file_result[i]);
            }

            if(should_print_hex) {
                int j = 0;
                while(j <= 20) {
                    printf("%02x", cat_file_result[i+j]);
                    j++;
                }
                i+=20;
                should_print_hex = 0;
                printf("\n");
            }
            
        }
    }
    free(compressedFile);
    free(cat_file_result);
}
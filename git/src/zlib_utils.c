#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include "file_manager.h"

unsigned char *decompressData(const unsigned char *compressedData, long compressedSize, long *decompressedSize) {
    *decompressedSize = compressedSize * 4;
    unsigned char *decompressedData = malloc(*decompressedSize);
    if (decompressedData == NULL) {
        fprintf(stderr, "Failed to allocate memory for decompression\n");
        return NULL;
    }
    int result = uncompress(decompressedData, decompressedSize, compressedData, compressedSize);

    if (result != Z_OK) {
        fprintf(stderr, "Decompression failed with error code: %d\n", result);
        free(decompressedData);
        return NULL;
    }
    return decompressedData;
}

unsigned char *compressData(char *source, long sourceLen, long *compressedSize) {
    *compressedSize = sourceLen * 4;
    unsigned char *compressedData = malloc(*compressedSize);
    if (compressedData == NULL) {
        fprintf(stderr, "Failed to allocate memory for decompression\n");
        return NULL;
    }
    int result = compress(compressedData, compressedSize, source, sourceLen);

    if (result != Z_OK) {
        fprintf(stderr, "Decompression failed with error code: %d\n", result);
        free(compressedData);
        return NULL;
    }
    return compressedData;
}

int save_compress_data_in_git_folder(char* sha1, char* content, int total_file_content_length) {
    char* git_object_file_path = get_object_filepath(sha1);
    if(git_object_file_path == NULL) {
        return -1;
    }

    long compressedSize;
    char *compressedContent = compressData(content, total_file_content_length, &compressedSize);
    if(compressedContent != NULL) {
        int r = write_file(git_object_file_path, compressedContent, compressedSize);
        free(git_object_file_path);
        free(compressedContent);
        if(r != 0) {
            return -1;
        }
    }

    return 0;
}

int decompressPackObject(uint8_t *buffer, size_t size, uint8_t **decompressedData, size_t *used) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    inflateInit(&stream);

    size_t bufferSize = size * 2; // Initial output buffer size
    *decompressedData = malloc(bufferSize);
    if (!*decompressedData) {
        inflateEnd(&stream);
        return -1;
    }

    stream.next_in = buffer;
    stream.avail_in = size;
    stream.next_out = *decompressedData;
    stream.avail_out = bufferSize;

    int ret;
    while ((ret = inflate(&stream, Z_NO_FLUSH)) != Z_STREAM_END) {
        if (ret != Z_OK && ret != Z_BUF_ERROR) {
            free(*decompressedData);
            inflateEnd(&stream);
            return -1;
        }

        if (stream.avail_out == 0) {
            bufferSize *= 2;
            *decompressedData = realloc(*decompressedData, bufferSize);
            if (!*decompressedData) {
                inflateEnd(&stream);
                return -1;
            }

            stream.next_out = *decompressedData + stream.total_out;
            stream.avail_out = bufferSize - stream.total_out;
        }
    }

    *used = stream.total_in;
    inflateEnd(&stream);
    return 0;
}
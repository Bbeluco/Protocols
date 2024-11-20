#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

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

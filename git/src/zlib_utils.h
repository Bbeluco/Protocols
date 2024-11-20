#ifndef ZLIB_UTILS_H
#define ZLIB_UTILS_H

unsigned char *decompressData(const unsigned char *compressedData, long compressedSize, long *decompressedSize);
unsigned char *compressData(char *source, long sourceLen, long *compressedSize);

#endif
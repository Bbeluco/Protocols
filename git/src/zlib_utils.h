#ifndef ZLIB_UTILS_H
#define ZLIB_UTILS_H

unsigned char *decompressData(const unsigned char *compressedData, long compressedSize, long *decompressedSize);
unsigned char *compressData(char *source, long sourceLen, long *compressedSize);
int save_compress_data_in_git_folder(char* sha1, char* content, int total_file_content_length);

#endif
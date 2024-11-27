#ifndef HASH_FILE_H
#define HASH_FILE_H

void* hash_file(char* file_path);
char* calculate_sha1(char* buffer, int total_file_content_length);

#endif
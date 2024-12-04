#ifndef HASH_FILE_H
#define HASH_FILE_H

void* hash_file(char* file_path, char* file_type);
char* hash_commit(char* content, char* file_type);
char* calculate_sha1(char* buffer, int total_file_content_length);

#endif
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

char* get_object_filepath(const char* sha1_string);
char* read_file(char* file_path, long *compressedSize);
int write_file(const char* file_path, const char* content, long compressedSize);
char* list_all_files(const char* folder);
int do_file_exists(char *file);

#endif
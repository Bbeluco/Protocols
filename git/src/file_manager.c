#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

#define NULL_OPERATOR_LENGTH 1

static int calculate_file_length(FILE *file) {
    if(fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Cannot seek end of file: %s\n", strerror(errno));
        fclose(file);
        return -1;
    }

    long fileLength = ftell(file);

    if(fseek(file, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Failed to seek to beginning of file\n");
        fclose(file);
        return -1;
    }

    return fileLength;
}

char* get_object_filepath(const char* sha1_string) {
    char subdir[3];
    char *filepath = malloc(55);
    if(filepath == NULL) {
        fprintf(stderr, "Failed to allocate memory to read filepath\n");
        return NULL;
    }

    strncpy(subdir, sha1_string, 2);
    subdir[2] = '\0';
    sha1_string += 2; // remove first two characters
    sprintf(filepath, ".git/objects/%s/%s", subdir, sha1_string);
    
    return filepath;
}

char* read_file(char* file_path, long *fileSize) {
    FILE *targetFile = fopen(file_path, "rb");
    if(targetFile == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
        return NULL;
    }

    int fileLength = calculate_file_length(targetFile);
    if(fileLength < 0) {
        fprintf(stderr, "No such file or directory\n");
        fclose(targetFile);
        return NULL;
    }
    char* fileContent = malloc(fileLength + NULL_OPERATOR_LENGTH);
    if(fileContent == NULL) {
        fprintf(stderr, "Failed to allocate memory to insert compressed file\n");
        fclose(targetFile);
        return NULL;
    }

    size_t bytesRead = fread(fileContent, sizeof(char), fileLength, targetFile);
    if(bytesRead != fileLength) {
        perror("Failed to read file");
        free(fileContent);
        fclose(targetFile);
        return NULL;
    
    }

    *fileSize = bytesRead;
    fileContent[fileLength] = '\0';
    fclose(targetFile);

    return fileContent;
}

static int create_folder(const char* file_path) {
    const int TOTAL_LENGTH_OBJECT_FOLDER = 15;
    char aux[TOTAL_LENGTH_OBJECT_FOLDER + NULL_OPERATOR_LENGTH];
    const int START = 0;
    strncpy(aux, file_path + START, TOTAL_LENGTH_OBJECT_FOLDER - START);
    mkdir(aux, 0775);

    return 0;
}

int write_file(const char* file_path, const char* content, long compressedSize) {
    FILE *fptr;
    create_folder(file_path);
    fptr = fopen(file_path, "wb");
    if(fptr == NULL) {
        perror("Error while creating file");
        return -1;
    }
    fwrite(content, sizeof(char), compressedSize, fptr);
    fclose(fptr);

    return 0;
}

int do_file_exists(char *file) {
    char* path = get_object_filepath(file);
    FILE* fp = fopen(path, "r");

    free(path);
    if (fp == NULL)  { 
        printf("Could not open current directory\n"); 
        return 0; 
    }

    return 1;
}
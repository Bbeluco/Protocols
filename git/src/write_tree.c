#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "hash_file.h"
#include "file_manager.h"
#include "zlib_utils.h"

#define NULL_OPERATOR_LENGTH 1

//We'll assume that everything in the folder is "staging", but original git requests stagging as a prev operation

static char* format_entry_data(char* sha1, struct dirent *de, int *total_tree_size) {
    char* file_mode = (de->d_type == DT_DIR) ? "40000" : "100644";
    int aux = strlen(file_mode) + strlen(" ") + strlen(de->d_name)+ NULL_OPERATOR_LENGTH + strlen(sha1) + NULL_OPERATOR_LENGTH;
    *total_tree_size += aux;
    char *hashed_tree_entry = malloc(aux);
    sprintf(hashed_tree_entry, "%s %s%c%s", file_mode, de->d_name, '\0', sha1);

    return hashed_tree_entry;
}

static char* calculate_tree_entry_file(char *file_path) {
    char *sha1_current_file = hash_file(file_path);
    if(sha1_current_file == NULL) {
        free(sha1_current_file);
        printf("Error while tring to hash file %s", file_path);
        return NULL;
    }

    return sha1_current_file;
}

char* write_tree(const char* folder, int should_include_tree_header) {
    struct dirent *de;
    DIR *dr = opendir(folder);

    if(dr == NULL) {
        printf("Could not open specificate folder: %s", folder);
        return NULL;
    }

    char *tree_content = malloc(1024);
    int entry_length = 0;

    while((de = readdir(dr)) != NULL) {
        if(strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".git") == 0 || strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "custom_git") == 0) {
            continue;
        }
        
        const int QUANTITY_EXTRA_CHARS = 2;
        char *file_or_folder_path = malloc(strlen(folder) + strlen(de->d_name) + QUANTITY_EXTRA_CHARS);
        sprintf(file_or_folder_path, "%s/%s", folder, de->d_name);
        char *new_data;
        int prev_entry_length = entry_length;
        if(de->d_type == DT_DIR && (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "./.") != 0)) {
            char* sha1 = write_tree(file_or_folder_path, 0);
            if(sha1 == NULL) {
                return NULL;
            }

            new_data = format_entry_data(sha1, de, &entry_length);

            char* git_object_file_path = get_object_filepath(sha1);
            if(git_object_file_path == NULL) {
                free(sha1);
                return NULL;
            }

            long compressedSize;
            char *compressedContent = compressData(new_data, entry_length, &compressedSize);
            if(compressedContent != NULL) {
                int r = write_file(git_object_file_path, compressedContent, compressedSize);
                free(git_object_file_path);
                if(r != 0) {
                    return NULL;
                }
            }
            free(sha1);
        } else {
            char* sha1 = calculate_tree_entry_file(file_or_folder_path);
            if(sha1 == NULL) {
                return NULL;
            }

            new_data = format_entry_data(sha1, de, &entry_length);
            free(sha1);
        }

        for(int i = 0; i < (entry_length - prev_entry_length); i++) {
            tree_content[(prev_entry_length)+i] = new_data[i];
        }

        free(new_data);
    }

    for(int i = 0; i < entry_length; i++) {
        printf("%c", tree_content[i]);
    }
    printf("\n\n\n\n");
    //Antes de executar o hash precisamos dizer qual o tamanho desse carinha e colocar o "tree" na frente
    //Seria de otimo tom refatorar esse codigo tambem, ta meio sinistro a situacao aqui
    if(should_include_tree_header == 1) {
        char* aux = malloc(20);
        char* result = malloc(2048);
        sprintf(aux, "tree %i", entry_length);
        
        sprintf(result, "%s%c%s", aux, '\0', tree_content);
        int str_len_aux = strlen(aux);
        for(int i = 0; i < entry_length+20; i++) {
            result[i+str_len_aux] = tree_content[i];
        }
        tree_content = result;
    }
    for(int i = 0; i < entry_length; i++) {
        printf("%c", tree_content[i]);
    }
    printf("\n\n\n\n");
    char *tree_sha1 = calculate_sha1(tree_content, entry_length); 
    free(tree_content);
    if(tree_sha1 == NULL) {
        free(tree_sha1);
        printf("Error while calculating tree sha for folder %s", folder);
        return NULL;
    }

    closedir(dr);

    return tree_sha1;
}
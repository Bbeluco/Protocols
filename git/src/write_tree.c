#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "hash_file.h"

#define NULL_OPERATOR_LENGTH 1

//We'll assume that everything in the folder is "staging", but original git requests stagging as a prev operation

//Vamos supor que temos so arquivos e nao temos pastas -> Ongoing
//Como a aplicacao vai se comportar em casos de pastas vazias
//Como a aplicacao vai se comportar em casos de pastas com itens

char* write_tree(const char* folder) {
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
            printf("de_dname: %s\n", de->d_name);
            continue;
        }
        const int QUANTITY_EXTRA_CHARS = 2;
        char *file_or_folder_path = malloc(strlen(folder) + strlen(de->d_name) + QUANTITY_EXTRA_CHARS);
        sprintf(file_or_folder_path, "%s/%s", folder, de->d_name);
        if(de->d_type == DT_DIR) {
            write_tree(file_or_folder_path);
        } else {
            // TODO: Hash every file in this place

            char *sha1_current_file = hash_file(file_or_folder_path);
            if(sha1_current_file == NULL) {
                free(sha1_current_file);
                printf("Error while tring to hash file %s", file_or_folder_path);
                return NULL;
            }

            const char* FILE_TYPE = "40000";
            int aux = strlen(FILE_TYPE) + strlen(" ") + strlen(de->d_name)+ NULL_OPERATOR_LENGTH + strlen(sha1_current_file) + NULL_OPERATOR_LENGTH;
            entry_length += aux;
            char *hashed_tree_entry = malloc(aux);
            sprintf(hashed_tree_entry, "%s %s%c%s", FILE_TYPE, de->d_name, '\0', sha1_current_file);
            for(int i = 0; i <= aux; i++) {
                tree_content[(entry_length)+i] = hashed_tree_entry[i];
            }
            free(sha1_current_file);
        }
    }

    for(int i = 0; i < entry_length; i++) {
        printf("%c", tree_content[i]);
    }
    printf("\n");
    char *tree_sha1 = calculate_sha1(tree_content, entry_length);
    if(tree_sha1 == NULL) {
        free(tree_sha1);
        printf("Error while calculating tree sha for folder %s", folder);
        return NULL;
    }
    printf("%s\n", tree_sha1);
    free(tree_sha1);
    free(tree_content);
    closedir(dr);
}
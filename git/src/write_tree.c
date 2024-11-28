#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "hash_file.h"

#define NULL_OPERATOR_LENGTH 1

//We'll assume that everything in the folder is "staging", but original git requests stagging as a prev operation

//Vamos supor que temos so arquivos e nao temos pastas -> DONE
//Como a aplicacao vai se comportar em casos de pastas vazias -> ONGOING
//Como a aplicacao vai se comportar em casos de pastas com itens

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

//O correto em casos onde a pasta eh vazia / tem pastas eh a gente calcular o tamanho da pasta, a funcao hash-file nao funciona pq estamos tentando hashear o arquivo e nao a pasta em si
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
        printf(" arquivo: %s\n", de->d_name);
        // if(strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".git") == 0 || strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "custom_git") == 0) {
        if(strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".git") == 0 || strcmp(de->d_name, "custom_git") == 0) {
            continue;
        }

        const int QUANTITY_EXTRA_CHARS = 2;
        char *file_or_folder_path = malloc(strlen(folder) + strlen(de->d_name) + QUANTITY_EXTRA_CHARS);
        sprintf(file_or_folder_path, "%s/%s", folder, de->d_name);
        char *new_data;
        int prev_entry_length = entry_length;
        if(de->d_type == DT_DIR && (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "./.") != 0)) {
            new_data = write_tree(file_or_folder_path);
        } else {
            printf("calculando hash arquivo: %s", file_or_folder_path);
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
    printf("\n");

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
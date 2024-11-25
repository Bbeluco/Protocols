#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

void* write_tree(const char* folder) { //We'll assume that everything in the folder is "staging", but original git requests stagging as a prev operation
    struct dirent *de;
    DIR *dr = opendir(folder);

    if(dr == NULL) {
        printf("Could not open specificate folder: %s", folder);
        return NULL;
    }

    while((de = readdir(dr)) != NULL) {
        if(strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".git") == 0 || strcmp(de->d_name, ".") == 0) {
            continue;
        }
        if(de->d_type == DT_DIR) {
            char *new_folder = malloc(strlen(folder) + strlen(de->d_name));
            sprintf(new_folder, "%s/%s", folder, de->d_name);
            write_tree(new_folder);
        } else {
            printf("%s:\t%s\n", folder, de->d_name); // TODO: Hash every file in this place
        }
    }

    closedir(dr);
}
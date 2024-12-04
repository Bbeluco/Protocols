#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "file_manager.h"
#include "hash_file.h"

char* commit_tree(char *argv[], int argc){
    char* sha1_tree = argv[2];
    int file = do_file_exists(sha1_tree);
    if(file != 1) {
        printf("tree do not exists\n");
        return NULL;
    }

    char *result = malloc(1024);
    sprintf(result, "tree %s\n", sha1_tree);

    if(strcmp(argv[3], "-p") == 0) {
        char* parent_commit = argv[4];
        if(do_file_exists(parent_commit) != 1){
            printf("parent commit do not exists\n");
            return NULL;
        }
        char* aux = malloc(100);
        sprintf(aux, "parent %s\n", parent_commit);
        strcat(result, aux);
        free(aux);
    }
    char* author = malloc(100);
    sprintf(author, "author Bruno Beluco <bbeluco@email.com> %u\n", (unsigned)time(NULL));
    strcat(result, author);
    free(author);

    char* commiter = malloc(100);
    sprintf(commiter, "commiter Bruno Beluco <bbeluco@email.com> %u\n", (unsigned)time(NULL));
    strcat(result, commiter);
    free(commiter);
    if(!(strcmp(argv[3], "-m") == 0) && !(strcmp(argv[5], "-m") == 0)) {
        printf("invalid comment\n");
        free(result);
        return NULL;
    }

    char *comment = malloc(1024); //I know this is unsafe, please just focus on the concept
    sprintf(comment, "\n%s\n", argv[argc - 1]);
    strcat(result, comment);


    return hash_commit(result, "commit");
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include "cat_file.h"
#include "hash_file.h"
#include "ls_tree.h"
#include "write_tree.h"
#include "commit_tree.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (argc < 2) {
        fprintf(stderr, "Usage: ./your_program.sh <command> [<args>]\n");
        return 1;
    }
    
    const char *command = argv[1];
    
    if (strcmp(command, "init") == 0) {
        // You can use print statements as follows for debugging, they'll be visible when running tests.
        fprintf(stderr, "Logs from your program will appear here!\n");
        
        if (mkdir(".git", 0755) == -1 || 
            mkdir(".git/objects", 0755) == -1 || 
            mkdir(".git/refs", 0755) == -1) {
            fprintf(stderr, "Failed to create directories: %s\n", strerror(errno));
            return 1;
        }
        
        FILE *headFile = fopen(".git/HEAD", "w");
        if (headFile == NULL) {
            fprintf(stderr, "Failed to create .git/HEAD file: %s\n", strerror(errno));
            return 1;
        }
        fprintf(headFile, "ref: refs/heads/main\n");
        fclose(headFile);
        
        printf("Initialized git directory\n");
    } else if((strcmp(command, "cat-file")) == 0 && (strcmp(argv[2], "-p") == 0) && (strlen(argv[3]) > 0)) {
        cat_file(argv[3]);
    } else if((strcmp(command, "hash-object")) == 0 && (strcmp(argv[2], "-w") == 0) && (strlen(argv[3]) > 0)) {
        char *sha1 = hash_file(argv[3], "blob");
        if(sha1 != NULL) {
            printf("%s", sha1);
            printf("\n");
        }
        free(sha1);
    } else if ((strcmp(command, "ls-tree") == 0 && (argc <= 4))) {
        int name_only = strcmp(argv[2], "--name-only");
        char *sha1 = ((name_only != 0) ? argv[2] : argv[3]);
        ls_tree(sha1, name_only);
    }else if(strcmp(command, "write-tree") == 0) {
        const char* INITIAL_FOLDER = ".";
        const int INCLUDE_TREE_HEADER = 1;
        char *tree = write_tree(INITIAL_FOLDER, INCLUDE_TREE_HEADER);
        printf("%s\n", tree);
        free(tree);
    }else if((strcmp(command, "commit-tree") == 0)) {
        char* commit = commit_tree(argv, argc);
        printf("%s\n", commit);
        free(commit);
    }else {
        fprintf(stderr, "Unknown command %s\n", command);
        return 1;
    }
    
    return 0;
}

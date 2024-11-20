#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include "file_manager.h"
#include "zlib_utils.h"

#define NULL_OPERATOR_LENGTH 1

static void intToStr(int N, char *str) {
    int i = 0;
  
    // Save the copy of the number for sign
    int sign = N;

    // If the number is negative, make it positive
    if (N < 0)
        N = -N;

    // Extract digits from the number and add them to the
    // string
    while (N > 0) {
      
        // Convert integer digit to character and store
      	// it in the str
        str[i++] = N % 10 + '0';
      	N /= 10;
    } 

    // If the number was negative, add a minus sign to the
    // string
    if (sign < 0) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

static int calculate_all_nulls_required() {
    const int null_operator_filelength = 1;
    const int null_operator_header = 1;
    const int null_operator_end_of_string = 1;
    
    return null_operator_filelength + null_operator_header + null_operator_end_of_string;
}

static char* calculate_sha1(char* buffer, int total_file_content_length) {
    SHA_CTX sha;
    uint8_t sha1_results_encoded[SHA_DIGEST_LENGTH];
    const int HEX_DECIMAL_QUANTITY = 2;
    char *sha1_char_type = malloc((SHA_DIGEST_LENGTH + NULL_OPERATOR_LENGTH) * HEX_DECIMAL_QUANTITY);
    if(sha1_char_type == NULL) {
        fprintf(stderr, "Unable to allocate memory to calculate SHA1");
        return NULL;
    }
    SHA1(buffer, total_file_content_length, sha1_results_encoded);
    sprintf(sha1_char_type, "%02x", sha1_results_encoded[0]);
    for(int n = 1; n < SHA_DIGEST_LENGTH; n++) {
        char aux[3];
        sprintf(aux, "%02x", sha1_results_encoded[n]);
        strcat(sha1_char_type, aux); 
    }
    return sha1_char_type;
}

void* hash_file(char* file_path) {
    long fileSize;
    char *fileContent = read_file(file_path, &fileSize);
    char fileLengthInString[1024];
    fileSize -= NULL_OPERATOR_LENGTH;
    intToStr(fileSize, fileLengthInString);

    const int BLOB_LENGTH = 5;
    int total_file_content_length = BLOB_LENGTH + fileSize + calculate_all_nulls_required();
    char buffer[total_file_content_length];
    sprintf(buffer, "%s%s%c%s", "blob ", fileLengthInString, '\0', fileContent);
    
    char* sha1 = calculate_sha1(buffer, total_file_content_length);
    if(sha1 == NULL) {
        return NULL;
    }
    char* git_object_file_path = get_object_filepath(sha1);
    if(git_object_file_path == NULL) {
        free(sha1);
        return NULL;
    }

    long compressedSize;
    char *compressedContent = compressData(buffer, total_file_content_length, &compressedSize);
    if(compressedContent != NULL) {
        int r = write_file(git_object_file_path, compressedContent, compressedSize);
        if(r == 0) {
            printf("%s", sha1);
        }
    }
    free(sha1);
    free(git_object_file_path);
}
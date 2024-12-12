#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "connect.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

MemoryStruct getRefFiles(const char* url) {
    CURL *curl;
    CURLcode res;

    MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;
    chunk.status = STATUS_FAIL;

    curl = curl_easy_init();
    if(!curl) {
        printf("Failed to allocate curl");
        return chunk;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://github.com/Bbeluco/LearnCpp.git/info/refs?service=git-upload-pack");

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Host: github.com");
    headers = curl_slist_append(headers, "User-Agent: git/2.34.1");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(chunk.memory);
        return chunk;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    chunk.status = STATUS_SUCESS;

    return chunk;
}
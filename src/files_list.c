#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/files_list.h"

void init_file_list(FileList *list, const int capacity) {
    list->files_path = malloc(sizeof(char *) * capacity);
    list->count = 0;
    list->capacity = capacity;
}

void add_file(FileList *list, const char *path) {
    char actualpath[MAX_PATH_LENGTH];
    char* realfilepath = realpath(path, actualpath);
    if (list->count >= list->capacity) {
        list->capacity = (list->capacity == 0) ? 1 : list->capacity * 2;
        list->files_path = realloc(list->files_path, sizeof(char *) * list->capacity);
    }
    list->files_path[list->count++] = strdup(realfilepath);
}

void free_file_list(FileList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->files_path[i]);
    }
    free(list->files_path);
    list->count = 0;
    list->capacity = 0;
}
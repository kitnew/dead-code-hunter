#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "include/files_list.h"

#define MAX_PATH_LENGTH 4096
#define MAX_FILES_COUNT 1000

// Check if directory is "." or ".." to avoid infinite recursion
static bool is_system_hidden_dir(const char *name){
    if (name[0] == '.'){
        if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0){
            return true;
        }
    }
    return false;
}

// Check if filename has one of the specified extensions
// exts is NULL-terminated array of extension strings
static bool has_extension(const char *filename, const char **exts){
    size_t exts_count = 0;
    while (exts[exts_count] != NULL) {
        exts_count++;
    }

    char dot = '.';
    
    for (size_t i = 0; i < exts_count; i++) {
        if (strchr(filename, dot) && strcmp(strchr(filename, dot), exts[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Recursively scan directory and collect files with specified extensions
void scan_directory(const char *path, const char **exts, FILE *output, int depth) {
    if (path == NULL || exts == NULL || output == NULL || depth < 0 || depth > 25) {
        return;
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        if (errno == EACCES) {
            // TODO: gotta do something with this
            fprintf(output, "%*sPermission denied: %s\n", depth * 2, "", path);
        }
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (is_system_hidden_dir(entry->d_name)) {
            continue;
        }

        // Get file status using full path
        struct stat st;
        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (stat(full_path, &st) == -1) {
            if (errno == EACCES) {
                // TODO: gotta do something with this either
                fprintf(output, "%*sPermission denied: %s\n", depth * 2, "", path);
            }
            continue;
        }

        // Recursively process directories, add matching files to list
        if (S_ISDIR(st.st_mode)) {
            scan_directory(full_path, exts, output, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if (has_extension(entry->d_name, exts)) {
                add_file(&files_list, full_path);
            }
        }
    }
    
    closedir(dir);
}
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_PATH_LENGTH 4096
#define MAX_EXT_LENGTH 16
#define MAX_FILES_COUNT 1000

typedef struct {
    char **files_path;
    int count;
    int capacity;
} FileList;

FileList files_list;

static void init_file_list(FileList *list, const int capacity) {
    list->files_path = malloc(sizeof(char *) * capacity);
    list->count = 0;
    list->capacity = capacity;
}

static void add_file(FileList *list, const char *path) {
    char actualpath[MAX_PATH_LENGTH];
    char* realfilepath = realpath(path, actualpath);
    if (list->count >= list->capacity) {
        list->capacity = (list->capacity == 0) ? 1 : list->capacity * 2;
        list->files_path = realloc(list->files_path, sizeof(char *) * list->capacity);
    }
    list->files_path[list->count++] = strdup(realfilepath);
}

static void free_file_list(FileList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->files_path[i]);
    }
    free(list->files_path);
    list->count = 0;
    list->capacity = 0;
}

static bool is_system_hidden_dir(const char *name){
    if (name[0] == '.'){
        if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0){
            return true;
        }
    }
    return false;
}

static bool has_extension(const char *filename, const char **exts){
    size_t exts_count = 0;
    while (exts[exts_count] != NULL) {
        exts_count++;
    }
    for (size_t i = 0; i < exts_count; i++) {
        if (strchr(filename, '.') && strcmp(strchr(filename, '.'), exts[i]) == 0) {
            return true;
        }
    }
    return false;
}

void scan_directory(const char *path, const char **exts, FILE *output, int depth) {
    if (path == NULL || exts == NULL || output == NULL || depth < 0 || depth > 25) {
        return;
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        if (errno == EACCES) {
            fprintf(output, "%*sPermission denied: %s\n", depth * 2, "", path);
        }
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (is_system_hidden_dir(entry->d_name)) {
            continue;
        }
        struct stat st;
        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (stat(full_path, &st) == -1) {
            if (errno == EACCES) {
                fprintf(output, "%*sPermission denied: %s\n", depth * 2, "", path);
            }
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            //char indent[MAX_PATH_LENGTH] = "";
            //for(int i = 0; i < depth * 2; i++) indent[i] = '-';
            //fprintf(output, "%sDirectory: %s\n", indent, entry->d_name);

            scan_directory(full_path, exts, output, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            //char indent[MAX_PATH_LENGTH] = "";
            //for(int i = 0; i < depth * 2; i++) indent[i] = '-';
            if (has_extension(entry->d_name, exts)) {
                //fprintf(output, "%sFile: %s\n", indent, entry->d_name);
                add_file(&files_list, full_path);
            }// else {
                //fprintf(output, "%sSkipped File: %s\n", indent, entry->d_name);
            //}
        }
    }
    closedir(dir);
}

int main() {
    const char *path = "../test_folder";
    const char *ext1 = ".c";
    const char *ext2 = ".h";

    const char *exts[] = {ext1, ext2, NULL};

    init_file_list(&files_list, MAX_FILES_COUNT);

    scan_directory(path, exts, stdout, 1);

    for (int i = 0; i < files_list.count; i++) {
        printf("%s\n", files_list.files_path[i]);
    }

    free_file_list(&files_list);

    return 0;
}
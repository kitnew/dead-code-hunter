#ifndef FILE_LIST_H
#define FILE_LIST_H

// Structure to store list of files with dynamic capacity
typedef struct {
    char **files_path;  // Array of file paths
    int count;          // Current number of files
    int capacity;       // Maximum capacity of the array
} FileList;

// Initialize file list with given capacity
void init_file_list(FileList *list, const int capacity);

// Add a file to the list, using realpath to resolve symbolic links
// and get absolute path. Doubles capacity if needed.
void add_file(FileList *list, const char *path);

// Clean up memory used by file list
void free_file_list(FileList *list);

#endif // FILE_LIST_H
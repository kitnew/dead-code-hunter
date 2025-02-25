#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void print_usage(const char *prog_name) {
    printf("Usage: %s [options] <path>\n", prog_name);
    printf("Options:\n");
    printf("  -l <language>   Specify the programming language (e.g., C, Python)\n");
    printf("  -r              Create backup before cleaning\n");
    printf("  -v              Enable verbose mode\n");
    printf("  --dry-run       Perform analysis without actual changes\n");
    printf("  -h, --help      Show this help message\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int verbose = 0;
    int create_backup = 0;
    int dry_run = 0;
    char *language = NULL;

    while ((opt = getopt(argc, argv, "l: rvh")) != -1) {
        switch (opt) {
            case 'l':
                language = optarg;
                break;
            case 'r':
                create_backup = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: Missing <path> argument\n");
        print_usage(argv[0]);
        return 1;
    }

    char *path = argv[optind];

    printf("Arguments parsed:\n");
    printf("  Path: %s\n", path);
    printf("  Language: %s\n", language ? language : "Not specified");
    printf("  Create backup: %s\n", create_backup ? "Yes" : "No");
    printf("  Verbose mode: %s\n", verbose ? "Yes" : "No");
    printf("  Dry run: %s\n", dry_run ? "Yes" : "No");

    return 0;
}

// File: main.c
// Author: frankischilling
// Date: 11/7/2024
// -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void count_lines_in_file(const char *filepath, long *total) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Could not open file");
        return;
    }

    long line_count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') line_count++;
    }
    fclose(file);

    *total += line_count;
    printf("cupid-lc: line count per %s: %ld\n", filepath, line_count);
}

void count_lines_in_dir(const char *dirpath, long *total) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("Could not open directory");
        return;
    }

    struct dirent *entry;
    char path[1024];
    struct stat path_stat;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Avoid adding an extra slash if dirpath already ends with one
        snprintf(path, sizeof(path), "%s%s%s", dirpath,
                 dirpath[strlen(dirpath) - 1] == '/' ? "" : "/", entry->d_name);

        stat(path, &path_stat);

        if (S_ISREG(path_stat.st_mode)) {
            count_lines_in_file(path, total);
        } else if (S_ISDIR(path_stat.st_mode)) {
            count_lines_in_dir(path, total);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
        return EXIT_FAILURE;
    }

    long total_lines = 0;
    struct stat path_stat;
    stat(argv[1], &path_stat);

    if (S_ISREG(path_stat.st_mode)) {
        count_lines_in_file(argv[1], &total_lines);
    } else if (S_ISDIR(path_stat.st_mode)) {
        count_lines_in_dir(argv[1], &total_lines);
    } else {
        fprintf(stderr, "Error: %s is neither a file nor a directory\n", argv[1]);
        return EXIT_FAILURE;
    }

    printf("cupid-lc: line count total: %ld\n", total_lines);
    return EXIT_SUCCESS;
}

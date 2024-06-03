#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage() {
    printf("Usage: head_new [-n LINES] [file ...]\n");
}

int main(int argc, char *argv[]) {
    int lines = 10; // Default number of lines
    int file_index = 1; // Default file index

    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        if (argc < 4) {
            print_usage();
            return 1;
        }
        lines = atoi(argv[2]);
        file_index = 3;
    }

    FILE *file = stdin;
    if (argc > file_index) {
        file = fopen(argv[file_index], "r");
        if (!file) {
            perror("fopen");
            return 1;
        }
    }

    char line[256];
    for (int i = 0; i < lines && fgets(line, sizeof(line), file); i++) {
        printf("%s", line);
    }

    if (file != stdin) fclose(file);

    return 0;
}

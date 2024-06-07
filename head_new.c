
/*
 * Author: Ari Salehpour
 * Course: CPE/CSC 3350
 * Assignment: Wash Assignment
 * Professor: Cameron
 * Date: 6/3/24
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINES 7  // Default number of lines to print

// Function to print usage information
void print_usage() {
    printf("Usage: ./head_new [-h] [-n N] [file.txt]\n");
}

// Function to print the first num_lines lines from the given file pointer
void print_lines(FILE *fp, int num_lines) {
    char line[1024];
    int count = 0;
    while (fgets(line, sizeof(line), fp) != NULL && count < num_lines) {
        printf("%s", line);
        count++;
    }
}

int main(int argc, char *argv[]) {
    int num_lines = DEFAULT_LINES;  // Number of lines to print, default is 7
    char *filename = NULL;  // Filename to read from

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {  // If -h flag is present
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "-n") == 0) {  // If -n flag is present
            if (i + 1 < argc) {
                num_lines = atoi(argv[i + 1]);  // Get the number of lines from the next argument
                i++;  // Skip the next argument as it has been processed
            } else {
                fprintf(stderr, "Invalid argument for -n\n");
                return 1;
            }
        } else {
            filename = argv[i];  // Get the filename
        }
    }

    // If a filename is provided, open the file and print the lines
    if (filename) {
        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {
            perror("fopen");
            return 1;
        }
        print_lines(fp, num_lines);
        fclose(fp);
    } else {
        // If no filename is provided, read from standard input
        print_lines(stdin, num_lines);
    }

    return 0;
}

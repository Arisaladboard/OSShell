/*
 * Author: Ari Salehpour
 * Course: CPE/CSC 3350
 * Assignment: Wash Assignment
 * Professor: Cameron
 * Date: 6/3/24
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <ctype.h> // Include this header for isspace
#define MAX_INPUT 1024
#define MAX_PATHS 100

// Function declarations
void print_help();
int handle_builtin(char **args);
void execute_command(char **args);
void set_path(char **paths, int count);
char* trim_whitespace(char *str);

char* path[MAX_PATHS] = { "/bin", NULL };

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        print_help();
        return 0;
    }

    char input[MAX_INPUT];
    char *args[MAX_INPUT / 2 + 1];
    
    while (1) {
        printf("wash> ");
        if (!fgets(input, MAX_INPUT, stdin)) break;

        input[strcspn(input, "\n")] = 0; // Remove the newline character
        char *trimmed_input = trim_whitespace(input);
        if (strlen(trimmed_input) == 0) continue;

        int i = 0;
        char *token = strtok(trimmed_input, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (args[0] == NULL) continue;

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        if (!handle_builtin(args)) {
            execute_command(args);
        }
    }

    return 0;
}

void print_help() {
    printf("wash - Washington Shell\n");
    printf("Built-in commands:\n");
    printf("  exit     - Exit the shell\n");
    printf("  echo     - Print arguments to the console\n");
    printf("  pwd      - Print the current working directory\n");
    printf("  cd       - Change the current directory\n");
    printf("  setpath  - Set the executable path\n");
    printf("  help     - Display this help message\n");
}

int handle_builtin(char **args) {
    if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
        return 1;
    } else if (strcmp(args[0], "pwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        return 1;
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            if (chdir(getenv("HOME")) != 0) {
                perror("cd");
            }
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        return 1;
    } else if (strcmp(args[0], "setpath") == 0) {
        set_path(&args[1], 0);
        return 1;
    } else if (strcmp(args[0], "help") == 0) {
        print_help();
        return 1;
    }
    return 0;
}

void execute_command(char **args) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

void set_path(char **paths, int count) {
    for (int i = 0; i < MAX_PATHS; i++) {
        if (paths[i] == NULL) {
            path[i] = NULL;
            break;
        } else {
            path[i] = paths[i];
        }
    }
}

char* trim_whitespace(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

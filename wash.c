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
#include <ctype.h>

#define MAX_INPUT 1024  // Maximum length of input command
#define MAX_PATHS 100   // Maximum number of paths in PATH variable

// Function declarations
void print_help();
int handle_builtin(char **args);
void execute_command(char **args);
void set_path(char **args);
char* trim_whitespace(char *str);
void redirect_output(char **args);

// Global variable for paths, initialized with /bin
char* path[MAX_PATHS] = { "/bin", NULL };

int main(int argc, char *argv[]) {
    // Check if the help flag is provided
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        print_help();
        return 0;
    }

    char input[MAX_INPUT]; // Buffer for input command
    char *args[MAX_INPUT / 2 + 1]; // Array for arguments

    // Main loop for the shell
    while (1) {
        printf("wash> "); // Print prompt
        if (!fgets(input, MAX_INPUT, stdin)) break; // Read input from user

        input[strcspn(input, "\n")] = 0; // Remove newline character
        char *trimmed_input = trim_whitespace(input); // Trim whitespace
        if (strlen(trimmed_input) == 0) continue; // Ignore empty input

        // Split input into arguments
        int i = 0;
        char *token = strtok(trimmed_input, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the argument list

        if (args[0] == NULL) continue; // Ignore empty command

        // Exit command
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // Handle redirection
        if (strchr(trimmed_input, '>') != NULL) {
            redirect_output(args);
        } else if (!handle_builtin(args)) {
            execute_command(args);
        }
    }

    return 0;
}

// Function to print help message
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

// Function to handle built-in commands
int handle_builtin(char **args) {
    // echo command
    if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
        return 1;
    } 
    // pwd command
    else if (strcmp(args[0], "pwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        return 1;
    } 
    // cd command
    else if (strcmp(args[0], "cd") == 0) {
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
    } 
    // setpath command
    else if (strcmp(args[0], "setpath") == 0) {
        set_path(&args[1]);
        return 1;
    } 
    // help command
    else if (strcmp(args[0], "help") == 0) {
        print_help();
        return 1;
    }
    return 0;
}

// Function to execute non-built-in commands
void execute_command(char **args) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        // Modify PATH environment variable for the child process
        char new_path[MAX_INPUT] = "";
        for (int i = 0; path[i] != NULL; i++) {
            strcat(new_path, path[i]);
            if (path[i + 1] != NULL) strcat(new_path, ":");
        }
        setenv("PATH", new_path, 1);

        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

// Function to set the PATH variable
void set_path(char **paths) {
    int i = 0;
    while (i < MAX_PATHS - 1 && paths[i] != NULL) {
        path[i] = paths[i];
        i++;
    }
    path[i] = NULL;
}

// Function to trim leading and trailing whitespace
char* trim_whitespace(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

// Function to handle output redirection
void redirect_output(char **args) {
    int i = 0;
    while (args[i] != NULL && strcmp(args[i], ">") != 0) {
        i++;
    }
    if (args[i] == NULL || args[i + 1] == NULL) {
        fprintf(stderr, "Invalid redirection syntax\n");
        return;
    }

    char *filename = args[i + 1];
    args[i] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        FILE *file = freopen(filename, "w", stdout);
        if (!file) {
            perror("freopen");
            exit(EXIT_FAILURE);
        }
        execute_command(args);
        fclose(file);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
    }
}

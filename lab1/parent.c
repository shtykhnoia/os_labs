#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int pipe1[2];
    pid_t pid;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *file_name = argv[1];

    if (pipe(pipe1) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        execl("./child", "child", file_name, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
        
    } else {
        close(pipe1[0]);
        printf("enter numbers: \n");
        char input[256];
        while (fgets(input, sizeof(input), stdin)) {
            if (strcmp(input, "\n") == 0 || strcmp(input, "\r\n") == 0) {
                break;
            }
            write(pipe1[1], input, strlen(input));
        }
        close(pipe1[1]);
        wait(NULL);
    }

    return 0;
}
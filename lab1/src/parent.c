
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pipe1[2];
    pid_t pid;
    char file_name[100];

    if (pipe(pipe1) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    printf("enter filename: ");
    scanf("%s", file_name);
    getchar();

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
    }
    else {
        close(pipe1[0]);
        char input[256];
        printf("enter numbers: \n");
        while (strcmp(input, "\0") != 0) {
            fgets(input, sizeof(input), stdin);
            write(pipe1[1], input, strlen(input));
        }
        close(pipe1[1]);
        wait(NULL);
    }

    return 0;
}
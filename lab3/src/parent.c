#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_NUMS 100
#define SHM_NAME "/my_shared_memory"

typedef struct {
    int numbers[MAX_NUMS];
    int count;
    int terminate;
} shared_data;

shared_data *data;
char *filename;

void handle_division_by_zero(int sig) {
    printf("Обнаружено деление на ноль. Завершение процессов.\n");
    if (data) {
        data->terminate = 1;
    }
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    pid_t pid;

    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя файла>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    filename = argv[1];

    struct sigaction sa;
    sa.sa_handler = handle_division_by_zero;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGFPE, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(shared_data)) == -1) {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    data = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    close(shm_fd);

    data->count = 0;
    data->terminate = 0;

    pid = fork();
    if (pid < 0) {
        perror("fork");
        munmap(data, sizeof(shared_data));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        execl("./child", "child", filename, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        while (!data->terminate) {
            printf("Введите числа через пробел и нажмите Enter: ");
            char buffer[1024];
            if (!fgets(buffer, sizeof(buffer), stdin)) {
                perror("fgets");
                data->terminate = 1;
                break;
            }
            data->count = 0;
            char *token = strtok(buffer, " \n");
            while (token && data->count < MAX_NUMS) {
                errno = 0;
                int num = strtol(token, NULL, 10);
                if (errno != 0) {
                    perror("strtol");
                    data->terminate = 1;
                    break;
                }
                data->numbers[data->count++] = num;
                token = strtok(NULL, " \n");
            }

            if (data->terminate) {
                break;
            }
        }

        wait(NULL);
        munmap(data, sizeof(shared_data));
        shm_unlink(SHM_NAME);
    }

    return 0;
}
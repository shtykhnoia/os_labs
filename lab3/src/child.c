#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
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
    printf("Обнаружено деление на ноль в дочернем процессе. Завершение.\n");
    if (data) {
        data->terminate = 1;
    }
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
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

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    data = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    close(shm_fd); 
    while (!data->terminate) {
        if (data->count > 0) {
            int dividend = data->numbers[0];
            FILE *fp = fopen(filename, "w");
            if (!fp) {
                perror("fopen");
                kill(getppid(), SIGFPE);
                exit(EXIT_FAILURE);
            }
            for (int i = 1; i < data->count; i++) {
                if (data->numbers[i] == 0) {
                    fclose(fp);
                    kill(getppid(), SIGFPE);
                    exit(EXIT_FAILURE);
                }
                fprintf(fp, "%d / %d = %d\n", dividend, data->numbers[i], dividend / data->numbers[i]);
            }
            fclose(fp);
            data->count = 0;
        }
        sleep(1); 
    }

    munmap(data, sizeof(shared_data));
    return 0;
}
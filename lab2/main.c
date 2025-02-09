#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#define MAX_SIZE 100

sem_t semaphore;
pthread_mutex_t lock;

void get_minor(int n, double src[MAX_SIZE][MAX_SIZE], double minor[MAX_SIZE][MAX_SIZE], int row, int col){
    int k, l;
    k = l = 0;

    for(int i = 0; i < n; i++){
        if(i == row) continue;

        l = 0;
        for(int j = 0; j < n; j++){
            if (j == col) continue;
            minor[k][l] = src[i][j];
            l++; 
        }
        k++;
    }
}

typedef struct {
    int n;
    int row;
    int col;
    double* res;
    double matrix[MAX_SIZE][MAX_SIZE];
    int sign;
} MinorArgs;

double determinant(int n, double matrix[MAX_SIZE][MAX_SIZE]);

void *calculate_minor(void* args) {
    MinorArgs* minorArgs = (MinorArgs*) args;
    double minor[MAX_SIZE][MAX_SIZE];
    get_minor(minorArgs->n, minorArgs->matrix, minor, minorArgs->row, minorArgs->col);

    double det = determinant(minorArgs->n - 1, minor);

    pthread_mutex_lock(&lock);
    *(minorArgs->res) += minorArgs->sign * minorArgs->matrix[0][minorArgs->col] * det;
    pthread_mutex_unlock(&lock);

    sem_post(&semaphore);

    free(args);

    return NULL;
}

double determinant(int n, double matrix[MAX_SIZE][MAX_SIZE]){
    if(n == 1) return matrix[0][0];
    if (n == 2) return matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];

    double result = 0.0;
    pthread_t threads[n];

    for(int col = 0; col < n; col++){
        sem_wait(&semaphore);
        MinorArgs *args = malloc(sizeof(MinorArgs));
        args->n = n;
        args->row = 0;
        args->col = col;
        args->sign = (col % 2 == 0) ? 1 : -1;
        args->res = &result;

        for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                args->matrix[i][j] = matrix[i][j];
        
        if (pthread_create(&threads[col], NULL, calculate_minor, args) != 0){
            perror("Не удалось создать поток");
            exit(1);
        }
    }

    for (int i = 0; i < n; ++i) {
        pthread_join(threads[i], NULL);
    }

    return result;

}

int main(int argc, char* argv[]) {
    if (argc != 2){
        fprintf(stderr, "Usage: %s <count of threads>", argv[0]);
        exit(1);
    }
    int max_threads = atoi(argv[1]);
    sem_init(&semaphore, 0, max_threads);
    pthread_mutex_init(&lock, NULL);

    int n;
    printf("Введите размер матрицы: \n");
    scanf("%d", &n);

    double matrix[MAX_SIZE][MAX_SIZE]; 

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("matrix[%d][%d] = ", i + 1, j + 1);
            scanf("%lf", &matrix[i][j]);
            printf("\n");
        }
    }     

    double det = determinant(n, matrix);

    printf("determinant = %lf", det);

    sem_destroy(&semaphore);
    pthread_mutex_destroy(&lock);

    return 0;
}
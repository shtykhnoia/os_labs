#include <stdio.h>
#include <dlfcn.h>

int GCF(int, int);
float E(int);

int main() {
    int choice;
    int arg1, arg2;
    while (1) {
        printf("Введите команду: ");
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Ошибка чтения команды\n");
            return 1;
        }
        if (choice == 0) {
            printf("Команда переключения не поддерживается в программе No1\n");
        } else if (choice == 1) {
            if (scanf("%d %d", &arg1, &arg2) != 2) {
                fprintf(stderr, "Ошибка чтения аргументов для GCF\n");
                return 1;
            }
            printf("Результат GCF(%d, %d) = %d\n", arg1, arg2, GCF(arg1, arg2));
        } else if (choice == 2) {
            if (scanf("%d", &arg1) != 1) {
                fprintf(stderr, "Ошибка чтения аргумента для E\n");
                return 1;
            }
            printf("Результат E(%d) = %f\n", arg1, E(arg1));
        } else if (choice == 3) {
            return 0;
        } else {
            printf("Неверная команда\n");
        }
    }
    return 0;
}
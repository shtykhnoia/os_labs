#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

// Прототипы функций
typedef int (*GCF_t)(int, int);
typedef float (*E_t)(int);

int main() {
    void *lib_handle = dlopen("./libimpl1.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }

    GCF_t GCF = (GCF_t) dlsym(lib_handle, "GCF");
    E_t E = (E_t) dlsym(lib_handle, "E");

    int choice;
    int arg1, arg2;

    while (1) {
        printf("Введите команду: ");
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Ошибка чтения команды\n");
            return 1;
        }
        if (choice == 0) {
            dlclose(lib_handle);
            lib_handle = dlopen("./libimpl2.so", RTLD_LAZY);
            if (!lib_handle) {
                fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
                return 1;
            }
            GCF = (GCF_t) dlsym(lib_handle, "GCF");
            E = (E_t) dlsym(lib_handle, "E");
            printf("Переключение библиотеки\n");
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
        }else if (choice == 3) {
            dlclose(lib_handle);
            return 0;
            }
        else {
            printf("Неверная команда\n");
        }
    }

    dlclose(lib_handle);
    return 0;
}
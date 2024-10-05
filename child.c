#include <stdlib.h>
#include <string.h>
#include "stdio.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *file_name = argv[1];

    FILE *file = fopen(file_name, "w");

    if (file == NULL) {
        perror("File open failed");
        exit(EXIT_FAILURE);
    }

    double numbers[256];
    char input[256];

    while (strcmp(input, "\n") != 0) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Reading from pipe failed");
            break;
        }

        int count = 0;
        char *token = strtok(input, " ");
        while (token != NULL) {
            numbers[count++] = atof(token);
            token = strtok(NULL, " ");
        }

        if (count < 2) {
            fprintf(stderr, "Invalid input\n");
            exit(EXIT_FAILURE);
        }

        double result = numbers[0];

        for (int i = 1; i < count; i++) {
            if (numbers[i] == 0) {
                fprintf(file, "Error: division by zero\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            result /= numbers[i];
        }

        fprintf(file, "Result of division: %.2f\n", result);
        fclose(file);
        break;
    }
    return 0;
}

// impl1.c
#include <stdio.h>
#include <math.h>

// Реализация Евклидова алгоритма для нахождения НОД
int GCF(int A, int B) {
    while (B != 0) {
        int t = B;
        B = A % B;
        A = t;
    }
    return A;
}

// Реализация вычисления числа e
float E(int x) {
    return pow((1 + 1.0/x), x);
}
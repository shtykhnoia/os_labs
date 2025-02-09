#include <stdio.h>

int GCF(int A, int B) {
    int gcd = 1;
    for (int i = 1; i <= A && i <= B; ++i) {
        if (A % i == 0 && B % i == 0) {
            gcd = i;
        }
    }
    return gcd;
}

float E(int x) {
    float e = 1.0;
    float factorial = 1.0;
    for (int i = 1; i <= x; ++i) {
        factorial *= i;
        e += 1.0 / factorial;
    }
    return e;
}
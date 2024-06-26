#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define matrix dimensions
#define M 32
#define N 64
#define P 128

// Perform matrix multiplication C = A * B
void matrix_multiply(float A[M][N], float B[N][P], float C[M][P]) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    float A[M][N];
    float B[N][P];
    float C[M][P];

    // Seed the random number generator with current time
    srand(time(NULL));

    // Initialize matrices A and B with random integers
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 101; // random integer between 0 and 100
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            B[i][j] = rand() % 101; // random integer between 0 and 100
        }
    }

    // Perform matrix multiplication
    matrix_multiply(A, B, C);

    return 0;
}

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <iostream>
#include <time.h>

#define MAX 105

#define R 0
#define G 1
#define B 2


const int is[] = {0, 1, 0, -1};
const int js[] = {1, 0, -1, 0};

int n, m, compM;
std::atomic <int> mat[MAX][MAX][3];
std::atomic <int> mat2[MAX][MAX][3];

void read_matrix (char* entrada) {
    FILE *in = fopen(entrada, "r");

    fscanf (in, "%d %d %d", &n, &m, &compM);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            int r, g, b;
            fscanf (in, "%d %d %d", &r, &g, &b);
            mat[i][j][R] = mat2[i][j][R] = r;
            mat[i][j][G] = mat2[i][j][G] = g;
            mat[i][j][B] = mat2[i][j][B] = b;
        }
}

void operacao (int i, int j) {
    mat2[i][j][R]--;
    mat2[i][j][G]--;
    mat2[i][j][B]--;
    for (int k = 0; k < 4; k++) {
        // Verficar se e melhor fazer o if assim ou ao contrario
        // para manter o pipeline
        if (!(i + is[k] < 0 || i + is[k] >= n || j + js[k] < 0 || j + js[k] >= m)) {
            mat2[i + is[k]][j + js[k]][R]++;
            mat2[i + is[k]][j + js[k]][G]++;
            mat2[i + is[k]][j + js[k]][B]++;
        }
    }
}

int main (int argc, char** argv) {
    if (argc != 5) {
        fprintf (stderr, "Uso: ./ep  <entrada> <saida> <iter> <num CPU>\n");
        return EXIT_FAILURE;
    }

    //read_matrix (argv[1]);
    int iter = atoi (argv[3]);
    int num_threads = atoi (argv[4]);

    // OpenMP initialization
    omp_set_num_threads (num_threads);

    n = m = 10;


    for (int it = 0; it < iter; it++)
#pragma omp parallel for
        for (int i = 0; i < n; i++) 
            for (int j = 0; j < m; j++) 
                operacao (i, j);

    //for (int i = 0; i < n; i++) { 
        //for (int j = 0; j < m; j++) 
            //std::cout << mat2[i][j][0] << " ";
        //std::cout << std::endl;
    //}

    return EXIT_SUCCESS;
}

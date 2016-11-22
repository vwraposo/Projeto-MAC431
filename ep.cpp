#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <cmath>

#define MAX 4005
#define space << " " << 

#define MU 1000000
#define R 0
#define G 1
#define B 2

// Nao sabemos o que fazer, deixar 255 com atomic<int> perde muita precisao 
// mas nao tem atomic double, entao talvez usar critical do open mp

typedef unsigned long long ull;

const int is[] = {0, 1, 0, -1};
const int js[] = {1, 0, -1, 0};

int n, m, compM;
std::atomic<ull> mat[MAX][MAX][3];
std::atomic<ull> mat2[MAX][MAX][3];

void read_im (char* entrada) {
    FILE *in = fopen (entrada, "r");
    char mn[2];

    fscanf (in, "%s %d %d %d", &mn, &n, &m, &compM);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            int r, g, b;
            fscanf (in, "%d %d %d", &r, &g, &b);
            mat[i][j][R] = mat2[i][j][R] = r * MU;
            mat[i][j][G] = mat2[i][j][G] = g * MU;
            mat[i][j][B] = mat2[i][j][B] = b * MU;
        }

    fclose (in);
}

void print_im (char *saida) {
    FILE *out = fopen (saida, "w");

    fprintf (out, "P3 %d %d 255\n", n, m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int r, g, b;
            r = mat[i][j][R] / MU;
            b = mat[i][j][G] / MU;
            g = mat[i][j][B] / MU;

            fprintf (out, "%d %d %d   ", r, g, b);
        }
        fprintf (out, "\n");
    }

    fclose (out);
}

void operacao (int i, int j) {
    double angle = 2 * M_PI * mat[i][j][G] / (255. * MU);
    double s = abs (sin (angle));
    double c = abs (cos (angle));
    double send[4];
    int send_cor[4];

    /* std::cout << s space c << std::endl; */

    if (angle >= 0 && angle <= M_PI) {
        // Vermelho na direita, azul na esquerda
        send[0] = mat[i][j][R] * s; send_cor[0] = R;
        send[2] = mat[i][j][B] * s; send_cor[2] = B;
    } else {
        // Azul na direita, vermelho na esquerda
        send[0] = mat[i][j][B] * s; send_cor[0] = B;
        send[2] = mat[i][j][R] * s; send_cor[2] = R;
    }
    if (angle >= M_PI_2 && angle <= 3 * M_PI_2) {
        // Vermelho está embaixo, azul em cima
        send[1] = mat[i][j][R] * c; send_cor[1] = R;
        send[3] = mat[i][j][B] * c; send_cor[3] = B;
    } else {
        // Vermelho está em cima, azul está embaixo
        send[1] = mat[i][j][B] * c; send_cor[1] = B;
        send[3] = mat[i][j][R] * c; send_cor[3] = R;
    }

    for (int k = 0; k < 4; k++) {
        int nx = i + is[k];
        int ny = j + js[k];
        if (!(nx < 0 || nx >= n || ny < 0 || ny >= m)) {
            ull delta = (255 * MU - mat[nx][ny][send_cor[k]]) * send[k] / (1020 * MU);  
            mat2[i][j][send_cor[k]].fetch_sub (delta);
            mat2[nx][ny][send_cor[k]].fetch_add (delta);
        }
    }

}

ull getGreen (int i, int j) {      
    double angle = M_PI_2 - atan2 ((int) mat2[i][j][B], (int) mat2[i][j][R]); 
    /* std::cout << angle << std::endl; */
    return ((ull) (angle * 255 * MU / (2 * M_PI)) + mat[i][j][G]) % (255 * MU);
}


int main (int argc, char** argv) {
    std::ios::sync_with_stdio (false);
    if (argc != 5) {
        fprintf (stderr, "Uso: ./ep  <entrada> <saida> <iter> <num CPU>\n");
        return EXIT_FAILURE;
    }

    read_im (argv[1]);
    int iter = atoi (argv[3]);
    int num_threads = atoi (argv[4]);

    // OpenMP initialization
    omp_set_num_threads (num_threads);

    for (int it = 0; it < iter; it++) {
        // Mecher no jeito de iterar
        #pragma omp parallel for
        for (int i = 0; i < n; i++) 
            for (int j = 0; j < m; j++) 
                operacao (i, j);
        
        #pragma omp parallel for
        for (int i = 0; i < n; i++) 
            for (int j = 0; j < m; j++) {
                mat[i][j][R].store (mat2[i][j][R]);
                mat[i][j][B].store (mat2[i][j][B]);
                mat[i][j][G].store (getGreen (i, j));
            }
    }

    std::cout << "RED" << std::endl;
    for (int i = 0; i < n; i++) { 
        for (int j = 0; j < m; j++) 
            std::cout << mat[i][j][R] << " ";
        std::cout << std::endl;
    }
        std::cout << std::endl;
    std::cout << "BLUE" << std::endl;
    for (int i = 0; i < n; i++) { 
        for (int j = 0; j < m; j++) 
            std::cout << mat[i][j][B] << " ";
        std::cout << std::endl;
    }
        std::cout << std::endl;
    std::cout << "GREEN" << std::endl;
    for (int i = 0; i < n; i++) { 
        for (int j = 0; j < m; j++) 
            std::cout << mat[i][j][G] << " ";
        std::cout << std::endl;
    }

    print_im (argv[2]);

    return EXIT_SUCCESS;
}

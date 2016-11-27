#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <cmath>

#define MAX 4005

#define MU 1000000
#define R 0
#define G 1
#define B 2

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


    // Unroll
    int ret = 0;

    if (j+1 < m) {
        ull delta = (255 * MU - mat[i][j+1][send_cor[0]]) * send[0] / (1020 * MU);
        mat2[i][j][send_cor[0]].fetch_sub (delta);
        mat2[i][j+1][send_cor[0]].fetch_add (delta);

        if (mat2[i][j+1][send_cor[0]] > 255 * MU) ret = 1;
    }

    if (i+1 < n) {
        ull delta = (255 * MU - mat[i+1][j][send_cor[1]]) * send[1] / (1020 * MU);
        mat2[i][j][send_cor[1]].fetch_sub (delta);
        mat2[i+1][j][send_cor[1]].fetch_add (delta);

        if (mat2[i+1][j][send_cor[1]] > 255 * MU) ret = 1;
    }

    if (j >= 1) {
        ull delta = (255 * MU - mat[i][j-1][send_cor[2]]) * send[2] / (1020 * MU);
        mat2[i][j][send_cor[2]].fetch_sub (delta);
        mat2[i][j-1][send_cor[2]].fetch_add (delta);

        if (mat2[i][j-1][send_cor[2]] > 255 * MU) ret = 1;
    }

    if (i >= 1) {
        ull delta = (255 * MU - mat[i-1][j][send_cor[3]]) * send[3] / (1020 * MU);
        mat2[i][j][send_cor[3]].fetch_sub (delta);
        mat2[i-1][j][send_cor[3]].fetch_add (delta);

        if (mat2[i-1][j][send_cor[3]] > 255 * MU) ret = 1;
    }

    return ret;
}

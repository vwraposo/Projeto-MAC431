/*******************************************************************/
/*                    Projeto de MAC0431                           */
/* Nomes:                                                          */
/* Leonardo Padilha                             Nº USP: 9298295    */
/* Lucas Santos                                 Nº USP:            */
/* Victor da Matta                              Nº USP:            */
/* Victor Raposo                                Nº USP:            */
/*******************************************************************/
#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string.h>


#define MAX 4005   /* Tamanho máximo de largura ou altura da imagem*/

#define MU 1000000 /* Valor do multiplicador */
#define R 0        /* Indíce para o vermelho */
#define G 1        /* Indíce para o verde    */
#define B 2        /* Indíce para o azul     */

typedef unsigned long long ull;

/* Variáveis globais */
const int is[] = {0, 1, 0, -1};
const int js[] = {1, 0, -1, 0};

int n, m, compM;
std::atomic<ull> mat[MAX][MAX][3];
std::atomic<ull> mat2[MAX][MAX][3];

/* read_im: recebe uma string entrada e le a imagem em formato  */
/* .ppm cujo nome é entrada e a salva na matriz global mat.     */
void read_im (char* entrada) {
    FILE *in = fopen (entrada, "r");
    int c, compare;
    char mn[2];

    // Leitura do formato da imagem ('P3' ou 'P6')
    if (fscanf (in, "%s ", mn) != 1) {
        fprintf(stderr, "ERROR!\n");
        exit (0);
    }

    if (strcmp (mn, "P6") != 0 && strcmp (mn, "P3") != 0) {
        fprintf(stderr, "ERROR: invalid image format\n");
        exit (0);
    }

    // Verificação de comentários
    c = getc(in);
    while (c == '#') {
        while (getc(in) != '\n') ;
        c = getc(in);
    }
    ungetc (c, in);

    if (fscanf (in, "%d %d %d", &n, &m, &compM) != 3) {
        fprintf(stderr, "ERROR: invalid image size!\n");
        exit (0);
    }

    // Retirando os espaços em branco
    c = getc(in);
    while (c == ' ' || c == '\t' || c == '\n')
        c = getc(in);
    ungetc (c, in);

    // Salvamos o valor da comparação do magic number com a string P6
    compare = strcmp ("P6", mn); 

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            int scan_ret = 0;

            // Se o tipo da imagem for P6
            if (compare == 0) {
                unsigned char r, g, b;
                scan_ret += fread(&r, 1, 1, in);
                scan_ret += fread(&g, 1, 1, in);
                scan_ret += fread(&b, 1, 1, in);

                if (scan_ret != 3) {  
                    fprintf(stderr, "ERROR: invalid pixel!\n");
                    exit (0);
                }


                // Salvamos na matriz mat e mat2 o valor * MU
                //para usar as operações atomicas do std::atomic
                //sem precisar declarar como double
                mat[i][j][R] = mat2[i][j][R] = r * MU;
                mat[i][j][G] = mat2[i][j][G] = g * MU;
                mat[i][j][B] = mat2[i][j][B] = b * MU;
            }
            // Se o tipo da imagem for P3
            else {
                int r, g, b;
                scan_ret = fscanf (in, "%d %d %d", &r, &g, &b);

                if (scan_ret != 3) {
                    fprintf(stderr, "ERROR: invalid pixel!\n");
                    exit (0);
                }

                // Salvamos na matriz mat e mat2 o valor * MU
                //para usar as operações atomicas do std::atomic
                //sem precisar declarar como double
                mat[i][j][R] = mat2[i][j][R] = r * MU;
                mat[i][j][G] = mat2[i][j][G] = g * MU;
                mat[i][j][B] = mat2[i][j][B] = b * MU;
            }
        }

    fclose (in);
}

/* print_im: recebe uma string saida e imprime uma imagem em formato */
/*.ppm cujo conteúdo é o resultado da matriz mat.                    */
void print_im (char *saida) {
    FILE *out = fopen (saida, "w");

    fprintf (out, "P3 %d %d 255\n", n, m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int r, g, b;
            // Normalizamos os valores para 0-255
            r = mat[i][j][R] / MU;
            b = mat[i][j][G] / MU;
            g = mat[i][j][B] / MU;

            fprintf (out, "%d %d %d   ", r, g, b);
        }
        fprintf (out, "\n");
    }

    fclose (out);
}

/* operacao: recebe um inteiro i e um j e realiza a operação dada */
/*no enunciado do projeto no pixel (i,j) da imagem. Retorna 0 se  */
/*a operação foi executada corretamente ou 1 se houve overflow no */
/*pixel.                                                          */
int operacao (int i, int j) {
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

    // Alteramos o valor das cores de cada um dos pixels vizinhos
    if (j+1 < m) {
        // delta é o valor a ser adicionado ao pixel
        ull delta = (255 * MU - mat[i][j+1][send_cor[0]]) * send[0] / (1020 * MU);
        mat2[i][j][send_cor[0]].fetch_sub (delta);
        mat2[i][j+1][send_cor[0]].fetch_add (delta);

        // se houve overflow, retornamos 1
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

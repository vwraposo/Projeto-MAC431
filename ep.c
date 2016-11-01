#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 5000

const int THREADS_PER_PROC = 2;
const int is[] = {0, 1, 0, -1};
const int js[] = {1, 0, -1, 0};

int mat[SIZE][SIZE][3];
int mat2[SIZE][SIZE][3];
pthread_t tids[50];
pthread_mutex_t mut[SIZE][SIZE];
int n, m;
int iter, threads;
int block_size;
int compM;

int free_line;
int die;
pthread_mutex_t line_mut;

void* run (void* p) {
    int id = p;
    while (1) {
        pthread_mutex_lock (&line_mut);
        if (free_line >= n && die == 0) {
            free_line = 2;
            die = 1;
        }
        else if (free_line >= n) die = 2;
        int line = free_line;
        free_line += 4;
        pthread_mutex_unlock (&line_mut);
        if (die == 2) return NULL;

        int end = line + block_size > n ? n : line + block_size;

        for (int i = line; i < end; i++) {
            for (int j = 0; j < m; j++) {
                pthread_mutex_lock (&mut[i][j]);
                mat2[i][j][0]--;
                pthread_mutex_unlock (&mut[i][j]);
                for (int k = 0; k < 4; k++) {
                    if (i + is[k] < 0 || i + is[k] >= n || j + js[k] < 0 || j + js[k] >= m) continue;
                    pthread_mutex_lock (&mut[i + is[k]][j + js[k]]);
                    mat2[i + is[k]][j + js[k]][0]++;
                    pthread_mutex_unlock (&mut[i + is[k]][j + js[k]]);
                }
            }
        }
    }
}

void read_matrix (char* entrada) {
    FILE *in = fopen(entrada, "r");

    fscanf (in, "%d %d %d", &n, &m, &compM);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            int r, g, b;
            fscanf (in, "%d %d %d", &r, &g, &b);
            mat[i][j][0] = mat2[i][j][0] = r;
            mat[i][j][1] = mat2[i][j][1] = g;
            mat[i][j][2] = mat2[i][j][2] = b;
        }
}

int main (int argc, char * argv[]) {
    //read_matrix (argv[1]);
    //iter = atoi (argv[3]);
    //threads = atoi (argv[4]) * THREADS_PER_PROC;
    if (argc < 2) {
        printf ("USE: ./ep <processadores>\n");
        return 1;
    }
    threads = atoi (argv[1]) * THREADS_PER_PROC;

    n = m = SIZE;

    block_size = 2; //problema em aberto
    free_line = 0;
    die = 0;
    pthread_mutex_init (&line_mut, NULL);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            pthread_mutex_init (&mut[i][j], NULL);
        }
    }

    for (int i = 0; i < threads; i++) {
        pthread_create (&tids[i], NULL, run, i);
    }

    for (int i = 0; i < threads; i++) pthread_join (tids[i], NULL);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
            printf ("%d ", mat2[i][j][0]);
        printf ("\n");
    }

    return 0;
}

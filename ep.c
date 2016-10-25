#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int THREADS_PER_PROC = 2;
const int is[] = {0, 1, 0, -1};
const int js[] = {1, 0, -1, 0};

int mat[500][500];
int mat2[500][500];
pthread_t tids[50];
pthread_mutex_t mut[500][500];
int n, m;
int iter, threads;
int block_size;

void * run (void * arg) {
    int line = arg;
    int end = line + block_size > n ? n : line + block_size;

    for (int i = line; i < end; i++) {
        for (int j = 0; j < m; j++) {
            pthread_mutex_lock (&mut[i][j]);
            mat[i][j]++;
            pthread_mutex_unlock (&mut[i][j]);
            for (int k = 0; k < 4; k++) {
                pthread_mutex_lock (&mut[i + is[k]][j + js[k]]);
                mat[i + is[k]][j + js[k]]++;
                pthread_mutex_unlock (&mut[i + is[k]][j + js[k]]);
            }
        }
    }
}

int main (int argc, char * argv[]) {
    readmat (argv[1]);
    iter = atoi (argv[3]);
    threads = atoi (argv[4]) * THREADS_PER_PROC;

    block_size = n / threads + 1; //problema em aberto

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            pthread_mutex_init (&mut[i][j], NULL);
            mat2[i][j] = mat[i][j];
        }
    }

    for (int i = 0; i < threads; i++) pthread_create (&tids[i], NULL, run, i * block_size);
    for (int i = 0; i < threads; i++) pthread_join (tids[i], NULL);

    return 0;
}

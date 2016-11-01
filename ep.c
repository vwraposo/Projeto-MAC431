#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int THREADS_PER_PROC = 2;
const int is[] = {0, 1, 0, -1};
const int js[] = {1, 0, -1, 0};

int mat[500][500][3];
int mat2[500][500][3];
pthread_t tids[50];
pthread_mutex_t mut[500][500];
int n, m;
int iter, threads;
int block_size;
int compM;

void* run (void* p) {
    int line = *((int *) p);
    int end = line + block_size > n ? n : line + block_size;

    for (int i = line; i < end; i++) {
        for (int j = 0; j < m; j++) {
            pthread_mutex_lock (&mut[i][j]);
            mat2[i][j][0]--;
            pthread_mutex_unlock (&mut[i][j]);
            for (int k = 0; k < 4; k++) {
                if (i + is[k] < 0 || i + is[k] >= n || j + js[k] < 0 || j + js[k] >= m)
                pthread_mutex_lock (&mut[i + is[k]][j + js[k]]);
                mat2[i + is[k]][j + js[k]][0]++;
                pthread_mutex_unlock (&mut[i + is[k]][j + js[k]]);
            }
        }
    }
    free (p);

    return NULL;
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
    threads = atoi (argv[1]) * THREADS_PER_PROC;

    int *aux;
    n = 4; m = 4;
    
    block_size = 2; //problema em aberto

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            pthread_mutex_init (&mut[i][j], NULL);
        }
    }

    for (int i = 0; i < threads; i++) {
        aux = malloc (sizeof (int));
        *aux = i * block_size;
        pthread_create (&tids[i], NULL, run, aux);
    }

    for (int i = 0; i < threads; i++) pthread_join (tids[i], NULL);

    for (int i = 0; i < n; i++) { 
        for (int j = 0; j < m; j++) 
            printf ("%d ", mat2[i][j][0]);
        printf ("\n");
    }

    return 0;
}

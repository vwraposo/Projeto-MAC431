#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define TRUE 1
#define FALSE 0
#define LINE_HEIGHT 10
#define THREADS_PER_PROC 3
#define SIZE 10005

const int is[] = {0, 1, 0, -1};
const int js[] = {1, 0, -1, 0};

int mat[SIZE][SIZE][3];
int mat2[SIZE][SIZE][3];
pthread_t tids[50];
//pthread_mutex_t mut[SIZE][SIZE];
pthread_mutex_t mut_next;
pthread_barrier_t barrier;
int n, m;
int iter, threads;
int compM;
int next1, next2;


void* run (void* p) {
    int line, end, i, j, k;

    while (TRUE) {
        pthread_mutex_lock (&mut_next);
        end = next1 >= n ? TRUE : FALSE; 
        line = next1;
        next1 += 2 * LINE_HEIGHT;
        pthread_mutex_unlock (&mut_next);
        if (end) break;

        i = line;
        while (i < n && i < line + LINE_HEIGHT) {
            for (j = 0; j < m; j++) {
                mat2[i][j][0]--;
                for (k = 0; k < 4; k++) {
                    if (i + is[k] < 0 || i + is[k] >= n || j + js[k] < 0 || j + js[k] >= m) continue;
		    mat2[i + is[k]][j + js[k]][0]++;
                }
            }
            i++;
        }
	
	sleep (0.1);

    }
    // BARRIERA
    pthread_barrier_wait (&barrier);

    while (TRUE) {
        pthread_mutex_lock (&mut_next);
        end = next2 >= n ? TRUE : FALSE; 
        line = next2;
        next2 += 2 * LINE_HEIGHT;
        pthread_mutex_unlock (&mut_next);
        if (end) break;

        i = line;
        while (i < n && i < line + LINE_HEIGHT) {
            for (j = 0; j < m; j++) {
                mat2[i][j][0]--;
                for (k = 0; k < 4; k++) {
                    if (i + is[k] < 0 || i + is[k] >= n || j + js[k] < 0 || j + js[k] >= m) continue;
		    mat2[i + is[k]][j + js[k]][0]++;
                }
            }
            i++;
        }

    }

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
	printf ("THREADS: %d\n", threads);

    n = 10000; 
    m = 10000;

    // Inicializacao
    pthread_mutex_init (&mut_next, NULL);
    pthread_barrier_init (&barrier, NULL, threads);
   // for (int i = 0; i < n; i++) {
     //   for (int j = 0; j < m; j++) {
       //     pthread_mutex_init (&mut[i][j], NULL);
      //  }
   // }
    next1 = 0;
    next2 = LINE_HEIGHT;

    for (int i = 0; i < threads; i++) {
        pthread_create (&tids[i], NULL, run, NULL);
    }

    for (int i = 0; i < threads; i++) pthread_join (tids[i], NULL);

    for (int i = 0; i < n; i++) { 
        for (int j = 0; j < m; j++) 
            printf ("%d ", mat2[i][j][0]);
        printf ("\n");
    }

    // Inicializacao
    //pthread_mutex_init (&next, NULL);
    //pthread_barrier_init (&barrier, NULL, threads);
    //for (int i = 0; i < n; i++) {
        //for (int j = 0; j < m; j++) {
            //pthread_mutex_init (&mut[i][j], NULL);
        //}

    return 0;
}

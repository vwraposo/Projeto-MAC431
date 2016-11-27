#include "header.hpp"

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

    std::atomic<int> over = 0;
    for (int it = 0; it < iter; it++) {
        // Mecher no jeito de iterar
        #pragma omp parallel for schedule (guided, 4)
        for (int i = 0; i < n; i++) 
            for (int j = 0; j < m; j++) 
                over.fetch_add (operacao (i, j));

        if (over > 0) { 
            #pragma omp parallel for schedule (guided, 4)
            for (int i = 0; i < n; i++) 
                for (int j = 0; j < m; j++) {
                    if (mat2[i][j][R] > 255 * MU) {
                        ull delta = mat2[i][j][R] - 255*MU / 4;
                        mat2[i][j][R].store (255*MU);
                        #pragma omp critcal 
                        {
                            if (j+1 < m && mat2[i][j+1][R] + delta > 255*MU)
                                mat2[i][j+1][R].fetch_add (delta);

                            if (i+1 < n && mat2[i+1][j][R] + delta > 255*MU)
                                mat2[i+1][j][R].fetch_add (delta);

                            if (j >= 1 && mat2[i][j-1][R] + delta > 255*MU)
                                mat2[i][j-1][R].fetch_add (delta);

                            if (i >= 1 && mat2[i-1][j][R] + delta > 255*MU) 
                                mat2[i-1][j][R].fetch_add (delta);
                        }
                    }
                    if (mat2[i][j][G] > 255 * MU) {
                        ull delta = mat2[i][j][G] - 255*MU / 4;
                        mat2[i][j][G].store (255*MU);
                        #pragma omp critcal 
                        {
                            if (j+1 < m && mat2[i][j+1][G] + delta > 255*MU)
                                mat2[i][j+1][G].fetch_add (delta);

                            if (i+1 < n && mat2[i+1][j][G] + delta > 255*MU)
                                mat2[i+1][j][G].fetch_add (delta);

                            if (j >= 1 && mat2[i][j-1][G] + delta > 255*MU)
                                mat2[i][j-1][G].fetch_add (delta);

                            if (i >= 1 && mat2[i-1][j][G] + delta > 255*MU) 
                                mat2[i-1][j][G].fetch_add (delta);
                        }
                    }
                    if (mat2[i][j][B] > 255 * MU) {
                        ull delta = mat2[i][j][B] - 255*MU / 4;
                        mat2[i][j][B].store (255*MU);
                        #pragma omp critcal 
                        {
                            if (j+1 < m && mat2[i][j+1][B] + delta > 255*MU)
                                mat2[i][j+1][B].fetch_add (delta);

                            if (i+1 < n && mat2[i+1][j][B] + delta > 255*MU)
                                mat2[i+1][j][B].fetch_add (delta);

                            if (j >= 1 && mat2[i][j-1][B] + delta > 255*MU)
                                mat2[i][j-1][B].fetch_add (delta);

                            if (i >= 1 && mat2[i-1][j][B] + delta > 255*MU) 
                                mat2[i-1][j][B].fetch_add (delta);
                        }
                    }
                }
        }



        #pragma omp parallel for schedule (guided, 4)
        for (int i = 0; i < n; i++) 
            for (int j = 0; j < m; j++) {
                mat[i][j][R].store (mat2[i][j][R]);
                mat[i][j][B].store (mat2[i][j][B]);
                double angle = M_PI_2 - atan2 ((int) mat2[i][j][B], (int) mat2[i][j][R]); 
                mat[i][j][G].store (((ull) (angle * 255 * MU / (2 * M_PI)) + mat[i][j][G]) % (255 * MU));
            }
    }
    print_im (argv[2]);

    return EXIT_SUCCESS;
}

#include "ep.hpp"

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
        #pragma omp parallel for schedule (guided, 4)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                operacao (i, j);

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

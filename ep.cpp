/*******************************************************************/
/*                    Projeto de MAC0431                           */
/* Nomes:                                                          */
/* Leonardo Padilha                             Nº USP: 9298295    */
/* Lucas Santos                                 Nº USP: 9345064    */
/* Victor da Matta                              Nº USP: 9298145    */
/* Victor Raposo                                Nº USP: 9298020    */
/*******************************************************************/
#include "ep.hpp"

int main (int argc, char** argv) {
    std::ios::sync_with_stdio (false);

    // Verificamos se os argumentos estão corretos 
    if (argc != 5) {
        fprintf (stderr, "Uso: ./ep  <entrada> <saida> <iter> <num CPU>\n");
        return EXIT_FAILURE;
    }

    // Leitura da imagem cujo nome é dado pelo primeiro argumento 
    read_im (argv[1]);

    // Quantidade de iterações 
    int iter = atoi (argv[3]);

    // Número de threads utilizadas
    int num_threads = atoi (argv[4]);

    // Inicialização do OpenMP 
    omp_set_num_threads (num_threads);

    // Declaração de um inteiro para registrar overflow
    std::atomic<int> over;
    over.store (0);

    for (int it = 0; it < iter; it++) {

        // Executamos, paralelamente e com balanceamento de
        //carga guided (tamanho mínimo de cada chunk de 4), a 
        //operação em cada um dos pixels da matriz mat e joga-
        //mos os resultados em mat2
        #pragma omp parallel for schedule (guided, 4)
        for (int i = 0; i < n; i++) 
            for (int j = 0; j < m; j++) 
                over.fetch_add (operacao (i, j));

        // Se houver overflow em alguns dos pixels
        if (over > 0) { 
            // Verificamos em cada pixel qual houve overflow
            //e corrigimos o valor dele
            #pragma omp parallel for schedule (guided, 4)
            for (int i = 0; i < n; i++) 
                for (int j = 0; j < m; j++) {
                    // Se o valor do vermelho deu overflow
                    if (mat2[i][j][R] > 255 * MU) {
                        ull delta = mat2[i][j][R] - 255*MU / 4;
                        mat2[i][j][R].store (255*MU);

                        // Distribuimos o valor que sobrou
                        //entre os elementos adjacentes
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
                    // Se o valor do verde deu overflow
                    if (mat2[i][j][G] > 255 * MU) {
                        ull delta = mat2[i][j][G] - 255*MU / 4;
                        mat2[i][j][G].store (255*MU);

                        // Distribuimos o valor que sobrou
                        //entre os elementos adjacentes
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
                    // Se o valor do azul deu overflow
                    if (mat2[i][j][B] > 255 * MU) {
                        ull delta = mat2[i][j][B] - 255*MU / 4;
                        mat2[i][j][B].store (255*MU);

                        // Distribuimos o valor que sobrou
                        //entre os elementos adjacentes
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

        // Salvamos o conteúdo de mat2 na matriz mat e ajustamos
        //o valor da componente verde de acordo com o valor do azul
        //e do vermelho.
        // Esse laço também é paralelizado
        #pragma omp parallel for schedule (guided, 4)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) {
                mat[i][j][R].store (mat2[i][j][R]);
                mat[i][j][B].store (mat2[i][j][B]);
                double angle = M_PI_2 - atan2 ((int) mat2[i][j][B], (int) mat2[i][j][R]);
                mat[i][j][G].store (((ull) (angle * 255 * MU / (2 * M_PI)) + mat[i][j][G]) % (255 * MU));
            }
    }
    // Imprimimos a imagem resultante
    print_im (argv[2]);

    return EXIT_SUCCESS;
}

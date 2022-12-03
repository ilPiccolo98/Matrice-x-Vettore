#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>  
#include <sys/time.h>
#define MATRIX_ROWS 5
#define MATRIX_COLUMNS 5
#define INDEX_ROWS 1
#define INDEX_COLUMNS 2
#define INDEX_HIDE_INIT_OUTPUT 3
#define INDEX_HIDE_LAST_OUTPUT 4
#define MIN_RANDOM_NUMBER 10000
#define MAX_RANDOM_NUMBER -10000

// ----------------------------------------------------------------------------------------------------
// | SCOPO                                                                                            
// | PROGRAMMA PER ESEGUIRE IL PRODOTTO TRA MATRICE E VETTORE IN PARALLELO SU UN'ARCHITETTURA A MEMORIA 
// | CONDIVISA TRAMITE LA LIBRERIA OPENMP. IL PROGRAMMA PUO' ESEGUIRE QUESTA OPERAZIONE CON MATRICI E 
// | VETTORI INDICANDO LA DIMENSIONE DI QUEST'ULTIMI. QUINDI PRIMA DI ESEGUIRE IL PROGRAMMA, IN INGRESSO
// | E' NECESSARIO INDICARE IL NUMERO DI RIGHE E IL NUMERO DI COLONNE DI QUESTI 2 OGGETTI. SUCCESSIVAMENTE
// | IL PROGRAMMA SI OCCUPERA' DELLA CREAZIONE DELLA MATRICE E DEL VETTORE CON VALORI CASUALI. IL PASSO
// | SUCCESSIVO SARA' QUELLO DI ESEGUIRE L'OPERAZIONE DI PRODOTTO TRA MATRICE E VETTORE. COME GIA' 
// | ANTICIPATO, VERRA' ESEGUITA IN PARALLELO USUFRUENDO DELLE FUNZIONALITA' DELLA LIBRERIA OPENMP, 
// | SICCOME L'ALGORITMO HA UNA ARCHITETTURA A MEMORIA CONDIVISA, LE OPERAZIONI PARALLELIZZATE SARANNO
// | GESTITE DA UN NUMERO PRE-STABILITO DI THREAD. ESEGUITO IL CALCOLO DEL PRODOTTO, IL RISULTATO FINALE
// | VERRA' STAMPATO DAL THREAD PRINCIPALE. NEL CASO IN CUI CI SONO ERRORI, IL PROGRAMMA TERMINERA' SENZA
// | SVOLGERE L'OPERAZIONE DI MOLTIPLICAZIONE RESTITUENDO UN MESSAGGIO DI ERRORE NEL FLUSSO DI ERRORE.                         
// ----------------------------------------------------------------------------------------------------
// | ISTRUZIONI PER L'USO                                                                             
// | IL PRIMO PASSO PER L'ESECUZIONE DELL'ALGORITMO E' LA COMPILAZIONE DEL SORGENTE VIENE EFFETTUATA
// | TRAMITE IL COMANDO: gcc -fopenmp -lgomp -o percorso/nome_file_output percorso/main.c  
// | E SUCCESSIVAMENTE BISOGNA ESEGUIRE IL PROGRAMMA A RIGA DI COMANDO CON I VARI PARAMETRI. IN TOTALE I 
// | PARAMETRI DA ESEGUIRE SONO 4. IL PRIMO INDICA IL NUMERO DI RIGHE DELLA MATRICE, IL SECONDO IL NUMERO
// | DI COLONNE DELLA MATRICE E DI CONSEGUENZA IL NUMERO DI COMPONENTI DEL VETTORE, IL TERZO INDICA SE SI
// | VUOLE STAMPARE O MENO LA MATRICE E IL VETTORE GENERATI E IL QUARTO INDICA SE SI VUOLE STAMPARE IL
// | PRODOTTO CALCOLATO. QUESTO E' UN ESEMPIO DI ESECUZIONE DEL PROGRAMMA A RIGA DI COMAND0:
// | percorso_file/nome_eseguibile NUMERO_RIGHE NUMERO_COLONNE NASCONDI_STAMPA_INIZIALE NASCONDI_STAMPA_FINALE
// | I PARAMETRI NUMERO_RIGHE E NUMERO_COLONNE DEVONO AVERE COME VALORE UN INTERO MAGGIORE DI ZERO, MENTRE
// | I PARAMETRI NASCONDI_STAMPA_INIZIALE E NASCONDI_STAMPA_FINALE POSSONO AVERE COME VALORE O 0 O 1.
// | ALTRIMENTI SI AVRA' UN MESSAGGIO DI ERRORE NEL FLUSSO DI ERRORI E IL PROGRAMMA TERMINERA' PRIMA
// | DELLA FINE DELL'ESECUZIONE.                              
// ----------------------------------------------------------------------------------------------------
// | ESEMPI D'USO                                                                                     
// | CON IL SEGUENTE INPUT IL PROGRAMMA GENERERA' UNA MATRICE CON 10 RIGHE E 10 COLONNE E UN VETTORE 
// | DI DIMENSIONE 10, STAMPERA' L'OUTPUT INIZIALE E FINALE E IL TUTTO VERRA' ESEGUITO CON 6 THREAD
// | INPUT INSERITO: 
// | #!/bin/bash
// | 
// | #PBS -q studenti
// | #PBS -l nodes=1:ppn=8
// | #PBS -N matrix_by_vector_show_input_and_output
// | #PBS -o matrix_by_vector_show_input_and_output.out
// | #PBS -e matrix_by_vector_show_input_and_output.err
// | 
// | cat $PBS_NODEFILE
// | 
// | PBS_O_WORKDIR=$PBS_O_HOME/matrix_by_vector_project
// | 
// | export OMP_NUM_THREADS=6
// | export PSC_OMP_AFFINITY=TRUE
// | gcc -fopenmp -lgomp -o $PBS_O_WORKDIR/matrix_by_vector $PBS_O_WORKDIR/main.c
// | 
// | $PBS_O_WORKDIR/matrix_by_vector 10 10 0 0
// | OUTPUT GENERATO:
// ------------------------------------------------------
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// |
// | GENERATED MATRIX----------------------------------
// | Row: 1
// | 9868.199062 9735.769536 7624.452710 5404.026210 -5043.940544 557.348132 -8841.720819 -3681.149483 7345.121801 700.200200 
// | Row: 2
// | -9920.681715 3945.401907 4026.206732 -2199.237347 -9284.671545 -7589.052916 4344.890714 7062.475681 -1890.122890 586.915016 
// | Row: 3
// | 9366.082251 6589.429975 2071.467042 -9648.523331 977.530479 1241.764426 8604.573160 -4083.974361 2488.842010 -8578.240871 
// | Row: 4
// | 7020.442188 2357.040644 -8842.471838 4644.894600 -2238.932848 -3886.412382 -4797.756672 -1080.653667 2432.438135 -7452.635765 
// | Row: 5
// | 9619.546421 2511.755824 6492.766440 3645.753264 -9687.480927 7208.095193 6056.700349 4657.409191 4270.570874 -5833.421946 
// | Row: 6
// | -4755.675793 3636.653423 -9243.991375 7315.791547 3988.130093 1733.538508 -1442.444324 2592.703104 7649.563849 -8953.602314 
// | Row: 7
// | 4014.462233 4670.006037 3403.438330 5171.990395 -685.099363 -8835.494518 -8714.421988 4517.143965 83.851814 3718.016148 
// | Row: 8
// | 7064.508498 -296.602249 -3770.227432 3557.274938 -6650.848389 -3457.709551 765.370131 9405.851923 -8800.299168 -4964.058399 
// | Row: 9
// | -6427.570581 -3555.974960 8672.594428 -5671.561956 -6240.183115 2660.724521 6061.976552 2317.371964 -4746.571779 3711.540103 
// | Row: 10
// | 3363.769650 9267.889708 -1618.453264 -3232.791424 4439.880252 7696.446925 -2068.285942 5725.457966 2213.590741 8015.565872 
// |
// | GENERATED VECTOR----------------------------------
// | -556.526184
// | -721.900463
// | -2281.036377
// | 5673.246682
// | -7164.626122
// | 1068.115830
// | -7784.461975
// | 3600.744605
// | 473.967195
// | -6584.762335
// |
// |
// | PRODUCT OF THE OPERATION
// | 91923667.858130
// | 26274386.928470
// | -99132233.750462
// | 136486530.019984
// | 85870308.563985
// | 119036986.216080
// | 71103985.747215
// | 125447632.846609
// | -63798251.341364
// | -61815599.138215
// |
// | Operation executed in 0.003164 seconds
// ----------------------------------------------------------------------------------------------------
// | CON IL SEGUENTE INPUT IL PROGRAMMA GENERERA' UNA MATRICE CON 10 RIGHE E 10 COLONNE E UN VETTORE 
// | DI DIMENSIONE 10, STAMPERA' SOLO L'OUTPUT FINALE E IL TUTTO VERRA' ESEGUITO CON 6 THREAD
// ----------------------------------------------------------------------------------------------------
// | #!/bin/bash
// |
// | #PBS -q studenti
// | #PBS -l nodes=1:ppn=8
// | #PBS -N matrix_by_vector_show_output
// | #PBS -o matrix_by_vector_show_output.out
// | #PBS -e matrix_by_vector_show_output.err
// |
// | cat $PBS_NODEFILE
// |
// | PBS_O_WORKDIR=$PBS_O_HOME/matrix_by_vector_project
// |
// | export OMP_NUM_THREADS=6
// | export PSC_OMP_AFFINITY=TRUE
// | gcc -fopenmp -lgomp -o $PBS_O_WORKDIR/matrix_by_vector $PBS_O_WORKDIR/main.c
// |
// | $PBS_O_WORKDIR/matrix_by_vector 10 10 1 0
// ------------------------------------------------------
// | OUTPUT GENERATO:
// ------------------------------------------------------
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | wn280.scope.unina.it
// | 
// | PRODUCT OF THE OPERATION
// | 8081554.569677
// | 170249771.410069
// | -212694685.679746
// | 126384894.027406
// | 114137758.535025
// | 1501443.295025
// | -6510422.211297
// | -160105523.739923
// | -64510427.360192
// | -89638798.421118
// | 
// | Operation executed in 0.004803 seconds
// ----------------------------------------------------------------------------------------------------
// | CON IL SEGUENTE INPUT IL PROGRAMMA GENERERA' UNA MATRICE CON 10 RIGHE E 10 COLONNE E UN VETTORE 
// | DI DIMENSIONE 10, STAMPERA' SOLO L'OUTPUT INIZIALE E IL TUTTO VERRA' ESEGUITO CON 6 THREAD
// ----------------------------------------------------------------------------------------------------
// #!/bin/bash
// 
// #PBS -q studenti
// #PBS -l nodes=1:ppn=8
// #PBS -N matrix_by_vector_show_input
// #PBS -o matrix_by_vector_show_input.out
// #PBS -e matrix_by_vector_show_input.err
// 
// cat $PBS_NODEFILE
// 
// PBS_O_WORKDIR=$PBS_O_HOME/matrix_by_vector_project
// 
// export OMP_NUM_THREADS=6
// export PSC_OMP_AFFINITY=TRUE
// gcc -fopenmp -lgomp -o $PBS_O_WORKDIR/matrix_by_vector $PBS_O_WORKDIR/main.c
// 
// $PBS_O_WORKDIR/matrix_by_vector 10 10 0 1
// ------------------------------------------------------
// | OUTPUT GENERATO:
// ------------------------------------------------------
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// 
// GENERATED MATRIX----------------------------------
// Row: 1
// -5735.131502 -5805.370808 -8620.911837 9598.998278 7900.451869 2167.373300 -8594.509363 9760.357216 5045.187473 -3252.325058 
// Row: 2
// -6511.504650 4026.443362 5951.164365 -5524.921417 -2810.976505 1152.716875 -5781.371593 1314.050555 -8565.207720 930.419564 
// Row: 3
// 7549.183220 4827.367663 7673.383951 7788.013220 954.578519 -547.559261 740.099549 -2840.628624 -4388.116598 2260.621786 
// Row: 4
// 4385.911226 -123.248100 6455.250382 5764.999390 -524.250269 4355.702400 -2067.627907 881.240964 4116.059542 -7022.440434 
// Row: 5
// 7628.915459 7604.554296 7004.003227 3580.079675 -7920.367718 -5806.972980 -5267.202854 -3701.739311 5507.077873 -3832.410574 
// Row: 6
// 7228.679657 3056.260943 -9005.043507 4902.063608 844.274163 1949.535012 -5645.495653 -8415.626287 9108.907059 -33.611059 
// Row: 7
// 3844.996095 3494.818211 9843.140189 300.245881 -740.182400 -681.109428 -5344.052315 7192.189991 -9799.869061 8772.007599 
// Row: 8
// -9830.249548 7829.046696 6376.561821 7173.753381 1409.126520 8456.194401 -8633.219004 6141.923070 -5245.544910 6873.858571 
// Row: 9
// -7690.488100 -8016.865253 -69.880486 -6695.531607 6885.198057 -9225.605726 5254.003406 -8760.297298 -7641.232014 4362.910390 
// Row: 10
// 1206.091642 6203.764081 -2142.270803 1049.231887 -3495.990038 7117.546499 -9631.878138 1159.958243 4309.736490 -9431.747198 
// 
// GENERATED VECTOR----------------------------------
// -68.033934
// 4479.486942
// 8397.299647
// -3691.471815
// 1653.239727
// -193.574429
// -5235.277414
// 3020.020723
// -4051.650763
// -480.822325
// 
// Operation executed in 0.004244 seconds
// ----------------------------------------------------------------------------------------------------
// | CON IL SEGUENTE INPUT IL PROGRAMMA GENERERA' UNA MATRICE CON 10 RIGHE E 10 COLONNE E UN VETTORE 
// | DI DIMENSIONE 10, STAMPERA' SOLO IL TEMPO DI ESECUZIONE IL TUTTO VERRA' ESEGUITO CON 6 THREAD
// ----------------------------------------------------------------------------------------------------
// #!/bin/bash
// 
// #PBS -q studenti
// #PBS -l nodes=1:ppn=8
// #PBS -N matrix_by_vector_hide_outputs
// #PBS -o matrix_by_vector_hide_outputs.out
// #PBS -e matrix_by_vector_hide_outputs.err
// 
// cat $PBS_NODEFILE
// 
// PBS_O_WORKDIR=$PBS_O_HOME/matrix_by_vector_project
// 
// export OMP_NUM_THREADS=6
// export PSC_OMP_AFFINITY=TRUE
// gcc -fopenmp -lgomp -o $PBS_O_WORKDIR/matrix_by_vector $PBS_O_WORKDIR/main.c
// 
// $PBS_O_WORKDIR/matrix_by_vector 10 10 1 1
// ------------------------------------------------------
// | OUTPUT GENERATO:
// ------------------------------------------------------
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// wn280.scope.unina.it
// Operation executed in 0.005222 seconds




double* matrix_by_vector(double *matrix, int rows, int columns, double *vector)
{
    int row, column;
    double *product = (double*)calloc(columns, sizeof(double));
    #pragma omp parallel for default(none) shared(rows, columns, matrix, vector, product) private(row, column)
    for(row = 0; row < rows; ++row)
        for(column = 0; column < columns; ++column)
            product[row] += matrix[row * columns + column] * vector[column];
    return product;
}

void print_matrix(double *matrix, int rows, int columns)
{
    int row, column;
    for(row = 0; row != rows; ++row)
    {
        printf("Row: %d\n", row + 1);
        for(column = 0; column != columns; ++column)
            printf("%f ", matrix[row * columns + column]);
        puts("");
    }
}

void print_vector(double *vector, int size)
{
    int i;
    for(i = 0; i != size; ++i)
        printf("%f\n", vector[i]);
    puts("");
}

int get_rows_from_input(int argc, char *argv[])
{
    if(argc < INDEX_ROWS)
    {
        fprintf(stderr, "Error! Not enough argv arguments\n");
        exit(-1);
    }
    int rows = atoi(argv[INDEX_ROWS]);
    if(rows <= 0)
    {
        fprintf(stderr, "Error! Rows parameter has incorrect value\n");
        exit(-1);
    }
    return rows;
}

int get_columns_from_input(int argc, char *argv[])
{
    if(argc < INDEX_COLUMNS)
    {
        fprintf(stderr, "Error! Not enough argv arguments\n");
        exit(-1);
    }
    int columns = atoi(argv[INDEX_COLUMNS]);
    if(columns <= 0)
    {
        fprintf(stderr, "Error! Columns parameter has incorrect value\n");
        exit(-1);
    }
    return columns;
}

double get_random_number(double min, double max)
{
    float scale = rand() / (float) RAND_MAX;
	return min + scale * (max - (min));
}

double* init_matrix(int rows, int columns)
{
    double *matrix = (double*)calloc(rows * columns, sizeof(double));
    int row, column;
    for(row = 0; row != rows; ++row)
        for(column = 0; column != columns; ++column)
            matrix[row * columns + column] = get_random_number(MIN_RANDOM_NUMBER, MAX_RANDOM_NUMBER);
    return matrix;
}

double* init_vector(int size)
{
    double *vector = (double*)calloc(size, sizeof(double));
    int i;
    for(i = 0; i != size; ++i)
        vector[i] = get_random_number(MIN_RANDOM_NUMBER, MAX_RANDOM_NUMBER);
    return vector;
}

int get_hide_init_output(int argc, char *argv[])
{
    if(argc < INDEX_HIDE_INIT_OUTPUT)
    {
        fprintf(stderr, "Error! Not enough argv arguments\n");
        exit(-1);
    }
    int hide_init_output = atoi(argv[INDEX_HIDE_INIT_OUTPUT]); 
    if(hide_init_output != 0 && hide_init_output != 1)
    {
        fprintf(stderr, "Error! Hide init output parameter has incorrect value\n");
        exit(-1);
    }
    return hide_init_output;
}

int get_hide_last_output(int argc, char *argv[])
{
    if(argc < INDEX_HIDE_LAST_OUTPUT)
    {
        fprintf(stderr, "Error! Not enough argv arguments\n");
        exit(-1);
    }
    int hide_last_output = atoi(argv[INDEX_HIDE_LAST_OUTPUT]); 
    if(hide_last_output != 0 && hide_last_output != 1)
    {
        fprintf(stderr, "Error! Hide last output parameter has incorrect value\n");
        exit(-1);
    }
    return hide_last_output;
}

int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));
    int matrix_rows = get_rows_from_input(argc, argv);
    int matrix_columns = get_columns_from_input(argc, argv);
    int hide_init_output = get_hide_init_output(argc, argv);
    int hide_last_output = get_hide_last_output(argc, argv);
    int vector_size = matrix_rows;
    double *matrix = init_matrix(matrix_rows, matrix_columns);
    double *vector = init_vector(vector_size);
    if(!hide_init_output)
    {
        puts("\nGENERATED MATRIX----------------------------------");
        print_matrix(matrix, matrix_rows, matrix_columns);
        puts("\nGENERATED VECTOR----------------------------------");
        print_vector(vector, vector_size);
    }
    struct timeval time;
    double beginning, end;
    gettimeofday(&time, NULL);
    beginning = time.tv_sec + (time.tv_usec / 1000000.0);
    double *product = matrix_by_vector(matrix, matrix_rows, matrix_columns, vector);
    gettimeofday(&time, NULL);
    end = time.tv_sec + (time.tv_usec / 1000000.0);
    if(!hide_last_output)
    {
        puts("\nPRODUCT OF THE OPERATION");
        print_vector(product, matrix_rows);
    }
    printf("Operation executed in %f seconds\n", end - beginning);
    free(matrix);
    free(vector);
    return 0;
}

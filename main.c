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

double* matrix_by_vector(double *restrict matrix, int rows, int columns, double *restrict vector)
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
        for(column = 0; column != columns; ++column)
            printf("%f ", matrix[row * columns + column]);
        puts("");
    }
}

void print_vector(double *vector, int size)
{
    int i;
    for(i = 0; i != size; ++i)
        printf("%f ", vector[i]);
    puts("");
}

int get_rows_from_input(int argc, char *argv[])
{
    if(argc < INDEX_ROWS)
    {
        fprintf(stderr, "Error! Not enough argv arguments\n");
        exit(-1);
    }
    return atoi(argv[INDEX_ROWS]);
}

int get_columns_from_input(int argc, char *argv[])
{
    if(argc < INDEX_COLUMNS)
    {
        fprintf(stderr, "Error! Not enough argv arguments\n");
        exit(-1);
    }
    return atoi(argv[INDEX_COLUMNS]);
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
    return atoi(argv[INDEX_HIDE_INIT_OUTPUT]);
}

int get_hide_last_output(int argc, char *argv[])
{
    if(argc < INDEX_HIDE_LAST_OUTPUT)
    {
        fprintf(stderr, "Error! Not enough argv arguments\n");
        exit(-1);
    }
    return atoi(argv[INDEX_HIDE_LAST_OUTPUT]);
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
        puts("GENERATED MATRIX----------------------------------");
        print_matrix(matrix, matrix_rows, matrix_columns);
        puts("GENERATED VECTOR----------------------------------");
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
        print_vector(product, matrix_columns);
    printf("Operation executed in %f seconds\n", end - beginning);
    free(matrix);
    free(vector);
    return 0;
}
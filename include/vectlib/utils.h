#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>  // ifdef
#include <math.h>
#include <unistd.h>
#include <pthread.h>

typedef struct sizes {
    size_t width_elemenet;
    size_t vect_size;
    size_t base_size;
} sizes_of_base;

typedef struct args_for_thread {
    int number;
    int num_of_cores;
    double in_norm;
    FILE *working_file;
    FILE *write_file;
    sizes_of_base *sizes;
} thread_args;

double vector_norm(const double *vector, unsigned int vector_size);
int read_vector(double **buffer, sizes_of_base *sizes, FILE *stream);
double *get_vect(int stroke, sizes_of_base *sizes, FILE *base_file);

int find_min_norm(FILE *base_file, sizes_of_base *sizes, int offset_stroke, double in_norm);
void print_vector(FILE *where, double *vect, sizes_of_base *sizes);
void fill_base(FILE *base_file, int base_size, int vect_size);

void execution(sizes_of_base *sizes, FILE *file_base, FILE *file_vect);
void *thread_routine(void *arg);
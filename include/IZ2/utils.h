#include <stdlib.h>
#include <stdio.h>

double vector_norm(const double *vector, unsigned int vector_size);
int read_vector(double **buffer, unsigned int vector_size, FILE *stream);
int offset_file_ptr(int count, FILE *file);
double *get_vect(int stroke,  int vector_size, FILE *base_file);
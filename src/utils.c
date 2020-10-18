#include <stdlib.h>
#include <stdio.h>  // Инклюды в h

double vector_norm(const double *vector, unsigned int vector_size) {
    if (!vector) {
        return -1;
    }

    double norm = 0;

    for (int i = 0; i < vector_size; ++i) {
        norm += vector[i] * vector[i];
    }

    return norm;
}

double* read_vector(unsigned int vector_size, FILE *stream) {
    if (!stream) {
        return NULL;
    }

    double *res_vect = (double*)calloc(vector_size, sizeof(double));
    if (!res_vect) {
        return NULL;
    }

    for (int i = 0; i < vector_size; ++i) {
        int fscanf_return_val = fscanf(stream, "%lf", &res_vect[i]);

        if (!fscanf_return_val || fscanf_return_val == EOF) {
            free(res_vect);
            return NULL;
        }
    }
    return res_vect;
}


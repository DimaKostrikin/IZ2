#include <stdlib.h>
#include <stdio.h>  // Инклюды в h
#include "IZ2/utils.h"

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

int read_vector(double **buffer, unsigned int vector_size, FILE *stream) {  // по одному указателю мб
    if (!stream) {
        return -1;
    }

    if (!buffer) {
        return -1;
    }
    double *temp = *buffer;

    for (int i = 0; i < vector_size; ++i) {
        int fscanf_return_val = fscanf(stream, "%lf", &temp[i]);  // name

        if (!fscanf_return_val || fscanf_return_val == EOF) { // Условие проверить
            return -1;
        }
    }
    return 1;
}


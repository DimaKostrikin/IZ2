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

int offset_file_ptr(int count, FILE *file) {
    double temp = 0;
    int err = 0;
    for (int i = 0; i < count; ++i) {
        err = fscanf(file, "%lf", &temp);
        if (err < 1) {
            return -1;
        }
    }
    return 1;
}

double *get_vect(int stroke,  int vector_size, FILE *base_file) {
    int err = 0;
    err = offset_file_ptr(stroke * vector_size, base_file);
    if (err < 0) {
        return NULL;
    }

    double *vect = (double*)calloc(vector_size, sizeof(double));
    for (int i = 0; i < vector_size; ++i) {
        err = fscanf(base_file, "%lf", &vect[i]);
        if (err < 1) {
            free(vect);
            return NULL;
        }
    }
    return vect;
}




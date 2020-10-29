#include <stdlib.h>
#include <stdio.h>  // ifdef
#include <math.h>
#include <unistd.h>

#include "vectlib/utils.h"
#include "vectlib//num_of_cores.h"

double vector_norm(const double *vector, unsigned int vector_size) {
    if (!vector) {
        return -1;
    }

    double norm = 0;

    for (int i = 0; i < vector_size; ++i) {
        norm += vector[i] * vector[i];
    }

    return sqrt(norm);
}

int read_vector(double **buffer, sizes_of_base *sizes, FILE *stream) {  // по одному указателю мб
    if (!stream || !sizes || !buffer) {
        return -1;
    }

    double *temp = *buffer;

    for (int i = 0; i < sizes->vect_size; ++i) {
        int fscanf_return_val = fscanf(stream, "%9lf", &temp[i]);  // name

        if (!fscanf_return_val || fscanf_return_val == EOF) { // Условие проверить
            return -1;
        }
    }
    return 1;
}

double *get_vect(int stroke, sizes_of_base *sizes, FILE *base_file) {
    off_t offset = stroke * sizes->width_elemenet * sizes->vect_size;
    int err = fseeko(base_file, offset, SEEK_SET);
    if (err < 0) {
        return NULL;
    }

    double *vect = (double*)calloc(sizes->vect_size, sizeof(double));
    for (int i = 0; i < sizes->vect_size; ++i) {
        err = fscanf(base_file, "%9lf", &vect[i]);
        if (err < 1) {
            free(vect);
            return NULL;
        }
    }
    return vect;
}

int find_min_norm(FILE *base_file, sizes_of_base *sizes, int offset_stroke, double in_norm) {
    double *buffer = (double*)calloc(sizes->vect_size, sizeof(double));
    if (!buffer) {
        fprintf(stderr, "allocation error\n");
        return -1;
    }

    int err = 0;

    err = read_vector(&buffer, sizes, base_file);
    if (!err) {
        free(buffer);
        fprintf(stderr, "reading vector err\n");
        return -1;
    }

    double norm_current_base = 0;
    double current_diff_norm = 0;

    norm_current_base = vector_norm(buffer, sizes->vect_size);
    current_diff_norm = fabs(in_norm - norm_current_base);
    int stroke = offset_stroke;
    for (int i = 1; i < sizes->base_size; ++i) {
        err = read_vector(&buffer, sizes, base_file);
        if (err < 0) {
            free(buffer);
            fprintf(stderr, "reading vect err here %d\n", i);
            return -1;
        }
        norm_current_base = vector_norm(buffer, sizes->vect_size);
        double temp_diff = fabs(in_norm - norm_current_base);
        if (temp_diff < current_diff_norm) {
            current_diff_norm = temp_diff;
            stroke = (i + offset_stroke);
        }
    }
    free(buffer);
    return stroke;
}

void print_vector(FILE *where, double *vect, sizes_of_base *sizes) {
    if (!where || !vect) {
        fprintf(stderr, "Null ptr");
        return;
    }

    int i = 0;

    for (; i < sizes->vect_size - 1; ++i) {
        fprintf(where, "%lf ", vect[i]);
    }

    fprintf(where, "%lf", vect[i]);
    fprintf(where, "\n");
}

void fill_base(FILE *base_file, int base_size, int vect_size) {
    for (int i = 0; i < base_size; ++i) {
        for (int j = 0; j < vect_size - 1; ++j) {
            double n = 9.99 * (double)rand() / RAND_MAX;
            fprintf(base_file, "%lf ", n);
        }
        double n = 9.99 * (double)rand() / RAND_MAX;
        fprintf(base_file, "%lf", n);
        fprintf(base_file, "\n");
    }
}

void fill_vect(FILE *vect_file, int vect_size) {
    for (int j = 0; j < vect_size - 1; ++j) {
        double n = 9.99 * (double)rand() / RAND_MAX;
        fprintf(vect_file, "%lf ", n);
    }
    double n = 9.99 * (double)rand() / RAND_MAX;
    fprintf(vect_file, "%lf", n);
    fprintf(vect_file, "\n");
}

double find_norm_from_file(sizes_of_base *sizes, FILE *file_vect) {
    if (!sizes || !file_vect) {
        fprintf(stderr, "Nullptr\n");
        return -1;
    }

    double *in_vector = (double*)calloc(sizes->vect_size, sizeof(double));
    if (!in_vector) {
        fprintf(stderr, "Allocation error\n");
        return -1;
    }
    read_vector(&in_vector, sizes, file_vect);
    double in_norm = vector_norm(in_vector, sizes->vect_size);
    free(in_vector);
    return in_norm;
}

void execution(sizes_of_base *sizes, FILE *file_base, FILE *file_vect) {
    printf("sequence method\n");
    if (!sizes || !file_base) {
        fprintf(stderr, "Nullptr\n");
        return;
    }

    double in_norm = find_norm_from_file(sizes, file_vect);
    if (in_norm < 0) {
        return;
    }

    int stroke = find_min_norm(file_base, sizes, 0, in_norm);
    if (stroke < 0) {
        return;
    }

    off_t offset = stroke * sizes->width_elemenet * sizes->vect_size; // wat (!) (!) (!)

    int errno = fseeko(file_base, offset, SEEK_SET);
    if (errno != 0) {
        return;
    }

    double *res_vect = get_vect(stroke, sizes, file_base);
    print_vector(stdout, res_vect, sizes);
    free(res_vect);
}
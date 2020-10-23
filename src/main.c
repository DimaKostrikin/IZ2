#include "IZ2/utils.h"
#include "IZ2/num_of_cores.h"

// У потоков смещение указателя с помощью fscanf n раз, до промежутка (!)
// ./main.out file_vect.txt file_base.txt 100

double *find_min_norm(FILE *vect_file, FILE *base_file, int vect_size, int base_size) {
    if (!vect_file || !base_file) {
        return NULL;
    }

    double *buffer = (double*)calloc(vect_size, sizeof(double));
    if (!buffer) {
        return NULL;
    }
    int err = 0;
    err = read_vector(&buffer, vect_size, vect_file);
    if (!err) {
        free(buffer);
        return NULL;
    }

    double in_norm = vector_norm(buffer, vect_size);  // ошибка?
    printf("in norm - %lf\n", in_norm);

    err = read_vector(&buffer, vect_size, base_file);
    if (!err) {
        free(buffer);
        return NULL;
    }
    double norm_current_base = 0;
    double current_diff_norm = 0;
    double temp_diff = 0;
    norm_current_base = vector_norm(buffer, vect_size);
    printf("norm_curr - %lf\n", norm_current_base);
    current_diff_norm = abs(in_norm - norm_current_base);
    int k = 0;
    for (int i = 1; i < base_size; ++i) {
        err = read_vector(&buffer, vect_size, base_file);
        if (!err) {
            free(buffer);
            return NULL;
        }
        norm_current_base = vector_norm(buffer, vect_size);
        printf("norm_curr - %lf\n", norm_current_base);
        temp_diff = abs(in_norm - norm_current_base);
        if (temp_diff < current_diff_norm) {
            current_diff_norm = temp_diff;
            k = i;
        }
    }
    printf("%d\n", k);
    return NULL; // Затычкa
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "4 arguments: 1) vector, 2) base, 3) num of components, 4) base size\n");
        return 1;
    }

    FILE *file_vect = fopen(argv[1], "r");

    if (!file_vect) {
        fprintf(stderr, "File of vect not found. Error.\n");
        return 1;
    }

    FILE *file_base = fopen(argv[2], "r");

    if (!file_base) {
        fprintf(stderr, "File of base not found. Error.\n");
        fclose(file_vect);
        return 1;
    }

    int vector_size = atoi(argv[3]);
    int base_size = atoi(argv[4]);

    if (vector_size < 1) {
        fprintf(stderr, "Vector size can't be 0 or less\n");
        fclose(file_vect);
        fclose(file_base);
        return 1;
    }

    find_min_norm(file_vect, file_base, vector_size, base_size);
}

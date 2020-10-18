#include "IZ2/utils.h"

// ./main.out file1.txt 100

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "3 arguments: vector, base, num of components\n");
        return 1;
    }

    FILE *file_vect = fopen(argv[1], "r");

    if (!file_vect) {
        fprintf(stderr, "File of vect not found. Error.\n");
        return 1;
    }

    /*FILE *file_base = fopen(argv[2], "r");

    if (!file_base) {
        fprintf(stderr, "File of base not found. Error.\n");
        fclose(file_vect);
        return 1;
    }
    */
    int vector_size = atoi(argv[2]);

    if (vector_size < 1) {
        fprintf(stderr, "Vector size can't be 0 or less\n");
        fclose(file_vect);
        //fclose(file_base);
    }

    double *vect = read_vector(vector_size, file_vect);
    double norm = vector_norm(vect, vector_size);
    printf("%lf", norm);
}

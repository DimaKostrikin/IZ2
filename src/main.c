#include "IZ2/utils.h"


int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "5 arguments: 1) vector, 2) base, 3) vect size, 4) base size, 5) mode\n");
        return 1;
    }

    FILE *file_vect = fopen(argv[1], "r");

    if (!file_vect) {
        fprintf(stderr, "File of vect not found. Error.\n");
        return 1;
    }

    FILE *file_base = fopen(argv[2], "r+");

    if (!file_base) {
        fprintf(stderr, "File of base not found. Error.\n");
        fclose(file_vect);
        return 1;
    }

    sizes_of_base sizes;
    sizes.vect_size = atoi(argv[3]);
    sizes.base_size = atoi(argv[4]);
    sizes.width_elemenet = 9;
    int mode = atoi(argv[5]);

    if (sizes.vect_size < 1) {
        fprintf(stderr, "Vector size can't be 0 or less\n");
        fclose(file_vect);
        fclose(file_base);
        return 1;
    }

    switch (mode){
        case 0:
            sequential_execution(&sizes, file_base, file_vect);
            break;
        case 1:
            parallel_execution(&sizes, file_base, file_vect);
            break;
        case 2:
            fill_base(file_base, sizes.base_size, sizes.vect_size);
    }

    fclose(file_base);
    fclose(file_vect);

    //printf("%0*.*lf", 8, 6, a);
}

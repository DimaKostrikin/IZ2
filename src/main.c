#include "IZ2/utils.h"
#include "IZ2/num_of_cores.h"
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

#include <string.h>
#include <math.h>

// У потоков смещение указателя с помощью fscanf n раз, до промежутка (!)
// ./main.out file_vect.txt file_base.txt 100
// массив указателей на файлы передавать в процессы

int find_min_norm(FILE *vect_file, FILE *base_file, int vect_size, int base_size, int offset_stroke) {
    if (!vect_file || !base_file) {
        return -1;
    }

    double *buffer = (double*)calloc(vect_size, sizeof(double));
    if (!buffer) {
        return -1;
    }
    int err = 0;
    err = read_vector(&buffer, vect_size, vect_file);
    if (!err) {
        free(buffer);
        return -1;
    }

    double in_norm = vector_norm(buffer, vect_size);  // ошибка?

    err = read_vector(&buffer, vect_size, base_file);
    if (!err) {
        free(buffer);
        return -1;
    }
    double norm_current_base = 0;
    double current_diff_norm = 0;
    double temp_diff = 0;

    norm_current_base = vector_norm(buffer, vect_size);
    current_diff_norm = abs(in_norm - norm_current_base);
    int stroke = offset_stroke;
    for (int i = 1; i < base_size; ++i) {
        err = read_vector(&buffer, vect_size, base_file);
        if (err < 0) {
            free(buffer);
            return -1;
        }
        norm_current_base = vector_norm(buffer, vect_size);
        temp_diff = abs(in_norm - norm_current_base);
        if (temp_diff < current_diff_norm) {
            current_diff_norm = temp_diff;
            stroke = (i + offset_stroke);
        }
    }
    free(buffer);
    return stroke;
}

void wait_process(int num_of_cores) {
    if (num_of_cores < 1) {
        fprintf(stderr, "Num of cores < 1");
    }

    int status = 0;
    int ended_count_process = 0;
    do {
        pid_t waited_pid = waitpid(-1, &status, WNOHANG);

        if (waited_pid < 0) {
            fprintf(stderr, "waitpid error\n");
            break;
        }

        if (waited_pid) {
            if (WIFEXITED(status)) {
                ++ended_count_process;
                printf("Exited with code %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                ++ended_count_process;
                printf("Killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                ++ended_count_process;
                printf("Stopped by signal %d\n", WSTOPSIG(status));
            }
        }
        sleep(1);
    } while (ended_count_process != num_of_cores);
}

void first_pass(int num_of_cores, int vector_size, int base_size, FILE* write) { // В структуру файлов
    int pid = 0;

    for (int i = 0; i < num_of_cores; ++i) {
        pid = fork();
        if (pid == 0) {
            if (!write) {
                fprintf(stderr, "Fopen err\n");
                return;
            }
            FILE *base = fopen("txt/file_base.txt", "r+");



            FILE *vect = fopen("txt/file_vect.txt", "r");

            if (!vect) {
                fprintf(stderr, "Fopen err\n");
                fclose(base);
                return;
            }

            int interval = base_size/num_of_cores;

            int offset = i * interval;

            //offset_file_ptr(offset, base);
            int o;
            char c;
            while (o != offset) {
                c = fgetc(base);
                if (c == '\n')
                    ++o;
            }
            int offset_stroke = i * interval;
            int k = find_min_norm(vect, base, vector_size, interval, offset_stroke);

            fseek(base, 0, SEEK_SET);
            double *find_vect = get_vect(k, vector_size, base);

            if (!find_vect) {
                fprintf(stderr, "allocation error");
                fclose(base);
                fclose(vect);
            }

            for (int j = 0; j < vector_size; ++j) {
                fprintf(write,"%lf ", find_vect[j]);
            }
            fprintf(write, "\n");

            fclose(base);
            fclose(vect);
            free(find_vect);

            exit(0);
        }
    }
}

void print_vector(FILE *whence, double *vect, int vector_size) {
    if (!whence || !vect) {
        fprintf(stderr, "Null ptr");
        return;
    }
    for (int i = 0; i < vector_size; ++i) {
        fprintf(whence,"%lf ", vect[i]);
    }
    printf("\n");
};

void fill_base(FILE *base_file, int base_size, int vect_size) {
    for (int i = 0; i < base_size; ++i) {
        for (int j = 0; j < vect_size; ++j) {
            double n = 10 * (double)rand() / RAND_MAX;
            fprintf(base_file, "%lf ", n);
        }
        fprintf(base_file, "\n");
    }
}

int main(int argc, char *argv[]) {
    /*if (argc != 6) {
        fprintf(stderr, "5 arguments: 1) vector, 2) base, 3) num of components, 4) base size, 5) mode\n");
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

    int vector_size = atoi(argv[3]);
    int base_size = atoi(argv[4]);
    int mode = atoi(argv[5]);

    if (vector_size < 1) {
        fprintf(stderr, "Vector size can't be 0 or less\n");
        fclose(file_vect);
        fclose(file_base);
        return 1;
    }

    int k = 0;
    int num_of_cores = get_num_cores();
    num_of_cores = 4;
    FILE *write = fopen("txt/file_write.txt", "r+");

    switch (mode){
        case 0:
            k = find_min_norm(file_vect, file_base, vector_size, base_size, 0);  // magic
            //printf("k = %d\n", k);
            fseek(file_base, 0, SEEK_SET);
            double *a_res = get_vect(k, vector_size, file_base);
            print_vector(stdout, a_res, vector_size);
            break;
        case 1:
            first_pass(num_of_cores, vector_size, base_size, write);
            wait_process(num_of_cores);
            fseek(write, 0, SEEK_SET);

            k = find_min_norm(file_vect, write, vector_size, num_of_cores, 0);

            fseek(write, 0, SEEK_SET);
            double *b_res = get_vect(k, vector_size, write);
            print_vector(stdout, b_res, vector_size);
            break;
    }
    */
    FILE *test = fopen("txt/file_base.txt", "r");

    struct stat st;
    stat("txt/file_base.txt", &st);
    size_t file_size = st.st_size;

    int fd = fileno(test);
    char *region = mmap(NULL,
                          file_size,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_POPULATE,
                          fd,
                          0);
    if (region == MAP_FAILED) {
        printf("Map failed\n");
        fclose(test);
        return 1;
    }
    double a;

    int width_of_num = 9;
    int size_of_string = 36;

    int num_of_stroke = 0;
    int num_of_elem = 0;

    double norm = 0;
    double sum = 0;

    while (num_of_elem < 4) {
        sscanf(region + size_of_string * num_of_stroke + num_of_elem * width_of_num, "%lf", &a);
        sum += (a*a);
        ++num_of_elem;
    }
    norm = sqrt(sum);
    printf("vector on stroke %d norm is %lf", num_of_stroke, norm);


    //sscanf(region + size_of_string * num_of_stroke + num_of_elem * width_of_num, "%lf", &a);
    //printf("%0*.*lf", 8, 6, a);
}

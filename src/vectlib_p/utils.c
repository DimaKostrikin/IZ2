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
        int fscanf_return_val = fscanf(stream, "%lf", &temp[i]);  // name

        if (!fscanf_return_val || fscanf_return_val == EOF) { // Условие проверить
            return -1;
        }
    }
    return 1;
}

double *get_vect(int stroke, sizes_of_base *sizes, FILE *base_file) {
    int err = 0;
    off_t offset = stroke * sizes->width_elemenet * sizes->vect_size;
    fseeko(base_file, offset, SEEK_SET);
    if (err < 0) {
        return NULL;
    }

    double *vect = (double*)calloc(sizes->vect_size, sizeof(double));
    for (int i = 0; i < sizes->vect_size; ++i) {
        err = fscanf(base_file, "%lf", &vect[i]);
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
    double temp_diff = 0;

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
        temp_diff = fabs(in_norm - norm_current_base);
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
    int num_of_processors = get_num_cores();
    FILE *massive_files[num_of_processors];

    for (int i = 0; i < num_of_processors; ++i) {  // Получение файловых дескрипторов для каждого потока
        massive_files[i] = fopen("txt/file_base.txt", "r");
        if (!massive_files[i]) {
            for (int j = i - 1; j >=0; --j) {
                fclose(massive_files[j]);
            }
            return;
        }
    }

    FILE *write_file = fopen("txt/file_write.txt", "a+");

    double in_norm = find_norm_from_file(sizes, file_vect); // find norm from file

    thread_args t_args[num_of_processors];  // Инициализация аргументов для каждого потока
    for (int i = 0; i < num_of_processors; ++i) {
        t_args[i].working_file = massive_files[i];
        t_args[i].number = i;
        t_args[i].sizes = sizes;
        t_args[i].write_file = write_file;
        t_args[i].in_norm = in_norm;
        t_args[i].num_of_cores = num_of_processors;
    }

    pthread_t thread_ids[num_of_processors];
    for (int i = 0; i < num_of_processors; ++i) {
        int errflag = pthread_create(&thread_ids[i], NULL, thread_routine, &t_args[i]);
        if (errflag != 0) {
            return;
        }
    }

    for (int i = 0; i < num_of_processors; ++i) {
        pthread_join(thread_ids[i], NULL);
    }

    sizes_of_base sizes_of_write;
    sizes_of_write.base_size = num_of_processors;
    sizes_of_write.vect_size = sizes->vect_size;
    sizes_of_write.width_elemenet = sizes->width_elemenet;

    fseek(write_file, 0, SEEK_SET);
    int k_min = find_min_norm(write_file, &sizes_of_write, 0, in_norm);
    fseek(write_file, 0, SEEK_SET);
    double *res_vect = get_vect(k_min, &sizes_of_write, write_file);
    print_vector(stdout, res_vect, sizes);
    remove("txt/file_write.txt");
}

void *thread_routine(void *arg) {
    thread_args *t_arg = (thread_args*)arg;

    int interval = t_arg->sizes->base_size/t_arg->num_of_cores;

    off_t offset = t_arg->number * interval * t_arg->sizes->width_elemenet * t_arg->sizes->vect_size;

    fseeko(t_arg->working_file, offset, SEEK_SET);

    sizes_of_base sizes_for_thread;
    sizes_for_thread.base_size = interval;
    sizes_for_thread.vect_size = t_arg->sizes->vect_size;
    sizes_for_thread.width_elemenet = t_arg->sizes->width_elemenet;

    int offset_stroke = t_arg->number * interval;

    int stroke_min = find_min_norm(t_arg->working_file, &sizes_for_thread, offset_stroke, t_arg->in_norm);

    fseeko(t_arg->working_file, 0, SEEK_SET);
    double *find_vect = get_vect(stroke_min, t_arg->sizes, t_arg->working_file);

    if (!find_vect) {
        fprintf(stderr, "allocation error");
        return NULL;
    }

    //mutex
    print_vector(t_arg->write_file, find_vect, &sizes_for_thread);
    return arg;
}
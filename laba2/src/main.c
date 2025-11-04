#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <complex.h>
#include <time.h>

typedef struct {
    int thread_id;
    int max_threads;
    int n;
    double complex **A;
    double complex **B;
    double complex **C;
    sem_t *sem;
} ThreadArgs;

static void *work(void *_args) {
    ThreadArgs *args = (ThreadArgs*)_args;
    int id = args->thread_id;
    int max_threads = args->max_threads;
    int n = args->n;
    
    for (int i = id; i < n; i+=max_threads) {
        for (int j = 0; j < n; ++j) {
            args->C[i][j]= 0 + 0 * I;
            for (int k = 0; k < n; ++k) {
                args->C[i][j] += args->A[i][k] * args->B[k][j];
            }
        }
    }
    sem_post(args->sem);

    return NULL;
}

static double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        const char msg[] = "error: invalid input\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int max_threads = atoi(argv[2]);

    if (n <= 0 || max_threads <= 0) {
        const char msg[] = "error: invalid input\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    double complex **A = malloc(n * sizeof(double complex*));
    double complex **B = malloc(n * sizeof(double complex*));
    double complex **C = malloc(n * sizeof(double complex*));

    for (int i = 0; i < n; ++i) {
        A[i] = malloc(n * sizeof(double complex));
        B[i] = malloc(n * sizeof(double complex));
        C[i] = malloc(n * sizeof(double complex));
    }
    
    srand(time(NULL));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = rand() % 10 + (rand() % 10 * I);
            B[i][j] = rand() % 10 + (rand() % 10 * I);
        }
    }

    char buf[128];

    // posledovatelniy algorithm
    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        const char msg[] = "error: cant get start time\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0 + 0 * I;
            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        const char msg[] = "error: cant get end time\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    double total_time = get_time_diff(start, end);

    if (snprintf(buf, sizeof(buf), "\nposledovatelno\nmatrix multiplication completed in %.6f sec\nused %d threads (n=%d)\n", total_time, max_threads, n) >= 0) {
        write(STDOUT_FILENO, buf, strlen(buf));
    }

    // parallel algorithm
    sem_t sem;
    sem_init(&sem, 0, 0);

    pthread_t *threads = (pthread_t*)malloc(max_threads * sizeof(pthread_t));
    ThreadArgs *thread_args = (ThreadArgs*)malloc(max_threads * sizeof(ThreadArgs));
    
    struct timespec start_paral, end_paral;

    if (clock_gettime(CLOCK_MONOTONIC, &start_paral) != 0) {
        const char msg[] = "error: cant get start time\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < max_threads; ++i) {
        thread_args[i] = (ThreadArgs){
            .thread_id = i,
            .max_threads = max_threads,
            .n = n,
            .A = A,
            .B = B,
            .C = C,
            .sem = &sem,
        };
        
        pthread_create(&threads[i], NULL, work, &thread_args[i]);
    }

    for (size_t i = 0; i < max_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end_paral) != 0) {
        const char msg[] = "error: cant get start time\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    double total_paral_time = get_time_diff(start_paral, end_paral);

    if (snprintf(buf, sizeof(buf), "\nparallel\nmatrix multiplication completed in %.6f sec\nused %d threads (n=%d)\n", total_paral_time, max_threads, n) >= 0) {
        write(STDOUT_FILENO, buf, strlen(buf));
    }

    for (int i =0; i < n; ++i) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }

    free(A);
    free(B);
    free(C);
    free(thread_args);
    free(threads);
    sem_destroy(&sem);

    return 0;
}

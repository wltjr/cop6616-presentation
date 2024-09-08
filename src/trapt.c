#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "equations.h"

// gcc -g -pthread trapt.c equations.c -lpthread -o trapt -lm

double global_sum = 0.0;
double a, b;
int n, num_threads;
pthread_mutex_t mutex;

typedef struct {
    int id;
    double local_a;
    double local_b;
    int local_n;
    double (*f)(double);
} thread_data;

void *trapezoid_rule(void *arg) {
    thread_data *data = (thread_data*)arg;
    double local_a = data->local_a;
    double local_b = data->local_b;
    int local_n = data->local_n;
    double h = (local_b - local_a) / local_n;
    double local_sum = 0.0;
    double (*f)(double) = data->f;

    for (int i = 1; i < local_n; i++) {
        double x = local_a + i * h;
        local_sum += f(x);
    }
    
    local_sum += (f(local_a) + f(local_b)) / 2.0;
    local_sum = local_sum * h;

    pthread_mutex_lock(&mutex);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char** argv){
    if (argc < 5) {
        printf("Usage: %s <a> <b> <n> <num_threads>\n", argv[0]);
        return -1;
    }
    
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%d", &n);
    sscanf(argv[4], "%d", &num_threads);

    pthread_t threads[num_threads];
    thread_data thread_args[num_threads];
    pthread_mutex_init(&mutex, NULL);

    struct timespec start, end;

    EquationSet es = get_equations();
    
    for (int f_idx = 0; f_idx < NUM_EQUATIONS; f_idx++) {
        global_sum = 0.0;
        const char* fn = es.equation_names[f_idx];
        
        clock_gettime(CLOCK_MONOTONIC, &start);

        double h = (b - a) / n;
        double trapezoids_per_thread = n / num_threads;
        
        for (int i = 0; i < num_threads; i++) {
            thread_args[i].id = i;
            thread_args[i].local_a = a + i * trapezoids_per_thread * h;
            thread_args[i].local_b = thread_args[i].local_a + trapezoids_per_thread * h;
            thread_args[i].local_n = trapezoids_per_thread;
            thread_args[i].f = es.equation_functions[f_idx];

            pthread_create(&threads[i], NULL, trapezoid_rule, (void*)&thread_args[i]);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("PARALLEL (THREADED)\nFUNCTION:\t%s\nThreads:\t%d\nn:\t\t%d trapezoids\n", fn, num_threads, n);
        printf("Interval:\t[%f, %f]\nApproximation:\t%f\n", a, b, global_sum);
        printf("Time:\t\t%lf seconds\n\n", time_taken);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
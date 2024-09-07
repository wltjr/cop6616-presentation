#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_THREADS 2000

// gcc -g -Wall -pthread trapt.c -lpthread -o trapt -lm

double global_sum = 0.0;
double a, b;
int n;
pthread_mutex_t mutex;

double f(double x) {
    //return x * x;
    return x*x + 50 * (sin(100 * M_PI * x) * sin(M_PI * x));
}

typedef struct {
    int id;
    double local_a;
    double local_b;
    int local_n;
} thread_data;

void *trapezoid_rule(void *arg) {
    thread_data *data = (thread_data*)arg;
    double local_a = data->local_a;
    double local_b = data->local_b;
    int local_n = data->local_n;
    double h = (local_b - local_a) / local_n;
    double local_sum = 0.0;

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
    if (argc < 4) {
        printf("Usage: %s <a> <b> <n>\n", argv[0]);
        return -1;
    }
    
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%d", &n);

    pthread_t threads[NUM_THREADS];
    thread_data thread_args[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    struct timespec start, end;
    
    clock_gettime(CLOCK_MONOTONIC, &start);

    double h = (b - a) / n;
    double trapezoids_per_thread = n / NUM_THREADS;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].id = i;
        thread_args[i].local_a = a + i * trapezoids_per_thread * h;
        thread_args[i].local_b = thread_args[i].local_a + trapezoids_per_thread * h;
        thread_args[i].local_n = trapezoids_per_thread;

        pthread_create(&threads[i], NULL, trapezoid_rule, (void*)&thread_args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("With n = %d trapezoids, the parallel estimate of the integral from %f to %f is: %f\n", n, a, b, global_sum);
    printf("Time taken: %lf seconds\n", time_taken);

    pthread_mutex_destroy(&mutex);
    return 0;
}
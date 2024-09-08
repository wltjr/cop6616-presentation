#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "equations.h"

// gcc -g -pthread -lpthread -o trapt trapt.c equations.c -lm

/**
 * thread_data: Struct to hold data for each thread
 * @param id: Thread ID
 * @param local_n: Number of trapezoids for each thread
 * @param local_a: Start of the interval for each thread
 * @param local_b: End of the interval for each thread
 * @param f: Function to compute the trapezoid rule for
 * @param global_sum: Global sum of the trapezoid rule
 * @param mutex: Mutex to lock the global sum
 */
typedef struct {
    int id, local_n;
    double local_a;
    double local_b;
    double (*f)(double);
    double *global_sum;
    pthread_mutex_t *mutex;
} thread_data;

/** 
 * trapezoid_rule: Compute the trapezoidal rule for a given function
 * @param arg: thread_data struct
 * @return: void
 * 
 */
void *trapezoid_rule(void *arg) {
    // Set thread data to local variables
    thread_data *data = (thread_data*)arg;
    double local_a = data->local_a;
    double local_b = data->local_b;
    int local_n = data->local_n;
    double h = (local_b - local_a) / local_n;
    double local_sum = 0.0;
    double (*f)(double) = data->f;


    // Compute trapezoidal rule for each thread
    for (int i = 1; i < local_n; i++) {
        double x = local_a + i * h;
        local_sum += f(x);
    }
    local_sum += (f(local_a) + f(local_b)) / 2.0;
    local_sum = local_sum * h;

    // Lock thread and update the global sum
    pthread_mutex_lock(data->mutex);
    *(data->global_sum) += local_sum;
    pthread_mutex_unlock(data->mutex);

    pthread_exit(NULL);
}

/**
 * main: Main function to compute the trapezoidal rule in parallel
 * @param argc: Number of arguments
 * @param argv: Arguments
 * @return: 0 if successful
 */
int main(int argc, char** argv) {
    // Check for correct number of arguments
    if (argc < 5) {
        printf("Usage: %s <a> <b> <n> <num_threads>\n", argv[0]);
        return -1;
    }

    // Declare and parse arguments to their variables
    double a, b;
    int n, num_threads;
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%d", &n);
    sscanf(argv[4], "%d", &num_threads);

    // Declare variables for threads and mutex
    pthread_t threads[num_threads];
    thread_data thread_args[num_threads];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    // Timer struct to measure time taken
    struct timespec start, end;

    // Get equation information from equations.h
    EquationSet es = get_equations();

    // Loop over each function in the equation set
    for (int f_idx = 0; f_idx < NUM_EQUATIONS; f_idx++) {
        double global_sum = 0.0;  // Local global_sum for each equation
        const char* fn = es.equation_names[f_idx]; // Function name for each equation

        // Start timer
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Calculate h and trapezoids per thread
        double h = (b - a) / n;
        int trapezoids_per_thread = n / num_threads;

        // Create threads to compute trapezoidal rule in parallel
        for (int i = 0; i < num_threads; i++) {
            thread_args[i].id = i;
            thread_args[i].local_a = a + i * trapezoids_per_thread * h;
            thread_args[i].local_b = thread_args[i].local_a + trapezoids_per_thread * h;
            thread_args[i].local_n = trapezoids_per_thread;
            thread_args[i].f = es.equation_functions[f_idx];
            thread_args[i].mutex = &mutex;
            thread_args[i].global_sum = &global_sum;

            pthread_create(&threads[i], NULL, trapezoid_rule, (void*)&thread_args[i]);
        }

        // Join threads
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // End timer and calculate time taken
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        // Print the results
        printf("PARALLEL (THREADED)\nFUNCTION:\t%s\nThreads:\t%d\nn:\t\t%d trapezoids\n", fn, num_threads, n);
        printf("Interval:\t[%f, %f]\nApproximation:\t%f\n", a, b, global_sum);
        printf("Time:\t\t%lf seconds\n\n", time_taken);
    }

    // Destroy mutex and return
    pthread_mutex_destroy(&mutex);

    return 0;
}

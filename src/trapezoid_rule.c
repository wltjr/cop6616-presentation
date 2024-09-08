#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// gcc -g -pthread trapezoid_rule.c -lpthread -o trapezoid_rule -lm

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NUM_EQUATIONS 4

// Function definitions
double f(double x) { return x * x; }
double g(double x) { return x * x + 50 * (sin(100 * M_PI * x) * sin(M_PI * x)); }
double h(double x) { return sin(30 * M_PI * x) + cos(23 * M_PI * x); }
double j(double x) { return x * x * x; }

// Array of strings to represent the equations
const char *equation_names[] = {
    "x^2",
    "x^2 + 50 * sin(100 * pi * x) * sin(pi * x)",
    "sin(30 * pi * x) + cos(23 * pi * x)",
    "x^3"
};

// Array of function pointers to the equations
double (*equation_functions[])(double) = { f, g, h, j };

/**
 * EquationSet: Struct to hold equation names and functions
 * @param equation_names: Array of equation names
 * @param equation_functions: Array of equation functions
 */
typedef struct {
    const char **equation_names;
    double (**equation_functions)(double);
} EquationSet;

/**
 * Function to get the equations
 * @return: The equation set
 */
EquationSet get_equations(void) {
    EquationSet set;
    set.equation_names = equation_names;
    set.equation_functions = equation_functions;
    return set;
}

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

// Main function
int main(int argc, char **argv) {
    // Check for correct number of arguments
    if (argc < 4) {
        printf("Usage: %s <a> <b> <n> <num_threads>\n", argv[0]);
        return -1;
    }

    // Declare variables and parse arguments
    double a, b;
    int n, num_threads;
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%d", &n);

    // Declare threaded variables
    pthread_t *threads = NULL;
    thread_data *thread_args = NULL;
    pthread_mutex_t mutex;
    int trapezoids_per_thread = 0;
    double global_sum = 0.0;
    
    // Assign number of threads and allocate memory if num_threads is provided
    if (argc == 5) {
        sscanf(argv[4], "%d", &num_threads);
        threads = malloc(num_threads * sizeof(pthread_t));
        thread_args = malloc(num_threads * sizeof(thread_data));
        pthread_mutex_init(&mutex, NULL);
        trapezoids_per_thread = n / num_threads;
    }

    // Timer struct to measure time taken
    struct timespec start, end;

    // Get equation information (names and functions)
    EquationSet es = get_equations();

    // Loop over equations
    for (int f_idx = 0; f_idx < NUM_EQUATIONS; f_idx++) {
        const char *fn = es.equation_names[f_idx]; // Function name for each equation

        // Start timer
        clock_gettime(CLOCK_MONOTONIC, &start);

        if (argc == 5) {
            global_sum = 0.0;

            double h = (b - a) / n;

            // Create threads
            for (int i = 0; i < num_threads; i++) {
                thread_args[i].id = i;
                thread_args[i].local_a = a + i * trapezoids_per_thread * h;
                thread_args[i].local_b = thread_args[i].local_a + trapezoids_per_thread * h;
                thread_args[i].local_n = trapezoids_per_thread;
                thread_args[i].f = es.equation_functions[f_idx];
                thread_args[i].mutex = &mutex;
                thread_args[i].global_sum = &global_sum;

                pthread_create(&threads[i], NULL, trapezoid_rule, (void *)&thread_args[i]);
            }

            // Join threads
            for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
            }

            // End timer
            clock_gettime(CLOCK_MONOTONIC, &end);
            double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

            // Print results
            printf("PARALLEL (THREADED)\nFUNCTION:\t%s\nThreads:\t%d\nn:\t\t%d trapezoids\n", fn, num_threads, n);
            printf("Interval:\t[%f, %f]\nApproximation:\t%f\n", a, b, global_sum);
            printf("Time:\t\t%lf seconds\n\n", time_taken);

        } else if (argc == 4) {
            double integral = 0.0;
            double h = (b - a) / n;
            integral = (es.equation_functions[f_idx](a) + es.equation_functions[f_idx](b)) / 2;
            double x = a;

            for (int i = 1; i <= n - 1; i++) {
                x += h;
                integral += es.equation_functions[f_idx](x);
            }
            integral *= h;

            // End timer
            clock_gettime(CLOCK_MONOTONIC, &end);
            double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

            // Print results
            printf("SERIAL\nFUNCTION:\t%s\nn:\t\t%d trapezoids\nInterval:\t[%f, %f]\n", fn, n, a, b);
            printf("Approximation:\t%f\nTime:\t\t%lf seconds\n\n", integral, time_taken);
        }
    }

    // Clean up and destroy mutex
    if (argc == 5) {
        free(threads);
        free(thread_args);
        pthread_mutex_destroy(&mutex);
    }

    return 0;
}
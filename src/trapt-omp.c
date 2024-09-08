#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include "equations.h"

// gcc -g -fopenmp -foffload=nvidia trapt-omp.c equations.c -o trapt-omp -lm

double global_sum = 0.0;
double a, b;
int n, num_threads;

int main(int argc, char** argv){
    if (argc < 5) {
        printf("Usage: %s <a> <b> <n> <num_threads>\n", argv[0]);
        return -1;
    }
    
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%d", &n);
    sscanf(argv[4], "%d", &num_threads);

    struct timespec start, end;

    EquationSet es = get_equations();
    
    for (int f_idx = 0; f_idx < NUM_EQUATIONS; f_idx++) {
        global_sum = 0.0;
        const char* fn = es.equation_names[f_idx];
        
        clock_gettime(CLOCK_MONOTONIC, &start);

        double h = (b - a) / n;
        double trapezoids_per_thread = n / num_threads;
        
        #pragma omp parallel for reduction(+:global_sum)
        #pragma omp num_threads(num_threads)
        for (int i = 0; i < num_threads; i++) {
            double local_a = a + i * trapezoids_per_thread * h;
            double local_b = local_a + trapezoids_per_thread * h;
            double local_sum = 0.0;
            double (*f)(double) = es.equation_functions[f_idx];

            for (int i = 1; i < trapezoids_per_thread; i++) {
                double x = local_a + i * h;
                local_sum += f(x);
            }
            
            local_sum += (f(local_a) + f(local_b)) / 2.0;
            local_sum = local_sum * h;

            global_sum += local_sum;
        }     

        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("PARALLEL (THREADED-OMP)\nFUNCTION:\t%s\nThreads:\t%d\nn:\t\t%d trapezoids\n", fn, num_threads, n);
        printf("Interval:\t[%f, %f]\nApproximation:\t%f\n", a, b, global_sum);
        printf("Time:\t\t%lf seconds\n\n", time_taken);
    }

    return 0;
}
#include <stdio.h>
#include <time.h>
#include "equations.h"

// gcc traps.c equations.c -o traps -lm

int main(int argc, char** argv){
    if (argc < 4) {
        printf("Usage: %s <a> <b> <n>\n", argv[0]);
        return -1;
    }

    double integral, a, b, h, x;
    int n;
    
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%d", &n);

    struct timespec start, end;

    EquationSet es = get_equations();

    for (int f_idx = 0; f_idx < NUM_EQUATIONS; f_idx++) {
        double (*f)(double) = es.equation_functions[f_idx];
        const char* fn = es.equation_names[f_idx];

        integral = 0.0;
        clock_gettime(CLOCK_MONOTONIC, &start);

        h = (b - a) / n;
        integral = (f(a) + f(b)) / 2;
        x = a;
        for (int i = 1; i <= n - 1 ; i++) {
            x += h;
            integral += f(x);
        }
        integral *= h;

        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("SERIAL\nFUNCTION:\t%s\nn:\t\t%d trapezoids\nInterval:\t[%f, %f]\n", fn, n, a, b);
        printf("Approximation:\t%f\nTime:\t\t%lf seconds\n\n", integral, time_taken);
    }

    return 0;
}
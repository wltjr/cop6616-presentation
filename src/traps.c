#include <stdio.h>
#include <time.h>
#include "equations.h"

// gcc traps.c equations.c -o traps -lm

/**
 * main: Main function to compute the trapezoidal rule in serial
 * @param argc: Number of arguments
 * @param argv: Arguments
 * @return: 0 if successful
 */
int main(int argc, char** argv){
    // Check for correct number of arguments
    if (argc < 4) {
        printf("Usage: %s <a> <b> <n>\n", argv[0]);
        return -1;
    }

    // Declare and parse arguments to their variables
    double integral, a, b, h, x;
    int n;
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%d", &n);

    // Timer struct to measure time taken
    struct timespec start, end;

    // Get equation information from equations.h
    EquationSet es = get_equations();

    // Loop over each function in the equation set
    for (int f_idx = 0; f_idx < NUM_EQUATIONS; f_idx++) {
        double (*f)(double) = es.equation_functions[f_idx]; // Function for each equation
        const char* fn = es.equation_names[f_idx]; // Function name for each equation
        integral = 0.0; // Total approximation for each equation

        // Start timer
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Calculate h
        h = (b - a) / n;

        // Compute trapezoidal rule in serial
        integral = (f(a) + f(b)) / 2;
        x = a;
        // Loop over each trapezoid
        for (int i = 1; i <= n - 1 ; i++) {
            x += h;
            integral += f(x);
        }
        // Multiply by h to get the final approximation
        integral *= h;

        // End timer and calculate time taken
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        // Print the results
        printf("SERIAL\nFUNCTION:\t%s\nn:\t\t%d trapezoids\nInterval:\t[%f, %f]\n", fn, n, a, b);
        printf("Approximation:\t%f\nTime:\t\t%lf seconds\n\n", integral, time_taken);
    }

    return 0;
}
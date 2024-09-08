#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include "equations.h"
// sudo apt install libopenmpi-dev
// mpicc -O tram.c equations.c -o trapm -lm
// mpirun -use-hwthread-cpus ./trapm <a> <b> <n>

/**
 * Trapezoidal rule function
 * @param local_a: Start of the interval
 * @param local_b: End of the interval
 * @param local_n: Number of trapezoids
 * @param h: Step size
 * @return: The integral
 */
double trap(double  local_a, double local_b, int local_n, double h) {
    double x = local_a, integral  = (f(local_a)  + f(local_b))/2;
    for (int i = 1; i <= local_n - 1;  i++) {
        x += h;
        integral  += f(x);
    }
    integral  *= h;
    return integral;
}

/**
 * Main function
 * @param argc: Number of arguments
 * @param argv: Array of arguments
 * @return: 0 if successful
 */
int main(int  argc,  char**  argv) {
    // Declare and parse arguments to their variables
    double a, b;
    int n;
    if (argc < 4) {
        printf("Usage: %s <a> <b> <n>\n", argv[0]);
        return -1;
    }
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%d", &n);

    // Declare necessary MPI variables
    int my_rank, p, local_n;
    double h, local_a, local_b, integral, total;
    MPI_Status  status;

    // Initialize MPI
    MPI_Init(&argc,  &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,  &my_rank);  // Get process rank
    MPI_Comm_size(MPI_COMM_WORLD,  &p);        // Get number of processes

    // Timer struct to measure time taken (only for rank 0)
    struct timespec start, end;

    // Get equation information from equations.h
    EquationSet es = get_equations();

    // Loop over each function in the equation set
    for (int f_idx = 0; f_idx < NUM_EQUATIONS; f_idx++) {
        double (*f)(double) = es.equation_functions[f_idx]; // Function for each equation
        const char* fn = es.equation_names[f_idx]; // Function name for each equation

        // Compute step size and the local range for each process
        h = (b - a) / n;
        local_n = n / p;
        local_a = a + (my_rank * local_n * h);
        local_b = local_a + (local_n * h);

        // Start timer on rank 0
        if (my_rank == 0) {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }

        // Each process computes its local integral
        integral = trap(local_a,  local_b, local_n, h);

        // Reduce the results to rank 0
        if (my_rank == 0) {
            total = integral;
            for (int source = 1; source < p; source++) {
                MPI_Recv(&integral, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                total += integral;
            }
        } else {
            MPI_Send(&integral, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }

        // Rank 0 prints the results
        if (my_rank == 0) {
            // End timer
            clock_gettime(CLOCK_MONOTONIC, &end);
            double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

            // Print results
            printf("PARALLEL (MPI)\nFUNCTION:\t%s\nn:\t\t%d trapezoids\nInterval:\t[%f, %f]\n", fn, n, a, b);
            printf("Approximation:\t%f\nTime:\t\t%lf seconds\n\n", total, time_taken);
        }
    }

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
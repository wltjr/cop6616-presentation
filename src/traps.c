#include <stdio.h>
#include <time.h>
#include <math.h>

// gcc traps.c -o traps.c -lm

double f(double x){
    //return x*x;
    return x*x + 50 * (sin(100 * M_PI * x) * sin(M_PI * x));
}

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
    
    // Record start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    h = (b - a) / n;
    integral = (f(a) + f(b)) / 2;
    x = a;
    for (int i = 1; i <= n - 1 ; i++) {
        x += h;
        integral += f(x);
    }
    integral *= h;

    // Record end time
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time taken in seconds
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("With n = %d trapezoids, the serial estimate of the integral from %f to %f is: %f\n", n, a, b, integral);
    printf("Time taken: %lf seconds\n", time_taken);

    return 0;
}
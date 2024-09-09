#include <stdio.h>
#include <math.h>

// Tolerance for finding roots
#define TOLERANCE 1e-6

// gcc accuracy.c -o accuracy -lm

/**
 * Function for example function f(x) = 100/(x+5)
 * @param x: Value at which to compute the function
 * @return: Value of the function at x
 */
double f(double x) {
    return -100 / (x + 5);
}

/**
 * Function for the second derivative of the example function f''(x) = 200/(x+5)^3
 * @param x: Value at which to compute the second derivative
 * @return: Second derivative of the function at x
 */
double f_double_prime(double x) {
    return 200 / pow(x + 5, 3);
}

/** 
 * Function for the third derivative of the example function f'''(x) = 600/(x+5)^4
 * @param x: Value at which to compute the third derivative
 * @return: Third derivative of the function at x
 */
double f_triple_prime(double x) {
    return -600 / pow(x + 5, 4);
}

/**
 * Function for the fourth derivative of the example function f''''(x) = 2400/(x+5)^5
 * @param x: Value at which to compute the fourth derivative
 * @return: Fourth derivative of the function at x
 */
double f_quad_prime(double x) {
    return 2400 / pow(x + 5, 5);
}

/**
 * Bisection method to numerically find root of f'(x) = 0 in [a, b]
 * @param f: Function pointer for the third derivative f'''(x)
 * @param a: Start of interval
 * @param b: End of interval
 * @return: Approximate root or -1 if no root found
 */
double binary_search(double (*f)(double), double a, double b) {
    double mid;

    while (fabs(b - a) > TOLERANCE) {
        mid = (a + b) / 2;
        if (f(mid) == 0.0) {
            return mid;  // Found exact root
        } else if (f(a) * f(mid) < 0) {
            b = mid;  // Root lies in [a, mid]
        } else {
            a = mid;  // Root lies in [mid, b]
        }
    }

    return (a + b) / 2;  // Approximate root
}

/**
 * Function to find critical points by solving f'''(x) = 0
 * @param a: Start of interval
 * @param b: End of interval
 * @return: Critical point within interval or -1 if none found
 */
double find_critical_point(double a, double b) {
    if (f_triple_prime(a) * f_triple_prime(b) > 0) {
        return -1;  // No sign change, hence no root in the interval
    }
    return binary_search(f_triple_prime, a, b);
}

/**
 * Function to compute the maximum value of f''(x) in [a, b]
 * @param a: Start of the interval
 * @param b: End of the interval
 * @return: Maximum value of f''(x) in the interval
 */
double find_max_f_double_prime(double a, double b) {
    // Evaluate f''(x) at the boundaries
    double max_val = f_double_prime(a);

    if (f_double_prime(b) > max_val) {
        max_val = f_double_prime(b);
    }

    // Find critical points where f'''(x) = 0
    double critical_point = find_critical_point(a, b);

    // If a valid critical point exists in the interval
    if (critical_point >= a && critical_point <= b && critical_point != -1) {
        if (f_quad_prime(critical_point) < 0) {  // Local maximum
            double second_deriv_at_critical = f_double_prime(critical_point);
            if (second_deriv_at_critical > max_val) {
                max_val = second_deriv_at_critical;
            }
        }
    }

    return max_val;
}

/**
 * Function to compute required subintervals for trapezoidal rule
 * @param a: Start of the interval
 * @param b: End of the interval
 * @param desired_error: Desired accuracy
 * @return: Number of subintervals needed
 */
double compute_subintervals(double a, double b, double desired_error) {
    double max_f_double_prime = find_max_f_double_prime(a, b);

    // Compute the required number of subintervals based on the error bound
    double n = sqrt((pow(b - a, 3) * max_f_double_prime) / (12 * desired_error));

    return n;
}

/**
 * Main function to compute the required number of subintervals
 * @param argc: Number of command line arguments
 * @param argv: Command line arguments
 * @return: 0 if successful, -1 otherwise
 */
int main(int argc, char** argv) {
    // Check if the number of arguments is correct
    if (argc < 4) {
        printf("Usage: %s <a> <b> <desired_error>\n", argv[0]);
        return -1;
    }

    // Declare and parse arguments to their variables
    double a, b, desired_error;
    sscanf(argv[1], "%lf", &a);
    sscanf(argv[2], "%lf", &b);
    sscanf(argv[3], "%lf", &desired_error);

    // Compute the required number of subintervals
    unsigned int n = (unsigned int)ceil(compute_subintervals(a, b, desired_error));

    // Output the result
    printf("Required number of subintervals: %d\n", n);
    return 0;
}
#include "equations.h"
#include <math.h>

/**
 * Function to calculate x^2
 * @param x: The value of x
 * @return: The value of x^2
 */
double f(double x) {
    return x * x;
}

/**
 * Function to calculate x^2 + 50 * sin(100 * pi * x) * sin(pi * x)
 * @param x: The value of x
 * @return: The value of x^2 + 50 * sin(100 * pi * x) * sin(pi * x)
 */
double g(double x) {
    return x * x + 50 * (sin(100 * M_PI * x) * sin(M_PI * x));
}

/**
 * Function to calculate sin(30 * pi * x) + cos(23 * pi * x)
 * @param x: The value of x
 * @return: The value of sin(30 * pi * x) + cos(23 * pi * x)
 */
double h(double x) {
    return sin(30 * M_PI * x) + cos(23 * M_PI * x);
}

/**
 * Function to calculate x^3
 * @param x: The value of x
 * @return: The value of x^3
 */
double j(double x) {
    return x * x * x;
}

/**
 * Array of strings to represent the equations
 */
const char *equation_names[] = {
    "x^2",
    "x^2 + 50 * sin(100 * pi * x) * sin(pi * x)",
    "sin(30 * pi * x) + cos(23 * pi * x)",
    "x^3"
};

/**
 * Array of function pointers to the equations
 */
double (*equation_functions[])(double) = { 
    f,
    g,
    h,
    j 
};

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
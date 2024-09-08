#include "equations.h"
#include <math.h>

#define NUM_EQUATIONS 3

double f(double x) {
    return x * x;
}

double g(double x) {
    return x * x + 50 * (sin(100 * M_PI * x) * sin(M_PI * x));
}

double h(double x) {
    return sin(30 * M_PI * x) + cos(23 * M_PI * x);
}

const char *equation_names[] = {
    "x^2",
    "x^2 + 50 * sin(100 * pi * x) * sin(pi * x)",
    "sin(30 * pi * x) + cos(23 * pi * x)"
};

double (*equation_functions[])(double) = { 
    f,
    g,
    h };

EquationSet get_equations(void) {
    EquationSet set;
    set.equation_names = equation_names;
    set.equation_functions = equation_functions;
    return set;
}
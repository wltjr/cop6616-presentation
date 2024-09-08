#ifndef EQUATIONS_H
#define EQUATIONS_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NUM_EQUATIONS 4

// Function prototypes
double f(double x);
double g(double x);
double h(double x);
double j(double x);

/**
 * EquationSet: Struct to hold equation names and functions
 * @param equation_names: Array of equation names
 * @param equation_functions: Array of equation functions
 */
typedef struct {
    const char **equation_names;
    double (**equation_functions)(double);
} EquationSet;

// Function prototype to get equations
EquationSet get_equations(void);

#endif
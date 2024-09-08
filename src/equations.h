#ifndef EQUATIONS_H
#define EQUATIONS_H

double f(double x);
double g(double x);
double h(double x);

typedef struct {
    const char **equation_names;
    double (**equation_functions)(double);
} EquationSet;

EquationSet get_equations(void);

#endif
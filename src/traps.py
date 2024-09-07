#import numpy as np
import time
import sys

def f(x):
    #return np.sin(x)
    return x**2

def trapezoid_rule(a, b, n):
    h = (b - a) / n
    result = 0.5 * (f(a) + f(b))
    for i in range(1, n):
        result += f(a + i * h)
    result *= h
    return result

a = int(sys.argv[1])
b = int(sys.argv[2])
n = int(sys.argv[3])

start_time = time.time()
result_serial = trapezoid_rule(a, b, n)
end_time = time.time()

print(f"Serial result: {result_serial:50.6f}")
print(f"Serial execution time: {end_time - start_time:.6f} seconds")

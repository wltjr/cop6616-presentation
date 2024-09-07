#import numpy as np
import time
from multiprocessing import Pool
import sys

def f(x):
    #return np.sin(x)
    return x**2

def trapezoid_worker(args):
    a, h, i = args
    return f(a + i * h)

def trapezoid_rule(a, b, n, num_workers):
    h = (b - a) / n
    with Pool(processes = num_workers) as pool:
        integral_part = pool.map(trapezoid_worker, [(a, h, i) for i in range(1, n)])
    result = 0.5 * (f(a) + f(b)) + sum(integral_part)
    result *= h
    return result

a = int(sys.argv[1])
b = int(sys.argv[2])
n = int(sys.argv[3])
num_workers = 24

start_time = time.time()
result_parallel = trapezoid_rule(a, b, n, num_workers)
end_time = time.time()

print(f"Parallel result: {result_parallel:50.6f}")
print(f"Parallel execution time: {end_time - start_time:.6f} seconds")

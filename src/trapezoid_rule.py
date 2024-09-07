import time
from multiprocessing import Pool
import sys

def f(x):
    return x**2

def trapezoid_worker(args):
    a, h, i = args
    return f(a + i * h)

def trapezoid_rule(a, b, n, num_workers=None):
    h = (b - a) / n

    # run serially
    if num_workers is None:
        result = 0.5 * (f(a) + f(b))
        for i in range(1, n):
            result += f(a + i * h)

    # run in parallel
    else:
        with Pool(processes = num_workers) as pool:
            integral_part = pool.map(trapezoid_worker, [(a, h, i) for i in range(1, n)])
        result = 0.5 * (f(a) + f(b)) + sum(integral_part)

    result *= h
    return result


if len(sys.argv) < 4:
    print("Usage: {} <a> <b> <n>\n".format(sys.argv[0]));
    sys.exit()

a = int(sys.argv[1])
b = int(sys.argv[2])
n = int(sys.argv[3])
num_workers = 24

start_time = time.time()
result_serial = trapezoid_rule(a, b, n)
end_time = time.time()

print(f"Serial result: {result_serial: .6f}")
print(f"Serial runtime: {end_time - start_time:.6f} seconds\n")

start_time = time.time()
result_parallel = trapezoid_rule(a, b, n, num_workers)
end_time = time.time()

print(f"Parallel result: {result_parallel: .6f}")
print(f"Parallel runtime: {end_time - start_time:.6f} seconds")

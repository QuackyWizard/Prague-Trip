import numpy as np
import sys
import os

def generate_matrix(size, max_value=10000.0):
        matrix = np.random.uniform(1, max_value, size=(size, size))  # Random float values
        np.fill_diagonal(matrix, 0.0)  # Set diagonal to 0
        return matrix

def main(args):
    np.random.seed(int(args[2]))
    if len(args) != 3:
        print("Usage: python generator.py <size> <seed>")

    if not os.path.exists('data'):
        os.makedirs('data')

    n = int(args[1])
    for i in range(5, n):
        matrix = generate_matrix(i)
        np.savetxt(f"data/matrix_{i}.txt", matrix, fmt='%.1f', header=f"{i}", comments='')
        print(f"Generated matrix_{i}.txt")

if __name__ == "__main__":
    main(sys.argv)


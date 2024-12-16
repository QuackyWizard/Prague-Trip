import numpy as np

def generate_matrix(size, max_value=10000.0):
        matrix = np.random.uniform(1, max_value, size=(size, size))  # Random float values
        np.fill_diagonal(matrix, 0.0)  # Set diagonal to 0
        return matrix
def main(args):
    n = int(args[1])
    for i in range(5, n):
        matrix = generate_matrix(i)
        np.savetxt(f"data/matrix_{i}.txt", matrix, fmt='%.1f', header=f"{i}", comments='')

if __name__ == "__main__":
    main()
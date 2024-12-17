from itertools import permutations
import numpy as np
import random

#implementation of TSP algorithms in python

def ant_colony_optimization(matrix, n, num_ants = 100, num_iterations = 10, alpha=1.0, beta=2.0, evaporation_rate=0.5):
    pheromone = np.ones((n, n))  # Initial pheromone levels
    best_route = None
    best_length = float('inf')

    for iteration in range(num_iterations):
        routes = []
        route_lengths = []

        for ant in range(num_ants):
            visited = [False] * n
            current_city = random.randint(0, n - 1)
            route = [current_city]
            visited[current_city] = True

            for _ in range(n - 1):
                probabilities = []
                for next_city in range(n):
                    if not visited[next_city]:
                        tau = pheromone[current_city][next_city] ** alpha
                        eta = (1.0 / matrix[current_city][next_city]) ** beta
                        probabilities.append(tau * eta)
                    else:
                        probabilities.append(0)

                probabilities = np.array(probabilities)
                probabilities /= probabilities.sum()

                next_city = np.random.choice(range(n), p=probabilities)
                route.append(int(next_city))
                visited[next_city] = True
                current_city = next_city

            route.append(route[0])  # Return to start
            routes.append(route)
            route_length = sum(matrix[route[i]][route[i + 1]] for i in range(len(route) - 1))
            route_lengths.append(route_length)

            if route_length < best_length:
                best_route = route
                best_length = route_length

        # Update pheromones
        pheromone *= (1 - evaporation_rate)
        for route, route_length in zip(routes, route_lengths):
            for i in range(len(route) - 1):
                pheromone[route[i]][route[i + 1]] += 1.0 / route_length

    return best_route, best_length


def nearest_neighbour(matrix, n):
    visited = [False]*n
    route = [0]
    visited[0] = True
    total = 0
    for _ in range(n-1):
        current = route[-1]
        min_duration = float('inf')
        next_location = -1
        for i in range(n):
            if not visited[i] and matrix[current][i] < min_duration:
                min_duration = matrix[current][i]
                next_location = i
        route.append(next_location)
        visited[next_location] = True
        total += min_duration
    
    
    total += matrix[route[-1]][route[0]]
    route.append(0)

    return route, total

def calculate_total_duration(route, matrix) -> float:
    total_duration = 0
    for i in range(len(route) - 1):
        total_duration += matrix[route[i]][route[i + 1]]
    total_duration += matrix[route[-1]][route[0]]
    return total_duration

def brute_force(matrix, n):
    locations = [i for i in range(1, n)]
    routes = permutations(locations)
    routes = [(0,) + r for r in routes]

    min_duration = float('inf')
    optimal_route = None
    for route in routes:
        duration = calculate_total_duration(route, matrix)
        if duration < min_duration:
            min_duration = duration
            optimal_route = route
    return optimal_route + (0,), min_duration

from itertools import combinations

def held_karp(matrix, n):
    
    dp = {}
    # Base case: only the starting point visited
    for i in range(1, n):
        dp[(1 << i, i)] = (matrix[0][i], 0)

    # Iterate over subsets of increasing size
    for subset_size in range(2, n):
        for subset in combinations(range(1, n), subset_size):
            bits = 0
            for bit in subset:
                bits |= 1 << bit
            for next_city in subset:
                prev_bits = bits & ~(1 << next_city)
                res = []
                for k in subset:
                    if k == next_city:
                        continue
                    res.append((dp[(prev_bits, k)][0] + matrix[k][next_city], k))
                dp[(bits, next_city)] = min(res)

    # Find optimal route
    bits = (2**n - 1) - 1  # All nodes visited except the start
    res = []
    for k in range(1, n):
        res.append((dp[(bits, k)][0] + matrix[k][0], k))
    opt, parent = min(res)

    # Reconstruct the path
    path = []
    bits = (2**n - 1) - 1
    for _ in range(n - 1):
        path.append(parent)
        bits, parent = bits & ~(1 << parent), dp[(bits, parent)][1]
    path.append(0)
    return [0] + list(reversed(path)), opt



from itertools import permutations

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





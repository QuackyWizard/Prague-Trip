/**
 * @file tsp_algorithms.cpp
 * @brief Implementation of multiple algorithms for solving the Traveling Salesman Problem (TSP).
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <climits>
#include <numeric>
#include <map>
#include <tuple>
#include <set>
#include <random>
#include <cfloat>
#include <unordered_map>
#include <bitset>
#include <fstream>
#include <string>
#include "json.hpp"
#include <chrono>
using json = nlohmann::json; ///< Alias for the JSON library
using namespace std;

/**
 * @brief Calculates the total duration of a given TSP route.
 * @param route A vector representing the sequence of nodes in the route.
 * @param matrix A 2D matrix representing distances between nodes.
 * @return The total duration of the route.
 */
double calculateTotalDuration(const vector<int>& route, const vector<vector<double>>& matrix) {
    double totalDuration = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        totalDuration += matrix[route[i]][route[i + 1]];
    }
    totalDuration += matrix[route.back()][route[0]]; // Add return to the starting point
    return totalDuration;
}

/**
 * @brief Solves the TSP using the Nearest Neighbor algorithm.
 * @param matrix A 2D matrix representing distances between nodes.
 * @param n The number of nodes in the graph.
 * @return A pair consisting of the optimal route and its total duration.
 */
pair<vector<int>, double> nearestNeighbour(const vector<vector<double>>& matrix, int n) {
    vector<bool> visited(n, false); ///< Tracks visited nodes
    vector<int> route = {0};        ///< Start from the first node
    visited[0] = true;
    double total = 0;

    for (int step = 0; step < n - 1; ++step) {
        int current = route.back(); ///< Current location
        double minDuration = DBL_MAX;
        int nextLocation = -1;

        // Find the nearest unvisited neighbor
        for (int i = 0; i < n; ++i) {
            if (!visited[i] && matrix[current][i] < minDuration) {
                minDuration = matrix[current][i];
                nextLocation = i;
            }
        }

        route.push_back(nextLocation);
        visited[nextLocation] = true;
        total += minDuration;
    }

    // Add the distance back to the starting point
    total += matrix[route.back()][0];
    route.push_back(0); // Complete the route by returning to the start
    return {route, total};
}

/**
 * @brief Solves the TSP using a brute force approach.
 * @param matrix A 2D matrix representing distances between nodes.
 * @param n The number of nodes in the graph.
 * @return A pair consisting of the optimal route and its total duration.
 */
pair<vector<int>, double> bruteForce(const vector<vector<double>>& matrix, int n) {
    vector<int> locations(n - 1);
    iota(locations.begin(), locations.end(), 1); // Generate {1, 2, ..., n-1}

    double minDuration = DBL_MAX;
    vector<int> optimalRoute;

    do {
        vector<int> route = {0}; // Start at the first node
        route.insert(route.end(), locations.begin(), locations.end());
        route.push_back(0); // Return to the start

        double duration = calculateTotalDuration(route, matrix);
        if (duration < minDuration) {
            minDuration = duration;
            optimalRoute = route;
        }
    } while (next_permutation(locations.begin(), locations.end())); // Try all permutations

    return {optimalRoute, minDuration};
}

/**
 * @brief Solves the TSP using the Ant Colony Optimization (ACO) algorithm.
 * @param matrix A 2D matrix representing distances between nodes.
 * @param n The number of nodes in the graph.
 * @param numAnts The number of ants to simulate.
 * @param numIterations The number of iterations to run the algorithm.
 * @param alpha The importance of pheromone strength in decision-making.
 * @param beta The importance of distance in decision-making.
 * @param evaporationRate The rate at which pheromones evaporate.
 * @return A pair consisting of the best route found and its total duration.
 */
pair<vector<int>, double> antColonyOptimization(const vector<vector<double>>& matrix, int n, int numAnts = 100, int numIterations = 10, double alpha = 1.0, double beta = 2.0, double evaporationRate = 0.5) {
    vector<vector<double>> pheromone(n, vector<double>(n, 1.0)); ///< Initial pheromone levels
    vector<int> bestRoute;
    double bestLength = DBL_MAX;

    random_device rd;
    mt19937 gen(rd()); ///< Random number generator

    for (int iteration = 0; iteration < numIterations; ++iteration) {
        vector<vector<int>> routes;    ///< Routes taken by ants
        vector<double> routeLengths;  ///< Lengths of these routes

        for (int ant = 0; ant < numAnts; ++ant) {
            vector<bool> visited(n, false);
            vector<int> route;
            int currentCity = gen() % n; // Random start city

            route.push_back(currentCity);
            visited[currentCity] = true;

            for (int step = 0; step < n - 1; ++step) {
                vector<double> probabilities(n, 0.0);
                double total = 0.0;

                // Calculate probabilities based on pheromone and distance
                for (int nextCity = 0; nextCity < n; ++nextCity) {
                    if (!visited[nextCity]) {
                        double tau = pow(pheromone[currentCity][nextCity], alpha);
                        double eta = pow(1.0 / matrix[currentCity][nextCity], beta);
                        probabilities[nextCity] = tau * eta;
                        total += probabilities[nextCity];
                    }
                }

                for (double& p : probabilities) p /= total; // Normalize probabilities

                // Choose the next city based on probabilities
                discrete_distribution<int> dist(probabilities.begin(), probabilities.end());
                int nextCity = dist(gen);
                route.push_back(nextCity);
                visited[nextCity] = true;
                currentCity = nextCity;
            }

            route.push_back(route[0]); // Return to start
            double routeLength = calculateTotalDuration(route, matrix);

            // Update the best route
            if (routeLength < bestLength) {
                bestRoute = route;
                bestLength = routeLength;
            }

            routes.push_back(route);
            routeLengths.push_back(routeLength);
        }

        // Evaporate pheromones
        for (auto& row : pheromone) {
            for (double& value : row) {
                value *= (1 - evaporationRate);
            }
        }

        // Update pheromones based on routes
        for (size_t i = 0; i < routes.size(); ++i) {
            const auto& route = routes[i];
            double routeLength = routeLengths[i];
            for (size_t j = 0; j < route.size() - 1; ++j) {
                pheromone[route[j]][route[j + 1]] += 1.0 / routeLength;
            }
        }
    }

    return {bestRoute, bestLength};
}

/**
 * @brief Solves the TSP using the Held-Karp dynamic programming algorithm.
 * @param matrix A 2D matrix representing distances between nodes.
 * @param n The number of nodes in the graph.
 * @return A pair consisting of the optimal route and its total duration.
 */
pair<vector<int>, double> heldKarp(const vector<vector<double>>& matrix, int n) {
    vector<vector<int>> dp(1 << n, vector<int>(n, INT_MAX)); ///< DP table
    vector<vector<int>> parent(1 << n, vector<int>(n, -1)); ///< Parent table for backtracking

    // Base case: direct paths from the starting node
    for (int i = 0; i < n; ++i) {
        dp[1 << i][i] = matrix[0][i];
    }

    // Fill the DP table
    for (int mask = 1; mask < (1 << n); ++mask) {
        for (int i = 0; i < n; ++i) {
            if (mask & (1 << i)) {
                for (int j = 0; j < n; ++j) {
                    if (mask & (1 << j)) {
                        if (dp[mask ^ (1 << i)][j] + matrix[j][i] < dp[mask][i]) {
                            dp[mask][i] = dp[mask ^ (1 << i)][j] + matrix[j][i];
                            parent[mask][i] = j;
                        }
                    }
                }
            }
        }
    }

    // Find the minimum route
    int last = -1;
    double minDuration = DBL_MAX;
    for (int i = 0; i < n; ++i) {
        if (dp[(1 << n) - 1][i] + matrix[i][0] < minDuration) {
            minDuration = dp[(1 << n) - 1][i] + matrix[i][0];
            last = i;
        }
    }

    // Backtrack to find the optimal route
    vector<int> route;
    int mask = (1 << n) - 1;
    while (last != -1) {
        route.push_back(last);
        int newLast = parent[mask][last];
        mask ^= (1 << last);
        last = newLast;
    }
    route.push_back(0); // Add the starting node
    reverse(route.begin(), route.end());

    return {route, minDuration};
}

/**
 * @brief Main function to read input data, execute TSP algorithms, and save results.
 * @return 0 on successful execution, non-zero on error.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @note The command-line argument is the number of files to process.
 */
int main(int argc, char* argv[]) {
    string filename;
    string outputFilename;
    int n;
    vector<int> route;
    double length;

    vector<string> algorithms = {
    "Nearest Neighbor", 
    "Brute Force", 
    "Ant Colony Optimization", 
    "Held-Karp"
    }; ///< List of algorithms to run 
    
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << "<start> <end>" << endl;
        return 1;
    }

    int end = std::stoi(argv[2]);
    int start = std::stoi(argv[1]);

    for (int i = start; i < end+1; i++) {
        filename = "data/matrix_" + to_string(i) + ".txt"; // Read the matrix from a file (from the "data" folder)
        ifstream inputFile(filename);

        if (!inputFile) {
            cerr << "Error: File could not be opened!" << endl;
            return 1;
        }

        cout << "Reading matrix from " << filename << endl;

        // Read the matrix size
        inputFile >> n;
        vector<vector<double>> matrix(n, vector<double>(n));

        // Read the matrix data
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                inputFile >> matrix[i][j];
            }
        }

        inputFile.close();

        json results;
       
        // Execute each algorithm
        for (const string& algorithm : algorithms) {
            auto start = chrono::high_resolution_clock::now();

            if (algorithm == "Nearest Neighbor") {
                cout << "Running Nearest Neighbor Algorithm..." << endl;
                auto result = nearestNeighbour(matrix, n);
                route = result.first;
                length = result.second;
            } else if (algorithm == "Brute Force") {
                cout << "Running Brute Force Algorithm..." << endl;
                auto result = bruteForce(matrix, n);
                route = result.first;
                length = result.second;
            } else if (algorithm == "Ant Colony Optimization") {
                cout << "Running Ant Colony Optimization Algorithm..." << endl;
                auto result = antColonyOptimization(matrix, n);
                route = result.first;
                length = result.second;
            } else if (algorithm == "Held-Karp") {
                cout << "Running Held-Karp Algorithm..." << endl;
                auto result = heldKarp(matrix, n);
                route = result.first;
                length = result.second;
            }

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

            // Save the results in JSON format
            results[algorithm] = {
                {"route", route},
                {"duration", length},
                {"time", duration.count()},
                {"size", i}
            };

        }

        // Write results to a JSON file (into the "output" folder)
        outputFilename = "output/output_" + to_string(i) + ".json";
        ofstream outputFile(outputFilename);
        if (!outputFile) {
            cerr << "Error: Could not open the file for writing!" << endl;
            return 1;
        }

        outputFile << results.dump(4);
        outputFile.close();
        cout << "Results saved to " << outputFilename << endl;
    }

    return 0;
}

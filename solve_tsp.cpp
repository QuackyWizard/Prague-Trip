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

using json = nlohmann::json;
using namespace std;

// Function to calculate total duration of a route
double calculateTotalDuration(const vector<int>& route, const vector<vector<double>>& matrix) {
    double totalDuration = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        totalDuration += matrix[route[i]][route[i + 1]];
    }
    totalDuration += matrix[route.back()][route[0]];
    return totalDuration;
}

// Nearest Neighbor Algorithm
pair<vector<int>, double> nearestNeighbour(const vector<vector<double>>& matrix, int n) {
    vector<bool> visited(n, false);
    vector<int> route = {0};
    visited[0] = true;
    double total = 0;

    for (int step = 0; step < n - 1; ++step) {
        int current = route.back();
        double minDuration = DBL_MAX;
        int nextLocation = -1;

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

    total += matrix[route.back()][0];
    route.push_back(0);
    return {route, total};
}

// Brute Force Algorithm
pair<vector<int>, double> bruteForce(const vector<vector<double>>& matrix, int n) {
    vector<int> locations(n - 1);
    iota(locations.begin(), locations.end(), 1);

    double minDuration = DBL_MAX;
    vector<int> optimalRoute;

    do {
        vector<int> route = {0};
        route.insert(route.end(), locations.begin(), locations.end());
        route.push_back(0);

        double duration = calculateTotalDuration(route, matrix);
        if (duration < minDuration) {
            minDuration = duration;
            optimalRoute = route;
        }
    } while (next_permutation(locations.begin(), locations.end()));

    return {optimalRoute, minDuration};
}



// Ant Colony Optimization (ACO)
pair<vector<int>, double> antColonyOptimization(const vector<vector<double>>& matrix, int n, int numAnts = 100, int numIterations = 10, double alpha = 1.0, double beta = 2.0, double evaporationRate = 0.5) {
    vector<vector<double>> pheromone(n, vector<double>(n, 1.0));
    vector<int> bestRoute;
    double bestLength = DBL_MAX;

    random_device rd;
    mt19937 gen(rd());

    for (int iteration = 0; iteration < numIterations; ++iteration) {
        vector<vector<int>> routes;
        vector<double> routeLengths;

        for (int ant = 0; ant < numAnts; ++ant) {
            vector<bool> visited(n, false);
            vector<int> route;
            int currentCity = gen() % n;

            route.push_back(currentCity);
            visited[currentCity] = true;

            for (int step = 0; step < n - 1; ++step) {
                vector<double> probabilities(n, 0.0);
                double total = 0.0;

                for (int nextCity = 0; nextCity < n; ++nextCity) {
                    if (!visited[nextCity]) {
                        double tau = pow(pheromone[currentCity][nextCity], alpha);
                        double eta = pow(1.0 / matrix[currentCity][nextCity], beta);
                        probabilities[nextCity] = tau * eta;
                        total += probabilities[nextCity];
                    }
                }

                for (double& p : probabilities) p /= total;

                discrete_distribution<int> dist(probabilities.begin(), probabilities.end());
                int nextCity = dist(gen);
                route.push_back(nextCity);
                visited[nextCity] = true;
                currentCity = nextCity;
            }

            route.push_back(route[0]);
            double routeLength = calculateTotalDuration(route, matrix);

            if (routeLength < bestLength) {
                bestRoute = route;
                bestLength = routeLength;
            }

            routes.push_back(route);
            routeLengths.push_back(routeLength);
        }

        // Update pheromones
        for (auto& row : pheromone) {
            for (double& value : row) {
                value *= (1 - evaporationRate);
            }
        }

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

// Held-Karp Algorithm
pair<vector<int>, double> heldKarp(const vector<vector<double>>& matrix, int n) {
    vector<vector<int>> dp(1 << n, vector<int>(n, INT_MAX));
    vector<vector<int>> parent(1 << n, vector<int>(n, -1));

    for (int i = 0; i < n; ++i) {
        dp[1 << i][i] = matrix[0][i];
    }

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

    int last = -1;
    double minDuration = DBL_MAX;
    for (int i = 0; i < n; ++i) {
        if (dp[(1 << n) - 1][i] + matrix[i][0] < minDuration) {
            minDuration = dp[(1 << n) - 1][i] + matrix[i][0];
            last = i;
        }
    }

    vector<int> route;
    int mask = (1 << n) - 1;
    while (last != -1) {
        route.push_back(last);
        int newLast = parent[mask][last];
        mask ^= (1 << last);
        last = newLast;
    }
    route.push_back(0);
    reverse(route.begin(), route.end());

    return {route, minDuration};
}

int main() {
    string filename;
    string outputFilename;
    int n;
    vector<int> route;
    double length;
    vector<string> algorithms = {"Nearest Neighbor", "Brute Force", "Ant Colony Optimization", "Held-Karp"};
    for (int i = 5; i < 100; ++i) {
        filename = "data/matrix_" + to_string(i) + ".txt";
        ifstream inputFile(filename); 
        if (!inputFile) {
            cerr << "Error: File could not be opened!" << endl;
            
            return 1;
        }
        cout << "Reading matrix from " << filename << endl;
        
        inputFile >> n; // Read dimensions from the first line
        vector<vector<double>> matrix;
         // Create a 2D vector to store the matrix
        matrix.resize(n, vector<double>(n));

        // Read the matrix values
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                inputFile >> matrix[i][j];
            }
        }

        inputFile.close(); // Close the file

        
        json results;

        // Loop through each algorithm and save its result
        for (const string& algorithm : algorithms) {
        
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

            // Add the result to the JSON object
            results[algorithm] = {
                {"route", route},
                {"duration", length}
            };
        }

        outputFilename = "output/output_" + to_string(i) + ".json";
        // Save the JSON object to a file
        ofstream outputFile(outputFilename);
        if (!outputFile) {
            cerr << "Error: Could not open the file for writing!" << endl;
            return 1;
        }

        outputFile << results.dump(4); // Pretty-print with an indentation of 4 spaces
        outputFile.close();
        cout << "Results saved to " << outputFilename << endl;

    }

    return 0;
}



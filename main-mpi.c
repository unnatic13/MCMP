#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <math.h>

// Function to calculate distance between two points
double computeDistance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// Function to check if a point is already in the tour
bool contains(int* tour, int size, int point) {
    for (int i = 0; i < size; i++) {
        if (tour[i] == point) {
            return true;
        }
    }
    return false;
}

// Function to find the nearest neighbor of a given point
int findNearestNeighbor(int* tour, int size, double** coords, int numOfCoords) {
    double minDistance = DBL_MAX;
    int nearestNeighbor = -1;
    int lastPoint = tour[size - 1];

    for (int i = 0; i < numOfCoords; i++) {
        if (!contains(tour, size, i)) {
            double distance = computeDistance(coords[lastPoint][0], coords[lastPoint][1], coords[i][0], coords[i][1]);
            if (distance < minDistance) {
                minDistance = distance;
                nearestNeighbor = i;
            }
        }
    }
    return nearestNeighbor;
}

// Function to compute the tour using Nearest Insertion
void nearestInsertion(int* tour, int* size, double** coords, int numOfCoords) {
    // Start with the first two points in the tour
    tour[0] = 0;
    tour[1] = findNearestNeighbor(tour, 1, coords, numOfCoords);
    *size = 2;

    // Continue adding points to the tour
    while (*size < numOfCoords) {
        int minInsertPoint = -1;
        int minInsertPosition = -1;
        double minInsertCost = DBL_MAX;

        for (int i = 0; i < *size; i++) {
            for (int j = 0; j < numOfCoords; j++) {
                if (!contains(tour, *size, j)) {
                    double cost = computeDistance(coords[tour[i]][0], coords[tour[i]][1], coords[j][0], coords[j][1]);
                    if (cost < minInsertCost) {
                        minInsertCost = cost;
                        minInsertPoint = j;
                        minInsertPosition = i;
                    }
                }
            }
        }

        // Insert the nearest point into the tour
        for (int i = *size; i > minInsertPosition + 1; i--) {
            tour[i] = tour[i - 1];
        }
        tour[minInsertPosition + 1] = minInsertPoint;
        (*size)++;
    }
}

// Example usage:
int main() {
    int numOfCoords = 5;
    double** coords = (double**)malloc(numOfCoords * sizeof(double*));
    for (int i = 0; i < numOfCoords; i++) {
        coords[i] = (double*)malloc(2 * sizeof(double));
    }
    // Initialize coordinates (x, y)
    // ...

    int* tour = (int*)malloc(numOfCoords * sizeof(int));
    int size = 0;

    nearestInsertion(tour, &size, coords, numOfCoords);

    // Print the tour
    printf("Tour: ");
    for (int i = 0; i < numOfCoords; i++) {
        printf("%d -> ", tour[i]);
    }
    printf("%d\n", tour[0]);

    // Cleanup
    for (int i = 0; i < numOfCoords; i++) {
        free(coords[i]);
    }
    free(coords);
    free(tour);

    return 0;
}

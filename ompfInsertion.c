#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "coordReader.c"


// Function to compute the Euclidean distance between two points
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// Function to read the number of coordinates from a file
int readNumOfCoords(const char *filename) {
    // Implementation omitted for brevity
    // Returns the number of coordinates
}

// Function to read coordinates from a file
double **readCoords(const char *filename, int numCoords) {
    // Implementation omitted for brevity
    // Returns a 2D array containing the coordinates
}

// Function to write the tour to a file
void writeTourToFile(int *tour, int tourLength, const char *filename) {
    // Implementation omitted for brevity
    // Writes the tour to the specified file
}

// Function to find the farthest point from the current tour
int findFarthestPoint(int *tour, int numCoords, double **coords, int *visited) {
    int farthestPoint = -1;
    double maxDistance = -1.0;

    #pragma omp parallel for
    for (int i = 0; i < numCoords; i++) {
        if (!visited[i]) {
            double minDist = INFINITY;

            // Find the closest point in the tour to the current point
            for (int j = 1; j < tourLength; j++) {
                int u = tour[j - 1];
                int v = tour[j];
                double dist = distance(coords[u][0], coords[u][1], coords[i][0], coords[i][1]);

                if (dist < minDist) {
                    minDist = dist;
                }
            }

            // Update the farthest point if necessary
            if (minDist > maxDistance) {
                #pragma omp critical
                {
                    if (minDist > maxDistance) {
                        maxDistance = minDist;
                        farthestPoint = i;
                    }
                }
            }
        }
    }

    return farthestPoint;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char *inputFile = argv[1];
    const char *outputFile = argv[2];

    int numCoords = readNumOfCoords(inputFile);
    double **coords = readCoords(inputFile, numCoords);

    int *tour = (int *)malloc(numCoords * sizeof(int));
    int tourLength = 0;

    // Initialize the tour with the first two points
    tour[0] = 0;
    tour[1] = 1;
    tourLength = 2;

    int *visited = (int *)calloc(numCoords, sizeof(int));
    visited[0] = 1;
    visited[1] = 1;

    // Main loop for the Farthest Insertion heuristic
    for (int i = 2; i < numCoords; i++) {
        int farthestPoint = findFarthestPoint(tour, numCoords, coords, visited);
        // Insert the farthest point into the tour
        // (Implementation omitted for brevity)
    }

    // Write the tour to the output file
    writeTourToFile(tour, tourLength, outputFile);

    free(visited);
    free(tour);
    // Free the memory for coords (Implementation omitted for brevity)

    return 0;
}

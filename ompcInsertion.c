#include <stdio.h>
#include <stdlib.h>
#include "coordReader.c"
#include <float.h>
#include <stdbool.h>

// Assume these functions are defined in coordReader.c
extern int readNumOfCoords(const char *filename);
extern double** readCoords(const char *filename, int numCoords);
extern void writeTourToFile(int *tour, int tourLength, const char *filename);

// Function prototypes
void cheapestInsertion(int numCoords, double **coords, int *tour, int *tourLength);

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

    cheapestInsertion(numCoords, coords, tour, &tourLength);

    writeTourToFile(tour, tourLength, outputFile);

    free(tour);
    for (int i = 0; i < numCoords; ++i) {
        free(coords[i]);
    }
    free(coords);

    return 0;
}

void cheapestInsertion(int numCoords, double **coords, int *tour, int *tourLength) {
    bool *visited = (bool *)malloc(numCoords * sizeof(bool));
    for (int i = 0; i < numCoords; ++i) {
        visited[i] = false;
    }

    // Start from the first coordinate
    tour[0] = 0;
    visited[0] = true;
    *tourLength = 1;

    #pragma omp parallel for
    for (int i = 1; i < numCoords; ++i) {
        int nearestVertex = -1;
        double minDistance = DBL_MAX;
        int positionToInsert = -1;

        for (int j = 0; j < numCoords; ++j) {
            if (!visited[j]) {
                for (int k = 0; k < *tourLength; ++k) {
                    double dist = pow(coords[tour[k]][0] - coords[j][0], 2) +
                                  pow(coords[tour[k]][1] - coords[j][1], 2);
                    if (dist < minDistance) {
                        minDistance = dist;
                        nearestVertex = j;
                        positionToInsert = k;
                    }
                }
            }
        }

        // Insert the nearest vertex into the tour
        #pragma omp critical
        {
            for (int k = *tourLength; k > positionToInsert; --k) {
                tour[k] = tour[k - 1];
            }
            tour[positionToInsert] = nearestVertex;
            visited[nearestVertex] = true;
            *tourLength += 1;
        }
    }

    free(visited);
}

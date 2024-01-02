#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <math.h>

// Assume these functions are defined in coordReader.c
extern int readNumOfCoords(const char *filename);
extern double** readCoords(const char *filename, int numCoords);
extern void writeTourToFile(int *tour, int tourLength, const char *filename);

// Function prototypes
void cheapestInsertion(int numCoords, double **coords, int *tour, int *tourLength);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    const char *outputFile = argv[1];

    // Directly initializing coordinates
    double exampleCoords[10][2] = {
        {867.8813832218043, 399.10334472405395},
        {263.5067701401023, 95.23581230656842},
        {949.1191382487299, 644.5029485036863},
        {940.7453325005885, 977.8121745416246},
        {968.1971754167525, 201.96289606671135},
        {978.9460051753653, 734.4678390438083},
        {426.372831602882, 293.7296237694936},
        {556.6539038363909, 719.5543505449659},
        {143.18918666443426, 756.1858968640379}
        // Add all your coordinates similarly
    };

    int numCoords = sizeof(exampleCoords) / (2 * sizeof(double));
    double **coords = (double **)malloc(numCoords * sizeof(double *));
    for (int i = 0; i < numCoords; ++i) {
        coords[i] = (double *)malloc(2 * sizeof(double));
        coords[i][0] = exampleCoords[i][0];
        coords[i][1] = exampleCoords[i][1];
    }

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

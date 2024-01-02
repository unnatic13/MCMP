#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "coordReader.c"


// Function prototypes from coordReader.c
int readNumOfCoords(const char *nine_coords);
void readCoords(const char *nine_coords, double coords[][2], int numCoords);
void writeTourToFile(int *tour, int tourLength, const char *nine_coords);

// Structure to represent a vertex
typedef struct {
    double x, y;
} Vertex;

// Function to calculate the Euclidean distance between two vertices
double calculateDistance(Vertex v1, Vertex v2) {
    return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

// Function to perform Nearest Addition algorithm in parallel using OpenMP
void nearestAdditionParallel(Vertex vertices[], int numVertices, int *tour) {
    int visited[numVertices]; // Array to keep track of visited vertices
    int current = 0;          // Start from the first vertex
    int next;                 // Next vertex to visit

    // Initialize tour and visited array
    for (int i = 0; i < numVertices; i++) {
        tour[i] = -1;
        visited[i] = 0;
    }

    tour[0] = current;  // Start from the first vertex
    visited[current] = 1;

    for (int i = 1; i < numVertices; i++) {
        double minDistance = INFINITY;  // Initialize with infinity
        next = -1;                      // Reset next vertex

        // Find the nearest unvisited vertex
        #pragma omp parallel for
        for (int j = 0; j < numVertices; j++) {
            if (!visited[j]) {
                double dist = calculateDistance(vertices[current], vertices[j]);
                if (dist < minDistance) {
                    minDistance = dist;
                    next = j;
                }
            }
        }

        tour[i] = next;       // Add the nearest vertex to the tour
        visited[next] = 1;   // Mark it as visited
        current = next;       // Update current vertex
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <coordinate_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *inputFile = argv[1];
    const char *outputFile = argv[2];

    int numVertices = readNumOfCoords(inputFile);
    Vertex vertices[numVertices];
    readCoords(inputFile, (double (*)[2]) vertices, numVertices);

    int tour[numVertices];
    nearestAdditionParallel(vertices, numVertices, tour);

    writeTourToFile(tour, numVertices, outputFile);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct {
    double x;
    double y;
} Point;

double distance(Point p1, Point p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

int nearestNeighbor(Point *coords, int numCoords, int start, int *visited) {
    int nearest = -1;
    double minDist = INFINITY;

    for (int i = 0; i < numCoords; i++) {
        if (!visited[i] && distance(coords[start], coords[i]) < minDist) {
            minDist = distance(coords[start], coords[i]);
            nearest = i;
        }
    }

    return nearest;
}

void nearestInsertion(Point *coords, int numCoords, int *tour) {
    int *visited = (int *)calloc(numCoords, sizeof(int));
    int current = 0; // Starting point

    tour[0] = current;
    visited[current] = 1;

    for (int i = 1; i < numCoords; i++) {
        int next = nearestNeighbor(coords, numCoords, current, visited);
        tour[i] = next;
        visited[next] = 1;
        current = next;
    }

    free(visited);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    if (argc != 3) {
        if (rank == 0) printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    const char *inputFile = argv[1];
    const char *outputFile = argv[2];

    Point *allCoords = NULL;
    int totalNumCoords = 0;

    if (rank == 0) {
    FILE *outputFilePtr = fopen(outputFile, "w");
    if (outputFilePtr == NULL) {
        printf("Error opening output file.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Assuming allTours contains the full tour (gathered from all processes)
    for (int i = 0; i < totalNumCoords; i++) {
        fprintf(outputFilePtr, "%d\n", allTours[i]);
    }

    fclose(outputFilePtr);
}

    // Broadcast total number of coordinates to all processes
    MPI_Bcast(&totalNumCoords, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Determine local portion of coordinates
    int localNumCoords = totalNumCoords / numProcs;
    Point *localCoords = (Point *)malloc(localNumCoords * sizeof(Point));

    // Scatter allCoords to localCoords
    MPI_Scatter(allCoords, localNumCoords * 2, MPI_DOUBLE, localCoords, localNumCoords * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Allocate memory for tour and compute nearest insertion
    int *localTour = (int *)malloc(localNumCoords * sizeof(int));
    nearestInsertion(localCoords, localNumCoords, localTour);

    // Gather local tours to master process
    int *allTours = NULL;
    if (rank == 0) {
        allTours = (int *)malloc(totalNumCoords * sizeof(int));
    }
    MPI_Gather(localTour, localNumCoords, MPI_INT, allTours, localNumCoords, MPI_INT, 0, MPI_COMM_WORLD);

    // Master process writes the result to the output file
    if (rank == 0) {
        // Write the tour to the output file
        // ...
    }

    // Clean up
    free(localCoords);
    free(localTour);
    if (rank == 0) {
        free(allCoords);
        free(allTours);
    }

    MPI_Finalize();

    return 0;
}

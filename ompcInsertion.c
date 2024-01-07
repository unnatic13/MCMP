#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "coordReader.c"
#include <limits.h>

// Structure to represent a point
typedef struct {
    double x;
    double y;
} Point;

// Function prototypes
double distance(Point a, Point b);
void cheapestInsertion(Point* points, int n, int* tour);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <coordinate_file>\n", argv[0]);
        exit(1);
    }

    int numOfCoords = readNumOfCoords(argv[1]);
    Point* points = readCoords(argv[1], numOfCoords);
    int tour[numOfCoords];

    // Find the cheapest insertion tour in parallel
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < numOfCoords; i++) {
            Point localPoints[numOfCoords];
            int localTour[numOfCoords];
            
            // Copy the original points array
            for (int j = 0; j < numOfCoords; j++) {
                localPoints[j] = points[j];
            }

            // Start the tour from point i
            int start = i;
            localTour[0] = start;
            localTour[1] = (start + 1) % numOfCoords;

            // Initialize the rest of the tour
            for (int j = 2; j < numOfCoords; j++) {
                localTour[j] = -1;
            }

            // Find the cheapest insertion for the current start point
            cheapestInsertion(localPoints, numOfCoords, localTour);

            // Update the global tour if the local tour is shorter
            #pragma omp critical
            {
                if (distance(points[localTour[numOfCoords - 1]], points[localTour[0]]) < 
                    distance(points[tour[numOfCoords - 1]], points[tour[0]])) {
                    for (int j = 0; j < numOfCoords; j++) {
                        tour[j] = localTour[j];
                    }
                }
            }
        }
    }

    // Write the tour to an output file
    writeTourToFile(tour, numOfCoords, "ompcInsertion_output.txt");

    // Free allocated memory
    free(points);

    return 0;
}

double distance(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void cheapestInsertion(Point* points, int n, int* tour) {
    // Initialize the tour with the first two points
    tour[0] = 0;
    tour[1] = 1;

    // Create an array to keep track of visited points
    int visited[n];
    for (int i = 0; i < n; i++) {
        visited[i] = 0;
    }
    visited[0] = visited[1] = 1;

    // Find the cheapest insertion for the remaining points
    for (int i = 2; i < n; i++) {
        int bestInsert = -1;
        int bestCost = INT_MAX;

        for (int j = 0; j < i; j++) {
            if (!visited[j]) {
                // Compute the cost of inserting point i between j and j+1
                double currentCost = distance(points[tour[j]], points[i]) +
                                     distance(points[i], points[tour[(j + 1) % i]]);

                if (currentCost < bestCost) {
                    bestCost = currentCost;
                    bestInsert = j;
                }
            }
        }

        // Insert point i in the best position found
        for (int j = i; j > bestInsert + 1; j--) {
            tour[j] = tour[j - 1];
        }
        tour[bestInsert + 1] = i;

        // Mark the inserted point as visited
        visited[i] = 1;
    }
}

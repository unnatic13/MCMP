#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "coordReader.c"

// Structure to represent a point
typedef struct {
    double x;
    double y;
} Point;

// Function prototypes
double distance(Point a, Point b);
void farthestInsertion(Point* points, int n, int* tour);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <coordinate_file>\n", argv[0]);
        exit(1);
    }

    int numOfCoords = readNumOfCoords(argv[1]);
    Point* points = readCoords(argv[1], numOfCoords);
    int tour[numOfCoords];

    // Find the farthest insertion tour in parallel
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

            // Initialize the rest of the tour
            for (int j = 1; j < numOfCoords; j++) {
                localTour[j] = -1;
            }

            // Find the farthest insertion for the current start point
            farthestInsertion(localPoints, numOfCoords, localTour);

            // Update the global tour if the local tour is longer
            #pragma omp critical
            {
                if (distance(points[localTour[numOfCoords - 1]], points[localTour[0]]) > 
                    distance(points[tour[numOfCoords - 1]], points[tour[0]])) {
                    for (int j = 0; j < numOfCoords; j++) {
                        tour[j] = localTour[j];
                    }
                }
            }
        }
    }

    // Write the tour to an output file
    writeTourToFile(tour, numOfCoords, "ompfInsertion_output.txt");

    // Free allocated memory
    free(points);

    return 0;
}

double distance(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void farthestInsertion(Point* points, int n, int* tour) {
    // Implement the Farthest Insertion algorithm
    // This function will find the tour with the farthest insertion
    // and store it in the 'tour' array
}

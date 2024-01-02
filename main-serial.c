#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure to represent a coordinate
typedef struct {
    double x;
    double y;
} Coordinate;

int readNumOfCoords(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Return -1 to indicate error
    }

    int count = 0;
    double x, y;
    while (fscanf(file, "%lf,%lf", &x, &y) == 2) {
        count++;
    }

    fclose(file);
    return count;
}

void readCoords(const char *filename, Coordinate coords[], int numOfCoords) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file.\n");
        exit(1);
    }

    for (int i = 0; i < numOfCoords; i++) {
        if (fscanf(file, "%lf,%lf", &(coords[i].x), &(coords[i].y)) != 2) {
            printf("Error reading coordinates from file.\n");
            exit(1);
        }
    }

    fclose(file);
}

double computeDistance(Coordinate a, Coordinate b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void nearestInsertion(Coordinate coords[], int numOfCoords, Coordinate tour[], int *tourLength) {
    // Initialize the tour with the first coordinate
    tour[0] = coords[0];
    *tourLength = 1;

    // Start with the second coordinate
    for (int i = 1; i < numOfCoords; i++) {
        double minDist = INFINITY;
        int insertPos = -1;

        for (int j = 0; j < *tourLength; j++) {
            double dist = computeDistance(coords[i], tour[j]);

            if (dist < minDist) {
                minDist = dist;
                insertPos = j;
            }
        }

        // Shift elements to make space for new coordinate
        for (int j = *tourLength; j > insertPos + 1; j--) {
            tour[j] = tour[j - 1];
        }

        // Insert the new coordinate
        tour[insertPos + 1] = coords[i];
        (*tourLength)++;
    }
}

// The main function remains unchanged from the previous structure
int main(int argc, char *argv[]) {
    // ... (same as before)
    return 0;
}

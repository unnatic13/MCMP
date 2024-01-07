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

void nearestInsertion(Point *coords, int numCoords, int *tour) {
    tour[0] = 0; // Starting from the first point
    int tourLength = 1;
    int *visited = (int *)calloc(numCoords, sizeof(int));
    visited[0] = 1;

    while (tourLength < numCoords) {
        double minIncrease = __DBL_MAX__;
        int bestInsertPoint = -1;
        int bestPosition = -1;

        for (int i = 0; i < tourLength; i++) {
            for (int j = 0; j < numCoords; j++) {
                if (!visited[j]) {
                    double increase = distance(coords[tour[i]], coords[j]) + distance(coords[tour[(i + 1) % tourLength]], coords[j]) - distance(coords[tour[i]], coords[tour[(i + 1) % tourLength]]);
                    if (increase < minIncrease) {
                        minIncrease = increase;
                        bestInsertPoint = j;
                        bestPosition = i;
                    }
                }
            }
        }

        // Insert the best point into the tour
        for (int i = tourLength; i > bestPosition + 1; i--) {
            tour[i] = tour[i - 1];
        }
        tour[bestPosition + 1] = bestInsertPoint;
        visited[bestInsertPoint] = 1;
        tourLength++;
    }

    free(visited);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char *inputFile = argv[1];
    const char *outputFile = argv[2];

    FILE *file = fopen(inputFile, "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    int numCoords;
    fscanf(file, "%d", &numCoords);

    Point *coords = (Point *)malloc(numCoords * sizeof(Point));
    for (int i = 0; i < numCoords; i++) {
        fscanf(file, "%lf,%lf", &coords[i].x, &coords[i].y);
    }
    fclose(file);

    int *tour = (int *)malloc(numCoords * sizeof(int));

    nearestInsertion(coords, numCoords, tour);

    FILE *out = fopen(outputFile, "w");
    if (!out) {
        printf("Error writing to file.\n");
        return 1;
    }

    for (int i = 0; i < numCoords; i++) {
        fprintf(out, "%d\n", tour[i]);
    }
    fclose(out);

    free(tour);
    free(coords);

    return 0;
}

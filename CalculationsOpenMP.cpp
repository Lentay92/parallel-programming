#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <iomanip>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Polygon {
public:
    Polygon(int numVertices, double radius) {
        generateVertices(numVertices, radius);
    }

    double computeArea() const {
        double area = 0.0;
        int n = vertices.size();

#pragma omp parallel for reduction(+:area)
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            area += vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
        }

        return std::abs(area) / 2.0;
    }

private:
    struct Point {
        double x, y;
    };

    std::vector<Point> vertices;

    void generateVertices(int numVertices, double radius) {
        for (int i = 0; i < numVertices; ++i) {
            double angle = 2.0 * M_PI * i / numVertices;
            vertices.push_back({ radius * cos(angle), radius * sin(angle) });
        }
    }
};

int main() {
    int numVertices = 10000000;
    double radius = 10000.0;
    Polygon polygon(numVertices, radius);

    int maxThreads = omp_get_max_threads();
    std::cout << "Max available threads: " << maxThreads << std::endl;

    for (int numThreads = 1; numThreads <= maxThreads; ++numThreads) {
        omp_set_num_threads(numThreads);

        double startTime = omp_get_wtime();
        double area = polygon.computeArea();
        double endTime = omp_get_wtime();

        std::cout << "Threads: " << numThreads
            << ", Area: " << std::fixed << std::setprecision(6) << area
            << ", Time: " << (endTime - startTime) << " seconds" << std::endl;
    }

    return 0;
}

#include "Quad.h"

// Test For knearest

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 1200), "K Nearest Neighbors Visualization");

    // Create a QuadTree that covers the window area
    QuadTree quadTree(1200,1200);

    // Manually insert points into the QuadTree

    // Insert points randomly
    for (int i = 0; i < 30; i++) {
        // Generate numbers in increments of 10
        int x = (rand() % 100) * 10;
        int y = (rand() % 100) * 10;

        quadTree.insert(x, y);
    }

    // Define the reference point for the k nearest search (this point could also be inserted into the QuadTree if needed)
    double refX = 400;
    double refY = 350;
    int k = 5; // Number of nearest neighbors to find

    // Perform the k nearest neighbors search
    vector<Point*> nearestPoints = quadTree.kNearest(k, refX, refY);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        // Draw the QuadTree, highlighting the k nearest points and the reference point
        quadTree.draw(window, nearestPoints, refX, refY);

        window.display();
    }

    return 0;
}

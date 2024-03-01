#include "Quad.h"


int main() {

    // Create a QuadTree
    QuadTree quadTree(1000, 1000);

    // Insert points into the QuadTree
    quadTree.insert(100, 100);
    quadTree.insert(200, 200);
    quadTree.insert(300, 300);
    quadTree.insert(400, 400);
    quadTree.insert(500, 540);
    quadTree.insert(600, 600);
    quadTree.insert(700, 300);

    // Insert points randomly
    for (int i = 0; i < 25; i++) {
        // Generate numbers in increments of 10
        int x = (rand() % 100) * 10;
        int y = (rand() % 100) * 10;

        quadTree.insert(x, y);
    }


    // Perform a range search
    double tx = 150, ty = 200, bx = 450, by = 450;
    vector<Point*> rangePoints = quadTree.rangedSearch(tx, ty, bx, by);

    // Draw the QuadTree
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "QuadTree");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        quadTree.draw(window, rangePoints, tx, ty, bx, by);
        window.display();
    }

    return 0;
}

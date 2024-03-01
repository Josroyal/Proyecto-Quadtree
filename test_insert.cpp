// Quad.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Quad.h"

int main() {
    // Create an SFML window
    sf::RenderWindow window(sf::VideoMode(1200, 1200), "QuadTree Visualization");

    // Create a QuadTree that covers the window area
    QuadTree quadTree(1200, 1200);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Handle window close event
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Handle mouse click event
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Insert a new point into the QuadTree where the user clicks
                    quadTree.insert(event.mouseButton.x, event.mouseButton.y);
                }
            }
        }

        // Clear the window
        window.clear(sf::Color::Black);

        // Draw the QuadTree
        quadTree.draw(window);

        // Display the window contents
        window.display();
    }

    return 0;
}






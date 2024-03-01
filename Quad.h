#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
using namespace std;

// Point class to store x and y coordinates, este ser� el dato que se almacenar� en el quadtree

struct Point {
    double x;
    double y;
    Point(double x_, double y_) {
        x = x_;
        y = y_;
    }
};

class QuadTree {
private:
    Point* n = nullptr;

    QuadTree* NW = nullptr;
    QuadTree* NE = nullptr;
    QuadTree* SE = nullptr;
    QuadTree* SW = nullptr;

    Point* topLeft;
    Point* botRight;
public:

    ~QuadTree() {
        // Delete the dynamically allocated points
        delete topLeft;
        delete botRight;
        delete n;

        // Recursively delete child quadtrees
        delete NW;
        delete NE;
        delete SW;
        delete SE;
    }

    QuadTree(double bx, double by) {
        topLeft = new Point(0, 0);
        botRight = new Point(bx, by);
    }

    QuadTree(double tx, double ty, double bx, double by) {
        topLeft = new Point(tx, ty);
        botRight = new Point(bx, by);
    }

    QuadTree(double x, double y, double tx, double ty, double bx, double by) {
        n = new Point(x, y);
        topLeft = new Point(tx, ty);
        botRight = new Point(bx, by);
    }

    void insert(double x, double y);
    Point* search(double x, double y);
    void subdivide();
    void remove(double x, double y);
    vector<Point*> rangedSearch(double tx, double ty, double bx, double by);
    void rangedRemove(double tx, double ty, double bx, double by);
    vector<Point*> kNearest(int k, double x, double y);

    void draw(sf::RenderWindow& window, vector<Point*>& rangePoints, double tx, double ty, double bx, double by); // Draw for range search
    void draw(sf::RenderWindow& window, const vector<Point*>& nearestPoints, double refX, double refY); // Draw for k nearest
    void draw(sf::RenderWindow& window); // Draw for insert


};

Point* QuadTree::search(double x, double y) {
    // Check if point is in boundaries
    if (x > botRight->x ||
        x < topLeft->x ||
        y > botRight->y ||
        y < topLeft->y) {
        return nullptr;
    }

    // Check if current Tree is a point
    if (n != nullptr) {
        // Check if current point is the searched point
        if (n->x == x && n->y == y) {
            return n;
        }
    }

    // Jump to next quadrant (recursive)
    if ((topLeft->x + botRight->x) / 2 >= x) {
        if ((topLeft->y + botRight->y) / 2 >= y) {
            // North west quadrant
            if (NW == nullptr) {
                return nullptr;
            }
            else {
                return NW->search(x, y);
            }
        }
        else {
            // South West quadrant
            if (SW == nullptr) {
                return nullptr;
            }
            else {
                return SW->search(x, y);
            }
        }
    }
    else {
        if ((topLeft->y + botRight->y) / 2 >= y) {
            // North East quadrant
            if (NE == nullptr) {
                return nullptr;
            }
            else {
                return NE->search(x, y);
            }
        }
        else {
            // South East quadrant
            if (SE == nullptr) {
                return nullptr;
            }
            else {
                return SE->search(x, y);
            }
        }
    }
}


void QuadTree::insert(double x, double y) {
    // Check if point is in boundaries
    if (!(x >= topLeft->x && x <= botRight->x && y >= topLeft->y && y <= botRight->y)) {
        return; // Point is out of bounds
    }

    // If current QuadTree is a leaf node
    if (NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr) {
        if (n == nullptr) {
            n = new Point(x, y);
            return;
        }
        else {
            // Store the existing point
            Point* existingPoint = n;
            n = nullptr;

            // Subdivide the current quadrant
            subdivide();

            // Reinsert the existing point
            insert(existingPoint->x, existingPoint->y);

            // Delete the existing point
            delete existingPoint;
        }
    }

    // Insert the point into the appropriate quadrant
    if ((topLeft->x + botRight->x) / 2 >= x) {
        if ((topLeft->y + botRight->y) / 2 >= y) {
            NW->insert(x, y);
        }
        else {
            SW->insert(x, y);
        }
    }
    else {
        if ((topLeft->y + botRight->y) / 2 >= y) {
            NE->insert(x, y);
        }
        else {
            SE->insert(x, y);
        }
    }
}



// Remove a point from the QuadTree
///////////////////////////////////////////
void QuadTree::remove(double x, double y) {
    // Check if point is in boundaries
    if (x > botRight->x ||
        x < topLeft->x ||
        y > botRight->y ||
        y < topLeft->y) {
        return;
    }

    // Keep track of path from root to node to delete
    QuadTree* temp = this;
    vector<QuadTree*> visited;

    // Find node to delete
    while (true) {
        // Check if point is point to be deleted
        if (temp->n != nullptr) {
            if (temp->n->x == x && temp->n->y == y) {
                break;
            }
        }

        // Go to next quadrant, saving the path inside the vector
        visited.push_back(temp);

        double midX = (temp->topLeft->x + temp->botRight->x) / 2;
        double midY = (temp->topLeft->y + temp->botRight->y) / 2;

        if (midX >= x) {
            if (midY >= y) {
                // North west quadrant
                if (temp->NW == nullptr) {
                    return;
                }
                else {
                    temp = temp->NW;
                }
            }
            else {
                // South west quadrant
                if (temp->SW == nullptr) {
                    return;
                }
                else {
                    temp = temp->SW;
                }
            }
        }
        else {
            if (midY >= y) {
                // North east quadrant
                if (temp->NE == nullptr) {
                    return;
                }
                else {
                    temp = temp->NE;
                }
            }
            else {
                // South east quadrant
                if (temp->SE == nullptr) {
                    return;
                }
                else {
                    temp = temp->SE;
                }
            }
        }
    }

    if (!visited.empty()) {
        if (visited.back()->NW == temp) {
            visited.back()->NW = nullptr;
        }
        else if (visited.back()->NE == temp) {
            visited.back()->NE = nullptr;
        }
        else if (visited.back()->SE == temp) {
            visited.back()->SE = nullptr;
        }
        else if (visited.back()->SW == temp) {
            visited.back()->SW = nullptr;
        }
    }

    if (temp == this) {
        delete (temp->n);
        n = nullptr;
        return;
    }
    delete temp;

    for (int i = visited.size() - 1; i >= 0; i--) {
        vector<QuadTree*> count;
        if (visited[i]->NW != nullptr) {
            count.push_back(visited[i]->NW);
        }
        if (visited[i]->NE != nullptr) {
            count.push_back(visited[i]->NE);
        }
        if (visited[i]->SW != nullptr) {
            count.push_back(visited[i]->SW);
        }
        if (visited[i]->SE != nullptr) {
            count.push_back(visited[i]->SE);
        }

        if (count.size() == 1 && count[0]->n != nullptr) {
            visited[i]->n = count[0]->n;
            delete count[0];
            visited[i]->NW = nullptr;
            visited[i]->NE = nullptr;
            visited[i]->SE = nullptr;
            visited[i]->SW = nullptr;
        }
        else {
            return;
        }
    }
}

///////////////////////////////////////////

void QuadTree::subdivide() {
    // Calculate the midpoint
    double midX = (topLeft->x + botRight->x) / 2;
    double midY = (topLeft->y + botRight->y) / 2;

    // Create child quadrants if they don't exist
    if (NW == nullptr) NW = new QuadTree(topLeft->x, topLeft->y, midX, midY);
    if (NE == nullptr) NE = new QuadTree(midX, topLeft->y, botRight->x, midY);
    if (SW == nullptr) SW = new QuadTree(topLeft->x, midY, midX, botRight->y);
    if (SE == nullptr) SE = new QuadTree(midX, midY, botRight->x, botRight->y);
}

vector<Point*> QuadTree::rangedSearch(double tx, double ty, double bx, double by) {
    queue<QuadTree*> q;
    vector<Point*> sol;

    q.push(this);

    while (!q.empty()) {
        // if point is in range, add to vector
        if (q.front()->n != nullptr) {
            if (q.front()->n->x >= tx &&
                q.front()->n->x <= bx &&
                q.front()->n->y >= ty &&
                q.front()->n->y <= by) {
                sol.push_back(q.front()->n);
            }
        }

        double midX = (q.front()->topLeft->x + q.front()->botRight->x) / 2;
        double midY = (q.front()->topLeft->y + q.front()->botRight->y) / 2;

        // if any of the areas shares space with search range, add to queue
        if (tx <= midX && ty <= midY) {
            if (q.front()->NW != nullptr) {
                q.push(q.front()->NW);
            }
        }

        if (bx > midX && ty <= midY) {
            if (q.front()->NE != nullptr) {
                q.push(q.front()->NE);
            }
        }

        if (bx > midX && by > midY) {
            if (q.front()->SE != nullptr) {
                q.push(q.front()->SE);
            }
        }

        if (tx <= midX && by > midY) {
            if (q.front()->SW != nullptr) {
                q.push(q.front()->SW);
            }
        }

        q.pop();
    }

    return sol;
}

void QuadTree::rangedRemove(double tx, double ty, double bx, double by) {
    vector<Point*> points = rangedSearch(tx, ty, bx, by);
    for (auto point : points) {
        remove(point->x, point->y);
    }
}

vector<Point*> QuadTree::kNearest(int k, double x, double y) {
    vector<Point*> sol;

    // Check if 'k' is a valid number
    if (k < 1) {
        return sol;
    }

    // Check if point is in range
    if (x > botRight->x ||
        x < topLeft->x ||
        y > botRight->y ||
        y < topLeft->y) {
        return sol;
    }

    QuadTree* temp = this;

    // Search the point (or the quadrant it would go)
    while (temp->n != nullptr) {
        double midX = (temp->topLeft->x + temp->botRight->x) / 2;
        double midY = (temp->topLeft->y + temp->botRight->y) / 2;

        if (midX >= x) {
            if (midY >= y) {
                // North west quadrant
                if (temp->NW == nullptr) {
                    return sol;
                }
                else {
                    temp = temp->NW;
                }
            }
            else {
                // South west quadrant
                if (temp->SW == nullptr) {
                    return sol;
                }
                else {
                    temp = temp->SW;
                }
            }
        }
        else {
            if (midY >= y) {
                // North east quadrant
                if (temp->NE == nullptr) {
                    return sol;
                }
                else {
                    temp = temp->NE;
                }
            }
            else {
                // South east quadrant
                if (temp->SE == nullptr) {
                    return sol;
                }
                else {
                    temp = temp->SE;
                }
            }
        }
    }
    double height = (temp->topLeft->y + temp->botRight->y) / 2;
    double width = (temp->topLeft->x + temp->botRight->x) / 2;
    sol = rangedSearch(x - width, y - height, x + width, y + height);
    while (sol.size() < k) {
        height *= 2;
        width *= 2;
        sol = rangedSearch(x - width, y - height, x + width, y + height);
    }

    map<double, vector<Point*>> mp;
    set<double> st;
    double distance;
    int total = sol.size();

    for (auto point : sol) {
        distance = sqrt(pow(abs(point->x - x), 2) + pow(abs(point->y - y), 2));
        mp[distance].push_back(point);
        st.insert(distance);
    }

    sol = {};

    for (auto l : st) {
        for (auto point : mp[l]) {
            sol.push_back(point);
        }
    }

    while (sol.size() > k) {
        sol.pop_back();
    }

    return sol;
}

// Draw for insert
void QuadTree::draw(sf::RenderWindow& window) {
    // Draw boundary
    sf::RectangleShape boundary(sf::Vector2f(botRight->x - topLeft->x, botRight->y - topLeft->y));
    boundary.setPosition(topLeft->x, topLeft->y);
    boundary.setFillColor(sf::Color::Transparent);
    boundary.setOutlineThickness(1);
    boundary.setOutlineColor(sf::Color::Green);
    window.draw(boundary);

    // Draw point if exists
    if (n != nullptr) {
        sf::CircleShape point(3); // Small circle for point
        point.setPosition(n->x, n->y);
        point.setFillColor(sf::Color::Red);
        window.draw(point);
    }

    // Recursively draw child quadtrees
    if (NW != nullptr) NW->draw(window);
    if (NE != nullptr) NE->draw(window);
    if (SW != nullptr) SW->draw(window);
    if (SE != nullptr) SE->draw(window);
}

// Draw for range search
void QuadTree::draw(sf::RenderWindow& window, vector<Point*>& rangePoints, double tx, double ty, double bx, double by) {
    // Draw the boundary of the QuadTree
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(botRight->x - topLeft->x, botRight->y - topLeft->y));
    rectangle.setOutlineColor(sf::Color::White);
    rectangle.setOutlineThickness(1);
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setPosition(topLeft->x, topLeft->y);
    window.draw(rectangle);

    // Draw the point if it exists
    if (n != nullptr) {
        sf::CircleShape point(4);
        point.setFillColor(sf::Color::Red);

        // Check if the point is in the range
        if (find(rangePoints.begin(), rangePoints.end(), n) != rangePoints.end()) {
            point.setFillColor(sf::Color::Yellow);
        }

        point.setPosition(n->x, n->y);
        window.draw(point);
    }

    // Draw the range rectangle
    sf::RectangleShape rangeRectangle;
    rangeRectangle.setSize(sf::Vector2f(bx - tx, by - ty));
    rangeRectangle.setOutlineColor(sf::Color::Yellow);
    rangeRectangle.setOutlineThickness(3);
    rangeRectangle.setFillColor(sf::Color::Transparent);
    rangeRectangle.setPosition(tx, ty);
    window.draw(rangeRectangle);

    // Recursively draw the child nodes
    if (NW != nullptr) NW->draw(window, rangePoints, tx, ty, bx, by);
    if (NE != nullptr) NE->draw(window, rangePoints, tx, ty, bx, by);
    if (SW != nullptr) SW->draw(window, rangePoints, tx, ty, bx, by);
    if (SE != nullptr) SE->draw(window, rangePoints, tx, ty, bx, by);
}


// Draw for k nearest
// Draw for k nearest
void QuadTree::draw(sf::RenderWindow& window, const vector<Point*>& nearestPoints, double refX, double refY) {
    // Draw the boundary of the QuadTree
    sf::RectangleShape rectangle(sf::Vector2f(botRight->x - topLeft->x, botRight->y - topLeft->y));
    rectangle.setOutlineColor(sf::Color::White);
    rectangle.setOutlineThickness(1);
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setPosition(topLeft->x, topLeft->y);
    window.draw(rectangle);

    // Draw the reference point for k nearest search in blue
    sf::CircleShape refPoint(5);
    refPoint.setFillColor(sf::Color::Blue);
    refPoint.setPosition(refX - 3, refY - 3); // Adjust position to center the circle on the point
    window.draw(refPoint);

    // Draw all points in the QuadTree
    if (n != nullptr) {
        sf::CircleShape point(5);
        // Check if the point is one of the k nearest points
        if (std::find(nearestPoints.begin(), nearestPoints.end(), n) != nearestPoints.end()) {
            point.setFillColor(sf::Color::Yellow);
        }
        else {
            point.setFillColor(sf::Color::Red);
        }
        point.setPosition(n->x - 2, n->y - 2); // Center the circle on the point
        window.draw(point);
    }

    // Recursively draw the child nodes
    if (NW != nullptr) NW->draw(window, nearestPoints, refX, refY);
    if (NE != nullptr) NE->draw(window, nearestPoints, refX, refY);
    if (SW != nullptr) SW->draw(window, nearestPoints, refX, refY);
    if (SE != nullptr) SE->draw(window, nearestPoints, refX, refY);
}


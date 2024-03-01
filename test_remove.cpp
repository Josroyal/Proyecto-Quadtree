#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <cmath>
#include <stdlib.h>

using namespace std;

struct Point{
    double x;
    double y;
    Point(double x_, double y_){
        x = x_;
        y = y_;
    }
};

class QuadTree{
    private:
        Point *n = nullptr;

        QuadTree *NW = nullptr;
        QuadTree *NE = nullptr;
        QuadTree *SE = nullptr;
        QuadTree *SW = nullptr;

        Point *topLeft;
        Point *botRight;
    public:
        QuadTree(){}

        QuadTree(double bx, double by){
            topLeft = new Point(0, 0);
            botRight = new Point(bx, by);
        }

        QuadTree(double tx, double ty, double bx, double by){
            topLeft = new Point(tx, ty);
            botRight = new Point(bx, by);
        }

        QuadTree(double x, double y, double tx, double ty, double bx, double by){
            n = new Point(x, y);
            topLeft = new Point(tx, ty);
            botRight = new Point(bx, by);
        }

        void insert(double x, double y);

        void remove(double x, double y);

        Point* search(double x, double y);

        vector<Point*> kNearest(int k, double x, double y);

        vector<Point*> rangedSearch(double tx, double ty, double bx, double by);

        void rangedRemove(double tx, double ty, double bx, double by);

        void printTree();
};

void QuadTree::insert(double x, double y){

    // Check if point is in boundaries
    if (x > botRight->x || 
        x < topLeft->x || 
        y > botRight->y ||
        y < topLeft->y){
        return;
    }

    // Check if point already exists
    if (n != nullptr){
        if (n->x == x && n->y == y){
            return;
        }
    }

    // Check if current Tree is a leaf
    if (NE == nullptr &&
        NW == nullptr &&
        SW == nullptr &&
        SE == nullptr){
            // If current Tree is an empty leaf, insert point and end
            if (n == nullptr){
                n = new Point(x, y);
                return;
            }

            // Else, insert current tree's data in corresponding child
            else {
                if ((topLeft->x + botRight->x) / 2 >= n->x){
                    if ((topLeft->y + botRight->y) / 2 >= n->y){
                        // North west quadrant
                        NW = new QuadTree(n->x, n->y, topLeft->x, topLeft->y, (topLeft->x + botRight->x) / 2, (topLeft->x + botRight->y) / 2);
                    } else {
                        // South West quadrant
                        SW = new QuadTree(n->x, n->y, topLeft->x, (topLeft->y + botRight->y / 2), (topLeft->x + botRight->x) / 2, botRight->y);
                    }
                } else {
                    if ((topLeft->y + botRight->y) / 2 >= n->y){
                        // North East quadrant
                        NE = new QuadTree(n->x, n->y, (topLeft->x + botRight->x) / 2, topLeft->y, botRight->x, (topLeft->x + botRight->y) / 2);
                    } else {
                        // South East quadrant
                        SE = new QuadTree(n->x, n->y, (topLeft->x + botRight->x) / 2, (topLeft->y + botRight->y) / 2, botRight->x, botRight->y);
                    }
                }
                n = nullptr;
            }   
        }
    
    // Check for child to insert point
    if ((topLeft->x + botRight->x) / 2 >= x){
        if ((topLeft->y + botRight->y) / 2 >= y){
            // North west quadrant
            if (NW == nullptr){
                NW = new QuadTree(topLeft->x, topLeft->y, (topLeft->x + botRight->x) / 2, (topLeft->x + botRight->y) / 2);
            }
            NW->insert(x, y);
        } else {
            // South West quadrant
            if (SW == nullptr){
                SW = new QuadTree(topLeft->x, (topLeft->y + botRight->y / 2), (topLeft->x + botRight->x) / 2, botRight->y);
            }
            SW->insert(x, y);
        }
    } else {
        if ((topLeft->y + botRight->y) / 2 >= y){
            // North East quadrant
            if (NE == nullptr){
                NE = new QuadTree((topLeft->x + botRight->x) / 2, topLeft->y, botRight->x, (topLeft->x + botRight->y) / 2);
            }
            NE->insert(x, y);
        } else {
            // South East quadrant
            if (SE == nullptr){
                SE = new QuadTree((topLeft->x + botRight->x) / 2, (topLeft->y + botRight->y) / 2, botRight->x, botRight->y);
            }
            SE->insert(x, y);
        }
    }
}

void QuadTree::remove(double x, double y){
    // Check if point is in boundaries
    if (x > botRight->x || 
        x < topLeft->x || 
        y > botRight->y ||
        y < topLeft->y){
        return;
    }

    // Keep track of path from root to node to delete
    QuadTree *temp = this;
    vector<QuadTree*> visited;

    // Find node to delete
    while (true){
        // Check if point is point to be deleted
        if (temp->n != nullptr){
            if (temp->n->x == x && temp->n->y == y){
                break;
            }
        }

        // Go to next quadrant, saving the path inside the vector
        visited.push_back(temp);

        double midX = (temp->topLeft->x + temp->botRight->x) / 2;
        double midY = (temp->topLeft->y + temp->botRight->y) / 2;

        if (midX >= x){
            if (midY >= y){
                // North west quadrant
                if (temp->NW == nullptr){
                    return;
                } else {
                    temp = temp->NW;
                }
            } else {
                // South west quadrant
                if (temp->SW == nullptr){
                    return;
                } else {
                    temp = temp->SW;
                }
            }
        } else {
            if (midY >= y){
                // North east quadrant
                if (temp->NE == nullptr){
                    return;
                } else {
                    temp = temp->NE;
                }
            } else {
                // South east quadrant
                if (temp->SE == nullptr){
                    return;
                } else {
                    temp = temp->SE;
                }
            }
        }
    }

    if (!visited.empty()){
        if (visited.back()->NW == temp){
            visited.back()->NW = nullptr;
        } else if (visited.back()->NE == temp){
            visited.back()->NE = nullptr;
        } else if (visited.back()->SE == temp){
            visited.back()->SE = nullptr;
        } else if (visited.back()->SW == temp){
            visited.back()->SW = nullptr;
        }
    }

    if (temp == this){
        delete (temp->n);
        n = nullptr;
        return;
    }
    delete temp;

    for (int i = visited.size()-1; i >= 0; i--){
        vector<QuadTree*> count;
        if (visited[i]->NW != nullptr){
            count.push_back(visited[i]->NW);
        }
        if (visited[i]->NE != nullptr){
            count.push_back(visited[i]->NE);
        }
        if (visited[i]->SW != nullptr){
            count.push_back(visited[i]->SW);
        }
        if (visited[i]->SE != nullptr){
            count.push_back(visited[i]->SE);
        }

        if (count.size() == 1 && count[0]->n != nullptr){
            visited[i]->n = count[0]->n;
            delete count[0];
            visited[i]->NW = nullptr;
            visited[i]->NE = nullptr;
            visited[i]->SE = nullptr;
            visited[i]->SW = nullptr;
        } else {
            return;
        }
    }
}

Point* QuadTree::search(double x, double y){
    // Check if point is in boundaries
    if (x > botRight->x || 
        x < topLeft->x || 
        y > botRight->y ||
        y < topLeft->y){
        return nullptr;
    }

    // Check if current Tree is a point
    if (n != nullptr){
        // Check if current point is the searched point
        if (n->x == x && n->y == y){
            return n;
        }
    }

    // Jump to next quadrant (recursive)
    if ((topLeft->x + botRight->x) / 2 >= x){
        if ((topLeft->y + botRight->y) / 2 >= y){
            // North west quadrant
            if (NW == nullptr){
                return nullptr;
            } else {
                return NW->search(x, y);
            }
        } else {
            // South West quadrant
            if (SW == nullptr){
                return nullptr;
            } else {
                return SW->search(x, y);
            }
        }
    } else {
        if ((topLeft->y + botRight->y) / 2 >= y){
            // North East quadrant
            if (NE == nullptr){
                return nullptr;
            } else {
                return NE->search(x, y);
            }
        } else {
            // South East quadrant
            if (SE == nullptr){
                return nullptr;
            } else {
                return SE->search(x, y);
            }
        }
    }
}



vector<Point*> QuadTree::kNearest(int k, double x, double y){
    vector<Point*> sol;

    // Check if 'k' is a valid number
    if (k < 1){
        return sol;
    }

    // Check if point is in range
    if (x > botRight->x || 
        x < topLeft->x || 
        y > botRight->y ||
        y < topLeft->y){
        return sol;
    }

    QuadTree *temp = this;

    // Search the point (or the quadrant it would go)
    while (temp->n != nullptr){
        double midX = (temp->topLeft->x + temp->botRight->x) / 2;
        double midY = (temp->topLeft->y + temp->botRight->y) / 2;

        if (midX >= x){
            if (midY >= y){
                // North west quadrant
                if (temp->NW == nullptr){
                    return sol;
                } else {
                    temp = temp->NW;
                }
            } else {
                // South west quadrant
                if (temp->SW == nullptr){
                    return sol; 
                } else {
                    temp = temp->SW;
                }
            }
        } else {
            if (midY >= y){
                // North east quadrant
                if (temp->NE == nullptr){
                    return sol;
                } else {
                    temp = temp->NE;
                }
            } else {
                // South east quadrant
                if (temp->SE == nullptr){
                    return sol;
                } else {
                    temp = temp->SE;
                }
            }
        }
    }
    double height =  (temp->topLeft->y + temp->botRight->y) / 2;
    double width =  (temp->topLeft->x + temp->botRight->x) / 2;
    sol = rangedSearch(x - width, y - height, x + width, y + height);
    while (sol.size() < k){
        height *= 2;
        width *= 2;
        sol = rangedSearch(x - width, y - height, x + width, y + height);
    }

    map<double, vector<Point*>> mp;
    set<double> st;
    double distance;
    int total = sol.size();

    for (auto point : sol){
        distance = sqrt(pow(abs(point->x - x), 2) + pow(abs(point->y - y), 2));
        mp[distance].push_back(point);
        st.insert(distance);
    }

    sol = {};

    for (auto l : st){
        for (auto point : mp[l]){
            sol.push_back(point);
        }
    }

    while (sol.size() > k){
        sol.pop_back();
    }

    return sol;
}

vector<Point*> QuadTree::rangedSearch(double tx, double ty, double bx, double by){
    queue<QuadTree*> q;
    vector<Point*> sol;

    q.push(this);

    while (!q.empty()){
        // if point is in range, add to vector
        if (q.front()->n != nullptr){
            if (q.front()->n->x >= tx && 
                q.front()->n->x <= bx &&
                q.front()->n->y >= ty &&
                q.front()->n->y <= by){
                    sol.push_back(q.front()->n);
                }
        }

        double midX = (q.front()->topLeft->x + q.front()->botRight->x) / 2;
        double midY = (q.front()->topLeft->y + q.front()->botRight->y) / 2;

        // if any of the areas shares space with search range, add to queue
        if (tx <= midX && ty <= midY){
            if (q.front()->NW != nullptr){
                q.push(q.front()->NW);
            }
        } 

        if (bx > midX && ty <= midY){
            if (q.front()->NE != nullptr){
                q.push(q.front()->NE);
            }
        } 

        if (bx > midX && by > midY){
            if (q.front()->SE != nullptr){
                q.push(q.front()->SE);
            }
        } 

        if (tx <= midX && by > midY){
            if (q.front()->SW != nullptr){
                q.push(q.front()->SW);
            }
        } 

        q.pop();
    }

    return sol;
}

void QuadTree::rangedRemove(double tx, double ty, double bx, double by){
    vector<Point*> points = rangedSearch(tx, ty, bx, by);
    for (auto point : points){
        remove (point->x, point->y);
    }
}

void QuadTree::printTree(){
    vector<vector<char>> grid;
    queue<QuadTree*> bfsQ;

    vector<char> temp(botRight->y, ' ');

    for (int i = 0; i < botRight->x; i++){
        grid.push_back(temp);
    }

    bfsQ.push(this);

    while (!bfsQ.empty()){
        QuadTree* current = bfsQ.front();
        bfsQ.pop();
        if (current->n != nullptr){
            grid[current->n->x-1][current->n->y-1] = 'O';
        } else {
            bool isLeaf = true;

            if (current->NW != nullptr){
                bfsQ.push(current->NW);
                isLeaf = false;
            }

            if (current->NE != nullptr){
                bfsQ.push(current->NE);
                isLeaf = false;
            }

            if (current->SE != nullptr){
                bfsQ.push(current->SE);
                isLeaf = false;
            }

            if (current->SW != nullptr){
                bfsQ.push(current->SW);
                isLeaf = false;
            }

            if (!isLeaf){
                int midX = (current->botRight->x + current->topLeft->x) / 2;
                int midY = (current->botRight->y + current->topLeft->y) / 2;

                for (int i = current->topLeft->x; i < current->botRight->x; i++){
                    grid[i][midY] = '|';
                }

                for (int i = current->topLeft->y; i < current->botRight->y; i++){
                    grid[midX][i] = '-';
                }

                grid[midX][midY] = '+';
            }
        }
    }

    for (auto row : grid){
        for (auto dot : row){
            cout << dot << " ";
        }
        cout << endl;
    }
}

int main(){
    system("clear");
    
    QuadTree q(20, 20);
    q.insert(4, 4);
    q.insert(19, 19);
    q.insert(9, 9);
    q.insert(12, 12);
    cin.get();
    q.printTree();

    q.remove(19, 19);
    cin.get();
    system("clear");
    q.printTree();

    q.remove(12, 12);
    cin.get();
    system("clear");
    q.printTree();

    q.remove(9, 9);
    cin.get();
    system("clear");
    q.printTree();
    cin.get();
    q.remove(4,4);

    return 0;
}
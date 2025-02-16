#include <iostream>
#include <vector>
#include <deque>
#include <fstream>
#include <string>
#include <algorithm>

struct Node {
    char state;
    Node *parent;
    std::pair<int, int> action;
};

class StackFrontier {
public:
    StackFrontier() {
        frontier = new std::deque<Node*>();
        finish_state = 'B';
    }

    ~StackFrontier() {
        delete frontier;
    }

    bool contains_state() {
        for (auto &item : *frontier) {
            if (item->state == finish_state) {
                return true;
            }
        }
        return false;
    }

    bool empty() {
        return frontier->empty();
    }

    virtual Node* remove() {
        Node *temp = nullptr;
        if (empty()) {
            std::cout << "Sorry, frontier is empty" << std::endl;
        }
        else {
            temp = frontier->back();
            frontier->pop_back();
        }
        return temp;
    }

    void add(Node *new_node) {
        frontier->push_back(new_node);
    }

protected:
    std::deque<Node*> *frontier;
    char finish_state;
};

class QueueFrontier : public StackFrontier {
    Node* remove() override {
        Node *temp = nullptr;
        if (empty()) {
            std::cout << "Sorry, frontier is empty" << std::endl;
        }
        else {
            temp = frontier->front();
            frontier->pop_front();
        }
        return temp;
    }
};

class Maze {
public:
    Maze(char* filePath) {
        std::ifstream filestream(filePath);
        std::string maze_str;
        unsigned int i = 0;

        std::pair<int, int> size = maze_size(filestream);
        maze = new std::vector<std::vector<char>>(size.first, std::vector<char>(size.second, ' '));

        filestream.clear();
        filestream.seekg(0, std::ios::beg);

        while (std::getline(filestream, maze_str)) {
            for (unsigned int j = 0; j < maze_str.length(); ++j) {
                (*maze)[i][j] = maze_str[j];
                if ((*maze)[i][j] == 'A') {
                    coord = {i, j};
                }
            }
            ++i;
        }

        // frontier = new StackFrontier();
        frontier = new QueueFrontier();
        cells_cnt = 0;
        actions = {
            {1, 0},
            {0, 1},
            {-1, 0},
            {0, -1}
        };
    }

    ~Maze() {
        delete maze;
    }

    std::pair<int, int> maze_size(std::ifstream &f) {
        std::string str;
        int size_i = 0, size_j = 0;

        while (std::getline(f, str)) {
            size_i++;
        }
        size_j = str.length();

        return {size_i, size_j};
    }

    void go_to_finish() {
        int limit_up, limit_right;
        limit_up = (*maze).size();
        limit_right = (*maze)[0].size();

        Node *start = new Node();
        start->action = {coord.first, coord.second};
        start->parent = nullptr;
        start->state = 'A';

        frontier->add(start);

        while (!frontier->empty()) {
            Node *cell = frontier->remove();
            std::pair<int, int> curr_coord = cell->action;

            cells_cnt++;

            visited.push_back(curr_coord);

            for (int i = 0; i < actions.size(); ++i) {
                int new_i = curr_coord.first + actions[i].first;
                int new_j = curr_coord.second + actions[i].second;

                if ((new_i < limit_up && new_i >= 0) && (new_j < limit_right && new_j >= 0)) {
                    std::pair<int, int> new_coord = {new_i, new_j};
                    if ((*maze)[new_i][new_j] != '#' && (std::find(visited.begin(), visited.end(), new_coord)) == visited.end()) {
                        Node *nd = new Node();
                        nd->action = {new_i, new_j};
                        nd->state = (*maze)[new_i][new_j];
                        nd->parent = cell;
                        frontier->add(nd);
                    }
                }
            }

            if (frontier->contains_state()) {
                std::cout << "you reached finish\npassed cells = " << cells_cnt << std::endl;
                break;
            }

        }
    }

    void print_maze() {
        colorPath();
        for (int i = 0; i < (*maze).size(); ++i) {
            for (int j = 0; j < (*maze)[0].size(); ++j) {
                if ((*maze)[i][j] == '*')
                    std::cout << "\033[32m" << (*maze)[i][j] << "\033[0m";
                else
                    std::cout << (*maze)[i][j];
            }
            std::cout << '\n';
        }
    }

private:
    std::vector<std::vector<char>> *maze;
    std::vector<std::pair<int, int>> visited;
    StackFrontier *frontier;
    std::pair<int, int> coord;
    int cells_cnt;
    std::vector<std::pair<int, int>> actions;

    void colorPath() {
        for (int i = 0; i < visited.size(); ++i) {
            int x = visited[i].first, y = visited[i].second;
            if ((*maze)[x][y] != 'A' && (*maze)[x][y] != 'B') {
                (*maze)[x][y] = '*';
            }
        }
    }
};

int main(int argc, char **argv) {
    char *file;
    if (argc == 1) {
        std::cout << "no file path" << std::endl;
        return 1;
    }

    file = argv[1];
    Maze maze(file);

    maze.go_to_finish();
    maze.print_maze();

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"

class Point {
    public:
        Point(size_t x, size_t y) {
            this->x = x;
            this->y = y;
        }
        Point(const Point& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
        }
        std::vector<Point> get_neighbors(size_t x_max, size_t y_max) {
            std::vector<Point> neighbors;
            // left
            if (x != 0) {
                neighbors.push_back(Point(x-1, y));
            }
            // right
            if (x < x_max-1) {
                neighbors.push_back(Point(x+1, y));
            }
            // up
            if (y != 0) {
                neighbors.push_back(Point(x, y-1));
            }
            // down
            if (y < y_max-1) {
                neighbors.push_back(Point(x, y+1));
            }
            return neighbors;
        }
        bool operator==(const Point& rhs) const {
            if ((this->x != rhs.x)||(this->y != rhs.y)) {
                return false;
            } else {
                return true;
            }
        }
        Point operator+(const Point& rhs) const {
            return Point(this->x+rhs.x, this->y+rhs.y);
        }
        std::string str() const {
            std::stringstream ss;
            ss << this->x << "," << this->y;
            return ss.str();
        }
        size_t x;
        size_t y;
};

namespace std {
    template <>
    struct hash<Point> {
        size_t operator()(const Point& p) const {
            std::hash<std::pair<int,int>> pair_hasher;
            std::pair<int,int> p_p(p.x, p.y);
            return pair_hasher(p_p);
        }
    };
}

bool is_visible(const std::unordered_map<Point,bool>& visible, Point p) {
    for (const auto& el: visible)  {
        if ((el.first == p)&&el.second) {
            return true;
        }
    }
    return false;
}

void print_visible(const std::vector<std::vector<int>>& map, const std::unordered_map<Point,bool>& visible) {
    for (size_t y = 0; y < map.size(); ++y) {
        auto line = map[y];
        for(size_t x = 0; x < line.size(); ++x) {
            Point p(x, y);
            if (is_visible(visible, p)) {
                std::cout << "X";
            } else {
                std::cout << line[x];
            }
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("AOC Day 8 2022");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::vector<std::string> lines;
    std::string line_holder;

    while (std::getline(infile, line_holder)) {
        lines.push_back(line_holder);
    }

    // Read data into map
    std::vector<std::vector<int>> map;
    for (std::string line: lines) {
        std::vector<int> line_nums;
        for (auto& el: line) {
            std::stringstream ss;
            ss  << el;
            int n;
            ss >> n;
            line_nums.push_back(n);
        }
        map.push_back(line_nums);
    }
    auto get_elevation = [&map](const Point& p){
        return map[p.y][p.x];
    };

    std::cout << "Map as read in" << std::endl;
    for (const auto& line: map)  {
        for (const auto& el: line) {
            std::cout << el;
        }
        std::cout << std::endl;
    }

    // Task 1

    size_t H = lines.size();
    size_t W = lines[0].size();

    std::unordered_map<Point,bool> visible;

    // from north border
    for (size_t x=0; x < W; x++) {
        auto start_point = Point(x, 0);
        int cur_elevation = get_elevation(start_point);
        visible[start_point] = true;
        for(size_t d=1; d < H; d++) {
            Point new_point(x, d);
            int new_elevation = get_elevation(new_point);
            if (new_elevation > cur_elevation) {
                visible[new_point] = true;
                cur_elevation = new_elevation;
            }
        }
    }
    // from south border
    for (size_t x=0; x < W; x++) {
        auto start_point = Point(x, H-1);
        int cur_elevation = get_elevation(start_point);
        visible[start_point] = true;
        for(size_t d=1; d < H; d++) {
            Point new_point(x, H-1-d);
            int new_elevation = get_elevation(new_point);
            if (new_elevation > cur_elevation) {
                visible[new_point] = true;
                cur_elevation = new_elevation;
            }
        }
    }
    // from left border
    for (size_t y=0; y < H; y++) {
        auto start_point = Point(0, y);
        int cur_elevation = get_elevation(start_point);
        visible[start_point] = true;
        for(size_t d=1; d < W; d++) {
            Point new_point(d, y);
            int new_elevation = get_elevation(new_point);
            if (new_elevation > cur_elevation) {
                visible[new_point] = true;
                cur_elevation = new_elevation;
            }
        }
    }
    // from right border
    for (size_t y=0; y < H; y++) {
        auto start_point = Point(W-1, y);
        int cur_elevation = get_elevation(start_point);
        visible[start_point] = true;
        for(size_t d=1; d < W; d++) {
            Point new_point(W-1-d, y);
            int new_elevation = get_elevation(new_point);
            if (new_elevation > cur_elevation) {
                visible[new_point] = true;
                cur_elevation = new_elevation;
            }
        }
    }

    std::cout << "Visible locations" << std::endl;
    print_visible(map, visible);

    // Task 1

    size_t num_visible = 0;
    for (const auto& el: visible) {
        if (el.second) {
            num_visible += 1;
        }
    }

    std::cout << "Task 1: " << num_visible << std::endl;

    // Task 2

    auto score_point  = [&](const Point& p) {
        int init_elevation = get_elevation(p);
        // towards north
        size_t north_count = 0;
        size_t dmax = p.y;
        for (size_t d = 1; d <= dmax; d += 1) {
            Point n_p(p.x, p.y-d);
            north_count += 1;
            if (get_elevation(n_p) >= init_elevation) {
                // We've reached the end
                break;
            }
        }
        // towards south
        dmax = H-1-p.y;
        size_t south_count = 0;
        for (size_t d = 1; d <= dmax; d+= 1) {
            Point n_p(p.x, p.y+d);
            south_count += 1;
            if (get_elevation(n_p) >= init_elevation) {
                // We've reached the end
                break;
            }
        }
        // towards west
        size_t west_count = 0;
        dmax = p.x;
        for (size_t d = 1; d <= dmax; d += 1) {
            Point n_p(p.x-d, p.y);
            west_count += 1;
            if (get_elevation(n_p) >= init_elevation) {
                // We've reached the end
                break;
            }
        }
        // towards east
        size_t east_count = 0;
        dmax = W-1-p.x;
        for (size_t d = 1; d <= dmax; d += 1) {
            Point n_p(p.x+d, p.y);
            east_count += 1;
            if (get_elevation(n_p) >= init_elevation) {
                // We've reached the end
                break;
            }
        }
        return north_count*south_count*west_count*east_count;
    };

    size_t max_score = 0;
    for(size_t x = 0; x < W; x++) {
        for(size_t y = 0; y < H; y++) {
            Point p(x, y);
            size_t score = score_point(p);
            if (score > max_score) {
                max_score = score;
            }
        }
    }

    std::cout << "Task 2: " << max_score << std::endl;

	return 0;
}

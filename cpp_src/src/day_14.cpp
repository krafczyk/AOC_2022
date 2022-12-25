#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef int32_t val_t;

enum class Tile { Wall, Sand };

class Point {
    public:
        Point(val_t x, val_t y) {
            this->x = x;
            this->y = y;
        }
        Point(const Point& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
        }
        Point& operator=(const Point& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            return *this;
        }
        bool operator==(const Point& rhs) const {
            if ((this->x == rhs.x) && (this->y == rhs.y)) {
                return true;
            } else {
                return false;
            }
        }
        bool operator!=(const Point& rhs) const {
            return !((*this)==rhs);
        }
        Point operator-(const Point& rhs) const {
            return Point(this->x-rhs.x, this->y-rhs.y);
        }
        Point operator+(const Point& rhs) const {
            return Point(this->x+rhs.x, this->y+rhs.y);
        }
        std::string str() const {
            std::stringstream ss;
            ss << this->x << "," << this->y;
            return ss.str();
        }
        val_t x;
        val_t y;
};

// Define hash for Point so we can use maps and sets with it
namespace std {
    template<>
    struct hash<Point> {
        size_t operator()(const Point& p) const {
            std::hash<std::pair<val_t, val_t>> hasher;
            return hasher(std::pair<val_t, val_t>(p.x, p.y));
        }
    };
}

void show_line(const std::vector<Point>& line) {
    bool first = true;
    for (const auto& p: line) {
        if (first) {
            first = false;
        } else {
            std::cout << " -> ";
        }
        std::cout << p.str();
    }
    std::cout << std::endl;
}

typedef std::unordered_map<Point,Tile> map_t;

void init_map(map_t& map, const std::vector<std::vector<Point>>& point_lines) {
    for (const auto& point_line: point_lines) {
        for (size_t i = 1; i < point_line.size(); ++i) {
            Point first = point_line[i-1];
            Point last = point_line[i];
            Point diff = last-first;
            if (diff.x != 0) {
                diff.x = diff.x/std::abs(diff.x);
            }
            if (diff.y != 0) {
                diff.y = diff.y/std::abs(diff.y);
            }
            Point current = first;
            bool first_loop = true;
            do {
                if (!first_loop) {
                    current = current+diff;
                } else {
                    first_loop = false;
                }
                map[current] = Tile::Wall;
            } while (current != last);
        }
    }
}

void show_map(const map_t& map) {
    // Compute range of map
    bool first = true;
    val_t min_x = 0;
    val_t max_x = 0;
    val_t min_y = 0;
    val_t max_y = 0;
    for (const auto& el: map) {
        const auto& p = el.first;
        if (first) {
            min_x = max_x = p.x;
            min_y = max_y = p.y;
            first = false;
        }
        if (p.x < min_x) {
            min_x = p.x;
        } else if (p.x > max_x) {
            max_x = p.x;
        }
        if (p.y < min_y) {
            min_y = p.y;
        } else if (p.y > max_y) {
            max_y = p.y;
        }
    }
    min_x -= 1;
    max_x += 1;

    min_y = 0;
    for (val_t y = min_y; y <= max_y; ++y) {
        for (val_t x = min_x; x <= max_x; ++x) {
            Point p(x, y);
            if (p == Point(500, 0)) {
                std::cout << "+";
            } else if (map.contains(p)) {
                const auto& val = map.at(p);
                if (val == Tile::Wall) {
                    std::cout << "#";
                } else if (val == Tile::Sand) {
                    std::cout << "o";
                } else {
                    throw std::runtime_error("Unexpected tile");
                }
            } else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}

map_t copy_map(const map_t& map) {
    map_t copy;
    for (const auto& el: map) {
        copy[el.first] = el.second;
    }
    return copy;
}

bool add_sand_1(map_t& map, val_t max_y) {
    Point sand(500, 0);
    std::vector<Point> directions = {
        Point(0, 1),
        Point(-1,1),
        Point(1,1) };
    bool added_sand = false;
    while (true) {
        if (sand.y > max_y) {
            // Sand falls forever.
            break;
        }
        bool advanced = false;
        for (const auto& direction: directions) {
            Point proj_sand = sand+direction;
            if (!map.contains(proj_sand)) {
                sand = proj_sand;
                advanced = true;
                break;
            }
        }
        if (!advanced) {
            // Sand comes to rest
            map[sand] = Tile::Sand;
            added_sand = true;
            break;
        }
    }
    return added_sand;
}

void solve_task_1(const map_t& in_map) {
    map_t map = copy_map(in_map);

    // Get max y.
    val_t max_y = 0;
    bool first = true;
    for(const auto& el: map) {
        const auto& p = el.first;
        if (first) {
            max_y = p.y;
            first = false;
        }
        if (p.y > max_y) {
            max_y = p.y;
        }
    }

    while (add_sand_1(map, max_y)) {
    }

    size_t num_sand = 0;
    for (const auto& el: map) {
        const auto& type = el.second;
        if (type == Tile::Sand) {
            num_sand += 1;
        }
    }

    std::cout << "Task 1: " << num_sand << std::endl;
    show_map(map);
}

bool add_sand_2(map_t& map, val_t floor_y) {
    Point sand(500, 0);
    if (map.contains(sand)) {
        // sand entry point is now blocked
        return false;
    }
    std::vector<Point> directions = {
        Point(0, 1),
        Point(-1,1),
        Point(1,1) };
    bool added_sand = false;
    while (true) {
        bool advanced = false;
        for (const auto& direction: directions) {
            Point proj_sand = sand+direction;
            if ((!map.contains(proj_sand))&&(proj_sand.y < floor_y)) {
                sand = proj_sand;
                advanced = true;
                break;
            }
        }
        if (!advanced) {
            // Sand comes to rest
            map[sand] = Tile::Sand;
            added_sand = true;
            break;
        }
    }
    return added_sand;
}

void solve_task_2(const map_t& in_map) {
    map_t map = copy_map(in_map);

    // Get max y.
    val_t max_y = 0;
    bool first = true;
    for(const auto& el: map) {
        const auto& p = el.first;
        if (first) {
            max_y = p.y;
            first = false;
        }
        if (p.y > max_y) {
            max_y = p.y;
        }
    }

    val_t floor_y = max_y+2;

    while (add_sand_2(map, floor_y)) {
    }

    size_t num_sand = 0;
    for (const auto& el: map) {
        const auto& type = el.second;
        if (type == Tile::Sand) {
            num_sand += 1;
        }
    }

    std::cout << "Task 2: " << num_sand << std::endl;

    // Get min/max x. and add wall tiles for floor for proper map
    val_t min_x = 0;
    val_t max_x = 0;
    first = true;
    for(const auto& el: map) {
        const auto& p = el.first;
        if (first) {
            min_x = p.x;
            min_x = p.x;
            first = false;
        }
        if (p.x < min_x) {
            min_x = p.x;
        }
        if (p.x > max_x) {
            max_x = p.x;
        }
    }

    for (val_t x = min_x-2; x <= max_x+2; ++x) {
        Point p(x, floor_y);
        map[p] = Tile::Wall;
    }

    show_map(map);
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("AOC Day 14 2022");
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
    std::string line;

    std::regex coord_re("([-\\d]+),([-\\d]+)");

    std::vector<std::vector<Point>> lines;
    while (std::getline(infile, line)) {
        std::vector<Point> point_line;
        std::smatch m;
        while (std::regex_search(line, m, coord_re)) {
            val_t x;
            val_t y;
            std::stringstream ss;
            ss << m[1];
            ss >> x;
            ss.clear();
            ss << m[2];
            ss >> y;
            point_line.push_back(Point(x, y));
            line = m.suffix().str();
        }
        lines.push_back(point_line);
    }

    map_t map;
    init_map(map, lines);

    solve_task_1(map);

    solve_task_2(map);

	return 0;
}

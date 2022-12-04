#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include <stdexcept>
#include "ArgParseStandalone.h"
#include "utilities.h"

template<typename T>
bool contains(const std::pair<T,T>& a, T el) {
    if ((el >= a.first)&&(el <= a.second)) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool a_contains_b(const std::pair<T,T>& a, const std::pair<T,T>& b) {
    if (contains(a, b.first)&&contains(a, b.second)) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool a_intersects_b(const std::pair<T,T>& a, const std::pair<T,T>& b) {
    if (contains(a, b.first)||contains(a, b.second)) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
class elf_pair {
    public:
        elf_pair(T a_l, T a_h, T b_l, T b_h) {
            a = std::pair<T,T>(a_l, a_h);
            b = std::pair<T,T>(b_l, b_h);
        }
        void show() const {
            std::cout << this->a.first << "-" << this->a.second;
            std::cout << "," << this->b.first << "-" << this->b.second;
            std::cout << std::endl;
        }
        std::pair<T,T> a;
        std::pair<T,T> b;
};

template<typename T>
elf_pair<T> parse_line(const std::string& line) {
    std::regex line_rgx("(\\d*)\\-(\\d*)\\,(\\d*)\\-(\\d*)");

    std::smatch matches;
    std::regex_match(line, matches, line_rgx);
    if (matches.size() != 5) {
        std::stringstream message;
        message << "Line didn't match regex! Expected 5, got " << matches.size();
        throw std::runtime_error(message.str());
    }
    T a_l;
    T a_h;
    T b_l;
    T b_h;
    std::stringstream ss;
    ss << matches[1];
    ss >> a_l;
    ss.clear();
    ss << matches[2];
    ss >> a_h;
    ss.clear();
    ss << matches[3];
    ss >> b_l;
    ss.clear();
    ss << matches[4];
    ss >> b_h;
    ss.clear();
    return elf_pair<T>(a_l, a_h, b_l, b_h);
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("AOC Day 4 2022");
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
    std::vector<elf_pair<int>> pairs;
    std::string line_holder;

    while (std::getline(infile, line_holder)) {
        auto the_pair = parse_line<int>(line_holder);
        pairs.push_back(the_pair);
    }

    size_t num1 = 0;
    for (const auto& e_pair : pairs) {
        if (a_contains_b(e_pair.a, e_pair.b)||a_contains_b(e_pair.b, e_pair.a)) {
            num1 += 1;
        }
    }
    std::cout << "Task 1: " << num1 << std::endl;

    size_t num2 = 0;
    for (const auto& e_pair : pairs) {
        if (a_intersects_b(e_pair.a, e_pair.b)||a_intersects_b(e_pair.b, e_pair.a)) {
            num2 += 1;
        }
    }

    std::cout << "Task 2: " << num2 << std::endl;

	return 0;
}

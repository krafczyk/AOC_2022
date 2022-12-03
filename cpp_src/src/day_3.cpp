#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <typeinfo>
#include "ArgParseStandalone.h"
#include "utilities.h"

std::unordered_map<char, uint> count_items(const std::string& list) {
    std::unordered_map<char, uint> count;
    for (char c: list) {
        try {
            count[c] = count.at(c)+1;
        } catch (std::out_of_range&) {
            count[c] = 1;
        }
    }
    return count;
}

std::vector<char> unique_items(const std::unordered_map<char, uint>& item_count) {
    std::vector<char> unique;
    for (const auto& p : item_count) {
        unique.push_back(p.first);
    }
    return unique;
}

template<typename T>
void sort_vec(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end());
}

void show(const std::vector<char>& v) {
    for (auto el : v) {
        std::cout << el;
    }
    std::cout << std::endl;
}

char find_misplaced_item(const std::string& line) {
    size_t pocket_size = line.size()/2;
    auto pocket_1 = count_items(line.substr(0, pocket_size));
    auto pocket_1_unique = unique_items(pocket_1);
    sort_vec<char>(pocket_1_unique);
    auto pocket_2 = count_items(line.substr(pocket_size, pocket_size));
    auto pocket_2_unique = unique_items(pocket_2);
    sort_vec<char>(pocket_2_unique);
    std::vector<char> intersection;
    std::set_intersection(pocket_1_unique.begin(), pocket_1_unique.end(), pocket_2_unique.begin(), pocket_2_unique.end(), std::back_inserter(intersection));
    return intersection[0];
}

char find_common_item(const std::string& sack1, const std::string& sack2, const std::string& sack3) {
    // Get unique item list from each sack
    auto sack_1 = count_items(sack1);
    auto sack_1_unique = unique_items(sack_1);
    sort_vec<char>(sack_1_unique);

    auto sack_2 = count_items(sack2);
    auto sack_2_unique = unique_items(sack_2);
    sort_vec<char>(sack_2_unique);

    auto sack_3 = count_items(sack3);
    auto sack_3_unique = unique_items(sack_3);
    sort_vec<char>(sack_3_unique);

    // Get intersection of all three sacks
    std::vector<char> intersection1;
    std::set_intersection(sack_1_unique.begin(), sack_1_unique.end(), sack_2_unique.begin(), sack_2_unique.end(), std::back_inserter(intersection1));
    std::vector<char> final_intersection;
    std::set_intersection(intersection1.begin(), intersection1.end(), sack_3_unique.begin(), sack_3_unique.end(), std::back_inserter(final_intersection));

    return final_intersection[0];
}

int item_score(char item) {
    if ((item >= 'a') && (item <= 'z')) {
        return 1+(item-'a');
    }
    if ((item >= 'A') && (item <= 'Z')) {
        return 27+(item-'A');
    }
    return -1;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("AOC Day 3 2022");
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

    std::string line_holder;
    //std::unordered_map<char, uint> 
    std::vector<std::string> rucksacks;
    while (std::getline(infile, line_holder)) {
        rucksacks.push_back(line_holder);
    }

    int total_score = 0;
    for (const auto& rucksack : rucksacks) {
        total_score += item_score(find_misplaced_item(rucksack));
    }

    std::cout << "Task 1: " << total_score << std::endl;

    total_score = 0;
    for (size_t i = 0; i < rucksacks.size(); i += 3) {
        total_score += item_score(find_common_item(rucksacks[i], rucksacks[i+1], rucksacks[i+2]));
    }

    std::cout << "Task 2: " << total_score << std::endl;

	return 0;
}

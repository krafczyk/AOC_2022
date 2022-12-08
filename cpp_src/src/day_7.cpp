#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <vector>
#include <memory>
#include "ArgParseStandalone.h"
#include "utilities.h"

class Node {
    public:
        Node(std::shared_ptr<Node> parent, std::string name) {
            this->parent = parent;
            this->name = name;
        }
        virtual std::string type() const = 0;
        std::string name;
        std::shared_ptr<Node> parent;
};

class Directory: public Node {
    public:
        Directory(std::shared_ptr<Node> parent, std::string name): Node(parent, name) {
        }
        void add_child(std::shared_ptr<Node> child) {
            this->children.push_back(child);
        }
        std::shared_ptr<Node> get_node(std::string name) {
            for (auto& el: this->children) {
                if (el->name == name) {
                    return el;
                }
            }
            return nullptr;
        }
        virtual std::string type() const {
            return "dir";
        }
        std::vector<std::shared_ptr<Node>> children;
};

class File: public Node {
    public:
        File(std::shared_ptr<Node> parent, std::string name, size_t size): Node(parent, name) {
            this->size = size;
        }
        virtual std::string type() const {
            return "file";
        }
        size_t size;
};

void print_tabs(size_t num_tabs, const std::string& tab) {
    for (size_t i = 0; i < num_tabs; ++i) {
        std::cout << tab;
    }
}

void print_directory_imp(const std::shared_ptr<Directory> dir, size_t num_tabs, const std::string& tab) {
    print_tabs(num_tabs, tab);
    std::cout << dir->name << std::endl;
    for (const auto& child: dir->children) {
        if (child->type() == "dir") {
            print_directory_imp(std::dynamic_pointer_cast<Directory>(child), num_tabs+1, tab);
        } else {
            std::shared_ptr<File> file = std::dynamic_pointer_cast<File>(child);
            print_tabs(num_tabs+1, tab);
            std::cout << file->name << " " << file->size << std::endl;
        }
    }
}

void print_directory(const std::shared_ptr<Directory> dir) {
    print_directory_imp(dir, 0, "  ");
}

template<typename Out>
using FileFunc = Out(*)(std::shared_ptr<File>);

template<typename Out>
using DirFunc = Out(*)(std::shared_ptr<Directory>);

template<typename Out>
std::vector<Out> ApplyToFiles(const std::shared_ptr<Directory> dir, FileFunc<Out> f) {
    std::vector<Out> results;
    for (const auto& node: dir->children) {
        if (node->type() == "file") {
            results.push_back(f(std::dynamic_pointer_cast<File>(node)));
        } else {
            auto dir_results = ApplyToFiles(std::dynamic_pointer_cast<Directory>(node), f);
            results.insert(results.end(), dir_results.begin(), dir_results.end());
        }
    }
    return results;
}

template<typename Out>
std::vector<Out> ApplyToDirectories(const std::shared_ptr<Directory> dir, DirFunc<Out> f) {
    std::vector<Out> results;
    for (const auto& node: dir->children) {
        if (node->type() == "dir") {
            auto child_dir = std::dynamic_pointer_cast<Directory>(node);
            auto dir_results = ApplyToDirectories(child_dir, f);
            results.insert(results.end(), dir_results.begin(), dir_results.end());
        }
    }
    results.push_back(f(dir));
    return results;
}

size_t get_file_size(std::shared_ptr<File> file) {
    return file->size;
}

size_t get_directory_size(std::shared_ptr<Directory> dir) {
    std::vector<size_t> file_sizes = ApplyToFiles(dir, get_file_size);
    size_t total = 0;
    for (size_t s: file_sizes) {
        total += s;
    }
    return total;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("AOC Day 7 2022");
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

    std::vector<std::vector<std::string>> lines;
    std::string line_holder;
    std::shared_ptr<Directory> cur_dir = nullptr;
    std::shared_ptr<Directory> root_dir = nullptr;
    while (std::getline(infile, line_holder)) {
        auto words = split(line_holder, ' ');
        lines.push_back(words);
        if (words[0] == "$") {
            // We have a command
            if (words[1] == "cd") {
                // We have a cd command
                if (cur_dir == nullptr) {
                    cur_dir = std::make_shared<Directory>(nullptr, "/");
                    root_dir = cur_dir;
                } else {
                    auto dir_name = words[2];
                    if (dir_name == "..") {
                        // Go up to the parent of this directory
                        cur_dir = std::dynamic_pointer_cast<Directory>(cur_dir->parent);
                    } else {
                        // Look for the directory in the current directory
                        std::shared_ptr<Node> dir = cur_dir->get_node(dir_name);
                        if (dir == nullptr) {
                            std::cout << "Available directories" << std::endl;
                            for (const auto& node: cur_dir->children) {
                                std::cout << node->name << std::endl;
                            }
                            std::stringstream ss;
                            ss << "Couldn't find needed directory! " << dir_name;
                            throw std::runtime_error(ss.str());
                        } else {
                            // Change directory to the directory object
                            cur_dir = std::dynamic_pointer_cast<Directory>(dir);
                        }
                    }
                }
            } else {
                // We have an ls command
                // We can ignore this for now.
                continue;
            }
        } else {
            // We have an ls entry
            auto node_name = words[1];
            // Check if the node exists already
            if (cur_dir->get_node(node_name) != nullptr) {
                continue;
            }
            if (words[0] == "dir") {
                // We have another directory
                auto new_dir = std::make_shared<Directory>(std::dynamic_pointer_cast<Node>(cur_dir), node_name);
                // Add it to the current directory
                cur_dir->add_child(new_dir);
            } else {
                // We have a file
                std::stringstream ss(words[0]);
                size_t size = 0;
                ss >> size;
                auto new_file = std::make_shared<File>(std::dynamic_pointer_cast<Node>(cur_dir), node_name, size);
                // Add it to the current directory
                cur_dir->add_child(new_file);
            }
        }
    }

    std::cout << "Directory structure as read" << std::endl;
    print_directory(root_dir);

    // Solve Task 1

    std::vector<size_t> dir_sizes = ApplyToDirectories(root_dir, get_directory_size);

    size_t task1_total = 0;
    for (size_t s: dir_sizes) {
        if (s <= 100000) {
            task1_total += s;
        }
    }

    std::cout << std::endl;

    std::cout << "Task 1: " << task1_total << std::endl;

    size_t total_disk_size = 70000000;
    size_t needed_free_space = 30000000;

    // Compute total disk usage
    size_t disk_used = get_directory_size(root_dir);

    size_t disk_available = total_disk_size-disk_used;
    size_t disk_needed = needed_free_space-disk_available;

    // List all directory sizes
    std::vector<size_t> dir_sizes_filtered;
    std::copy_if(dir_sizes.begin(), dir_sizes.end(), std::back_inserter(dir_sizes_filtered), [&](size_t s) { return s > disk_needed; });
    std::sort(dir_sizes_filtered.begin(), dir_sizes_filtered.end());

    std::cout << "Task 2: " << dir_sizes_filtered[0] << std::endl;

	return 0;
}

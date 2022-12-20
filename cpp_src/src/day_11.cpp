#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include <deque>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef size_t worry_t;

size_t world_item_id = 0;

class Item {
    public:
        Item(worry_t val) {
            this->current_worry = val;
            this->id = world_item_id;
            world_item_id += 1;
        }
        worry_t current_worry;
        std::vector<size_t> history;
        size_t id;
};

class Monkey;

typedef worry_t (*monkey_fun_t)(std::shared_ptr<Monkey>,worry_t);

class Monkey {
    public:
        Monkey() {
            this->method = nullptr;
            this->divisor = 0;
            this->val = (uint32_t) 0;
            this->true_dest = 0;
            this->false_dest = 0;
        }
        Monkey(const Monkey& monkey) {
            this->method = monkey.method;
            this->divisor = monkey.divisor;
            this->val = monkey.val;
            this->true_dest = monkey.true_dest;
            this->false_dest = monkey.false_dest;
            for (auto v: monkey.items) {
                this->items.push_back(std::make_shared<Item>(*v));
            }
        }
        std::string show(bool history) const {
            std::stringstream ss;
            ss << "Monkey| items: ";
            for (const auto& it: this->items) {
                if (!history) {
                    ss << it->current_worry << ", ";
                } else {
                    ss << "(" << it->id << ")" << it->current_worry << "|";
                    for (auto h: it->history) {
                        ss << h << "|";
                    }
                    ss << ", ";
                }
            }
            ss << " val: " << this->val;
            ss << " divisor: " << this->divisor;
            ss << " dest: (" << this->true_dest << "," << this->false_dest << ")";
            return ss.str();
        }
        std::string show_items() const {
            std::stringstream ss;
            bool first = true;
            for (const auto& it: this->items) {
                if (!first) {
                    ss << ", ";
                } else {
                    first = false;
                }
                ss << it;
            }
            return ss.str();
        }
        size_t dest(std::shared_ptr<Item> item) const {
            if ((item->current_worry % this->divisor) == 0) {
                return this->true_dest;
            } else {
                return this->false_dest;
            }
        }
        std::deque<std::shared_ptr<Item>> items;
        monkey_fun_t method;
        worry_t val;
        worry_t divisor;
        size_t true_dest;
        size_t false_dest;
};

worry_t add(std::shared_ptr<Monkey> monkey, worry_t old) {
    return old+monkey->val;
}

worry_t mul(std::shared_ptr<Monkey> monkey, worry_t old) {
    return old*monkey->val;
}

worry_t square(std::shared_ptr<Monkey> monkey, worry_t old) {
    // 'use' monkey variable to silence unused warning.
    (void)monkey;
    return old*old;
}

worry_t doub(std::shared_ptr<Monkey> monkey, worry_t old) {
    // 'use' monkey variable to silence unused warning.
    (void)monkey;
    return old+old;
}

std::vector<size_t> do_round_1(std::vector<std::shared_ptr<Monkey>>& monkeys) {
    std::vector<size_t> items_inspected;
    for (auto& monkey: monkeys) {
        size_t inspected = 0;
        while(monkey->items.size() > 0) {
            // Retrieve first item
            auto item = monkey->items.front();
            monkey->items.pop_front();
            // Worry computation
            worry_t new_worry = monkey->method(monkey, item->current_worry);
            new_worry = new_worry / 3;
            item->current_worry = new_worry;
            size_t dest = monkey->dest(item);
            item->history.push_back(dest);
            monkeys[dest]->items.push_back(item);
            inspected += 1;
        }
        items_inspected.push_back(inspected);
    }
    return items_inspected;
}

std::vector<size_t> do_round_2(std::vector<std::shared_ptr<Monkey>>& monkeys, worry_t divisor_product) {
    std::vector<size_t> items_inspected;
    for (auto& monkey: monkeys) {
        size_t inspected = 0;
        while(monkey->items.size() > 0) {
            // Retrieve first item
            auto item = monkey->items.front();
            monkey->items.pop_front();
            // Worry computation
            worry_t new_worry = monkey->method(monkey, item->current_worry);
            // 'Control' size by getting the remainder against the divisor product
            item->current_worry = new_worry % divisor_product;
            size_t dest = monkey->dest(item);
            item->history.push_back(dest);
            monkeys[dest]->items.push_back(item);
            inspected += 1;
        }
        items_inspected.push_back(inspected);
    }
    return items_inspected;
}

void report_items(const auto& monkeys) {
    size_t i = 0;
    for (const auto& monkey: monkeys) {
        std::cout << i << ": " << monkey->show_items() << std::endl;
        i += 1;
    }
};

void task_1(const std::vector<std::shared_ptr<Monkey>>& in_monkeys) {
    // Copy monkeys
    std::vector<std::shared_ptr<Monkey>> monkeys;
    for(const auto& monkey: in_monkeys) {
        monkeys.push_back(std::make_shared<Monkey>(*monkey));
    }

    size_t round = 0;
    std::vector<size_t> total_item_inspections;
    for (auto v:monkeys) {
        total_item_inspections.push_back(0);
    }
    size_t num_rounds = 20;
    for (; round < num_rounds; round++) {
        auto item_inspections = do_round_1(monkeys);
        for (size_t i=0; i < total_item_inspections.size(); ++i) {
            total_item_inspections[i] += item_inspections[i];
        }
    }

    std::sort(total_item_inspections.begin(), total_item_inspections.end());
    std::reverse(total_item_inspections.begin(), total_item_inspections.end());

    size_t task1_monkey_business = total_item_inspections[0]*total_item_inspections[1];
    std::cout << "Task 1: " << task1_monkey_business << std::endl;
}

void task_2(const std::vector<std::shared_ptr<Monkey>>& in_monkeys) {
    // Copy monkeys
    std::vector<std::shared_ptr<Monkey>> monkeys;
    worry_t divisor_product = 1;
    for(const auto& monkey: in_monkeys) {
        divisor_product *= monkey->divisor;
        monkeys.push_back(std::make_shared<Monkey>(*monkey));
    }

    size_t round = 0;
    std::vector<size_t> total_item_inspections;
    for (auto v:monkeys) {
        total_item_inspections.push_back(0);
    }
    size_t num_rounds = 10000;
    for (; round < num_rounds; round++) {
        auto item_inspections = do_round_2(monkeys, divisor_product);

        for (size_t i=0; i < total_item_inspections.size(); ++i) {
            total_item_inspections[i] += item_inspections[i];
        }
    }

    for(size_t i = 0; i < total_item_inspections.size(); ++i) {
        std::cout << i << ": " << total_item_inspections[i] << std::endl;
    }

    std::sort(total_item_inspections.begin(), total_item_inspections.end());
    std::reverse(total_item_inspections.begin(), total_item_inspections.end());

    size_t task1_monkey_business = total_item_inspections[0]*total_item_inspections[1];
    std::cout << "Task 2: " << task1_monkey_business << std::endl;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("AOC Day 11 2022");
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

    std::regex number_re("(\\d+)");
    std::regex equation_re("new = old ([\\*\\+]) ([^\\s]+)");
    std::regex divisible_re("divisible by ([^\\s]+)");
    std::regex dest_re("monkey ([^\\s]+)");

    std::vector<std::shared_ptr<Monkey>> monkeys;
    size_t current_monkey_idx = 0;
    auto add_monkey = [&](auto monkey) {
        monkeys.push_back(monkey);
        current_monkey_idx += 1;
    };
    std::shared_ptr<Monkey> current_monkey = nullptr;
    while(std::getline(infile, line_holder)) {
        if (line_holder.find("Monkey") != std::string::npos) {
            if (current_monkey != nullptr) {
                add_monkey(current_monkey);
            }
            current_monkey = std::make_shared<Monkey>();
            // Initialize variables for the monkey.
            continue;
        }
        if (line_holder.find("Starting items") != std::string::npos) {
            std::smatch m;
            while (std::regex_search(line_holder, m, number_re)) {
                // Put worry value in monkey's deque
                std::stringstream ss;
                worry_t worry_val;
                ss << m[1];
                ss >> worry_val;
                auto new_item = std::make_shared<Item>(worry_val);
                new_item->history.push_back(current_monkey_idx);
                current_monkey->items.push_back(new_item);
                // Advance to next bit of line
                line_holder = m.suffix().str();
            }
            continue;
        }
        if (line_holder.find("Operation") != std::string::npos) {
            std::smatch m;
            std::regex_search(line_holder, m, equation_re);
            std::string op = m[1];
            std::string val = m[2];
            if (val == "old") {
                if (op == "+") {
                    current_monkey->method = doub;
                } else if (op == "*") {
                    current_monkey->method = square;
                } else {
                    throw std::runtime_error("Unexpected operation 1");
                }
            } else {
                worry_t w_val;
                std::stringstream ss;
                ss << val;
                ss >> w_val;
                current_monkey->val = w_val;
                if (op == "+") {
                    current_monkey->method = add;
                } else if (op == "*") {
                    current_monkey->method = mul;
                } else {
                    throw std::runtime_error("Unexpected operation 2");
                }
            }
            continue;
        }
        if (line_holder.find("Test") != std::string::npos) {
            std::smatch m;
            std::regex_search(line_holder, m, divisible_re);
            std::stringstream ss;
            worry_t divisor;
            ss << m[1];
            ss >> divisor;
            current_monkey->divisor = divisor;
            continue;
        }
        if (line_holder.find("If true") != std::string::npos) {
            std::smatch m;
            std::regex_search(line_holder, m, dest_re);
            std::stringstream ss;
            size_t dest;
            ss << m[1];
            ss >> dest;
            current_monkey->true_dest = dest;
            continue;
        }
        if (line_holder.find("If false") != std::string::npos) {
            std::smatch m;
            std::regex_search(line_holder, m, dest_re);
            std::stringstream ss;
            size_t dest;
            ss << m[1];
            ss >> dest;
            current_monkey->false_dest = dest;
            continue;
        }
    }
    if (current_monkey != nullptr) {
        add_monkey(current_monkey);
    }

    std::cout << "Monkeys retrieved" << std::endl;
    for (const auto& monkey: monkeys) {
        std::cout << monkey->show(true) << std::endl;
    }

    task_1(monkeys);

    task_2(monkeys);

	return 0;
}

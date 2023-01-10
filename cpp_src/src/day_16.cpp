#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include <ranges>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef size_t valve_t;
typedef std::map<std::string,std::vector<std::string>> room_map_t;
typedef std::map<std::string,valve_t> rate_map_t;
typedef std::map<std::pair<std::string,std::string>,valve_t> dist_map_t;

void render_rate_map(const rate_map_t& rate_map) {
    for (const auto& map_el: rate_map) {
        std::cout << map_el.first << ": " << map_el.second << std::endl;
    }
}


std::map<std::string,valve_t> shortest_distances(const std::string& orig, const std::vector<std::string>& dests, const room_map_t& room_map) {
    // Compute the shortest distances from the origin to a collection of destinations
    std::map<std::string,valve_t> best_dists;
    std::vector<std::string> to_visit;
    auto visit_inserter = std::back_inserter(to_visit);

    auto visit_room = [&](const std::string& name) {
        valve_t n_dist = best_dists.at(name)+1;
        for (const auto& neighbor: room_map.at(name)) {
            if (!best_dists.contains(neighbor)) {
                // neighbor isn't in the map yet
                best_dists[neighbor] = n_dist;
                if (!hasElement(to_visit, neighbor)) {
                    // Add the neighbor to the visit list if not there yet.
                    *visit_inserter = neighbor;
                }
            } else {
                if (best_dists.at(neighbor) < n_dist) {
                    // We need to revisit this point since we found a better distance
                    best_dists[neighbor] = n_dist;
                    if (!hasElement(to_visit, neighbor)) {
                        *visit_inserter = neighbor;
                    }
                }
                
            }
        }
    };

    // We start at the origin, distance is zero.
    best_dists[orig] = 0;
    visit_room(orig);

    while(true) {
        // First, check whether we have any more points to visit
        if (to_visit.size() == 0) {
            // We're done, break
            break;
        }
        // Next, check whether some destinations haven't been found
        bool missing_dests = false;
        for (const auto& dest: dests) {
            if (!best_dists.contains(dest)) {
                missing_dests = true;
            }
        }
        if (!missing_dests) {
            // Just because we have encountered all destinations doesn't mean we're done.
            // We need to check that we CAN'T do any better.
            // First, let's get the farthest destination
            valve_t max_dist = 0;
            for (const auto& dest: dests) {
                valve_t t_dist = best_dists.at(dest);
                if (t_dist > max_dist) {
                    max_dist = t_dist;
                }
            }
            bool cant_do_better = true;
            for (const auto& visit: to_visit) {
                valve_t v_dist = best_dists.at(visit);
                if (v_dist+1 <= max_dist) {
                    cant_do_better = false;
                }
            }
            if (cant_do_better) {
                break;
            }
        }

        // Now we need to visit a room.
        std::string name = *to_visit.begin();
        to_visit.erase(to_visit.begin());
        visit_room(name);
    }

    std::map<std::string, valve_t> result;

    for (const auto& dest: dests) {
        result[dest] = best_dists.at(dest);
    }

    return result;
}

dist_map_t build_dest_map(const std::vector<std::string>& dests, const room_map_t& room_map) {
    std::map<std::pair<std::string, std::string>, valve_t> result;

    for (size_t i = 0; i < dests.size(); ++i) {
        const auto& orig = dests[i];
        std::vector<std::string> new_dests;
        auto new_dests_inserter = std::back_inserter(new_dests);
        for (size_t j = i+1; j < dests.size(); ++j) {
            *new_dests_inserter = dests[j];
        }
        std::map<std::string,valve_t> dest_map = shortest_distances(orig, new_dests, room_map);

        for (const auto& map_el: dest_map) {
            const auto& dest = map_el.first;
            const auto& dist = map_el.second;
            result[std::make_pair(orig, dest)] = dist;
            result[std::make_pair(dest, orig)] = dist;
        }
    }

    return result;
}

valve_t task_1_max_relief_imp(std::vector<std::string> visited_rooms, valve_t time_remaining, const dist_map_t& dist_map, const rate_map_t& rate_map) {
    //std::cout << "task_1_max_relief_imp start: visited_rooms(" << visited_rooms.size() << "): " << render_iterable(visited_rooms) << std::endl;
    //std::cout << "rate_map:" << std::endl;
    //render_rate_map(rate_map);
    if (time_remaining == 0) {
        // No more time remaining.
        return 0;
    }
    //std::cout << "4" << std::endl;
    std::vector<std::string> available_valves;
    for (const auto& rate_el: rate_map) {
        const auto& name = rate_el.first;
        const auto& rate = rate_el.second;
        if (rate > 0) {
            available_valves.push_back(name);
        }
    }
    if (available_valves.size() == 0) {
        // No more valves remaining.
        return 0;
    }
    //std::cout << "5: " << render_iterable(available_valves) << std::endl;
    std::vector<valve_t> max_reliefs;
    // We can turn the valve off in this room.
    const auto& cur_room = *(visited_rooms.cend()-1);
    //std::cout << "6: " << cur_room << std::endl;
    if (rate_map.at(cur_room) > 0.) {
        valve_t room_relief = (time_remaining-1)*rate_map.at(cur_room);
        rate_map_t new_rate_map(rate_map);
        new_rate_map[cur_room] = 0.;
        max_reliefs.push_back(
            room_relief+task_1_max_relief_imp(visited_rooms, time_remaining-1, dist_map, new_rate_map)); 
        // Remove the current room from the available valves
        available_valves.erase(std::remove_if(available_valves.begin(), available_valves.end(), [&](const auto& name) {
            return name == cur_room;
        }), available_valves.end());
    }
    for (const auto& avail_valve: available_valves) {
        auto map_key = std::make_pair(cur_room, avail_valve);
        //std::cout << "7: " << map_key.first << "," << map_key.second << std::endl;
        // Now we try to go to another valve
        valve_t valve_dist = dist_map.at(map_key);
        // Check if we have time to go there.
        if (time_remaining > valve_dist) {
            // Equal also rules out the valve because we wouldn't have time to open the valve.
            rate_map_t new_rate_map(rate_map);
            std::vector<std::string> new_visited_rooms(visited_rooms);
            new_visited_rooms.push_back(avail_valve);
            max_reliefs.push_back(task_1_max_relief_imp(new_visited_rooms, time_remaining-valve_dist, dist_map, new_rate_map));
        }
    }
    //std::cout << "task_1_max_relief_imp end" << std::endl;
    return maxElement(max_reliefs);
}

void solve_task_1(const room_map_t& room_map, const rate_map_t& rate_map) {
    //std::cout << "2" << std::endl;
    // Build distance map with just the rooms of interest
    std::vector<std::string> rooms_of_interest = {"AA"};
    auto rooms_inserter = std::back_inserter(rooms_of_interest);
    for (const auto& map_el: rate_map) {
        const auto& name = map_el.first;
        const auto& rate = map_el.second;
        if (rate > 0) {
            *rooms_inserter = name;
        }
    }
    auto dist_map = build_dest_map(rooms_of_interest, room_map);
    //std::cout << "dist_map: " << dist_map.size() << std::endl;
    //for (const auto& map_el: dist_map) {
    //    std::stringstream ss;
    //    std::cout << "(" << map_el.first.first << "," << map_el.first.second << "): " << map_el.second << std::endl;
    //}

    //std::cout << "3" << std::endl;
    valve_t max_relief = task_1_max_relief_imp(
        std::vector<std::string>({"AA"}),
        30,
        dist_map,
        rate_map);
    std::cout << "Task 1: " << max_relief << std::endl;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("AOC Day 16 2022");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    room_map_t room_map;
    rate_map_t rate_map;

    std::regex line_re("Valve ([A-Z][A-Z]) has flow rate=([0-9]+); tunnel");
    std::regex room_re("([A-Z][A-Z])");

    auto read_line = [&](std::string line) {
        std::smatch m;
        std::stringstream ss;
        if(!std::regex_search(line, m, line_re)) {
            ss.clear();
            ss << "Unexpected line! (" << line << ")" << std::endl;
            throw std::runtime_error(ss.str());
        }
        std::string room_name = m[1];
        ss.clear();
        ss << m[2];
        valve_t rate;
        ss >> rate;
        // Save valve rate information
        rate_map[room_name] = rate;
        std::vector<std::string> connecting_rooms;
        line = m.suffix().str();

        while(std::regex_search(line, m, room_re)) {
            ss.clear();
            connecting_rooms.push_back(m[1]);
            line = m.suffix().str();
        }

        room_map[room_name] = connecting_rooms;
    };

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::string line;

    while(std::getline(infile, line)) {
        read_line(line);
    }

    for (const auto& map_el: room_map) {
        std::cout << map_el.first << ": " << rate_map[map_el.first] << " ";
        for (const auto& str: map_el.second) {
            std::cout << str << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << "1" << std::endl;
    solve_task_1(room_map, rate_map);

	return 0;
}

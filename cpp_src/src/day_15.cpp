#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include <set>
#include <optional>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef int64_t val_t;
typedef Point<val_t> point_t;

void solve_task_1(const std::vector<std::pair<point_t,point_t>>& sensor_beacon_pairs, val_t y) {
    // First, we should determine if the swept area intersects y.
    std::set<val_t> ruled_out_positions;
    std::set<val_t> other_beacon_positions;
    for (const auto& pair: sensor_beacon_pairs) {
        const auto& sensor = pair.first;
        const auto& beacon = pair.second;
        val_t radius = sensor.dist(beacon);
        if (beacon.y == y) {
            other_beacon_positions.insert(beacon.x);
        }
        val_t diff = std::abs(pair.first.y-y);
        if (diff <= radius) {
            // Compute diff
            val_t r_diff = radius-diff;
            val_t x_c = pair.first.x;
            for (val_t x = x_c-r_diff; x <= x_c+r_diff; ++x) {
                ruled_out_positions.insert(x);
            }
        }
    }
    std::set<val_t> ruled_out_positions_final = set_diff_fast(ruled_out_positions, other_beacon_positions);

    std::cout << "Task 1: " << ruled_out_positions_final.size() << std::endl;
}

val_t move_level(const point_t& p) {
    val_t x = std::abs(p.x);
    val_t y = std::abs(p.y);
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

typedef std::pair<point_t, point_t> line_t;

point_t line_intersection(const line_t& l_1, const line_t& l_2) {
    // line 1: p_1+a*d_1
    // line 2: p_2+b*d_2
    // let p_D = p_1-p_2
    // intersection: p_i = p_1+((d_1 wedge d_2) dot (p_D wedge d_2))/((d_1 wedge d_2) dot (d_2 wedge d_1)) d_1
    const auto& p_1 = l_1.first;
    const auto& d_1 = l_1.second;
    const auto& p_2 = l_2.first;
    const auto& d_2 = l_2.second;
    Point<float> p_1_f(p_1);
    Point<float> d_1_f(d_1);
    Point<float> p_2_f(p_2);
    Point<float> d_2_f(d_2);
    Point<float> p_d = p_1_f-p_2_f;
    Bivector<float> d12 = d_1_f.wedge(d_2_f);
    if (d12.xy == 0.) {
        throw std::runtime_error("Lines are parallel");
    }
    Bivector<float> pd = p_d.wedge(d_2_f);
    float alpha = d12.dot(pd)/(-d12.dot(d12));
    Point<float> p_i_f = p_1_f+(d_1_f*alpha);
    return point_t(p_i_f);
}

std::vector<point_t> intersect_points(const std::pair<point_t, point_t>& sensor_1, const std::pair<point_t, point_t>& sensor_2) {
    // compute intersection points of two sensors.
    // Its the intersection of the lines of the boundaries of the two sensors.
    std::vector<point_t> intersections;

    auto build_sensor_lines = [](const std::pair<point_t, point_t>& sensor) {
        std::map<std::string, line_t> lines;
        const point_t& sensor_center = sensor.first;
        val_t sensor_radius = sensor_center.dist(sensor.second);
        // corners
        point_t right = sensor_center+point_t(sensor_radius, 0);
        point_t up = sensor_center+point_t(0, sensor_radius); 
        point_t left = sensor_center+point_t(-sensor_radius, 0);
        point_t down = sensor_center+point_t(0, -sensor_radius);

        // p for positive slope, n for negative slope
        lines["p1"] = line_t(left, up-left);
        lines["p2"] = line_t(down, right-down);
        lines["n1"] = line_t(up, right-up);
        lines["n2"] = line_t(left, down-left);

        return lines;
    };

    auto sensor_1_lines = build_sensor_lines(sensor_1);
    auto sensor_2_lines = build_sensor_lines(sensor_2);

    auto inserter = std::back_inserter(intersections);

    *inserter = point_t(line_intersection(sensor_1_lines["p1"], sensor_2_lines["n1"]));
    *inserter = point_t(line_intersection(sensor_1_lines["p1"], sensor_2_lines["n2"]));
    *inserter = point_t(line_intersection(sensor_1_lines["p2"], sensor_2_lines["n1"]));
    *inserter = point_t(line_intersection(sensor_1_lines["p2"], sensor_2_lines["n2"]));
    *inserter = point_t(line_intersection(sensor_1_lines["n1"], sensor_2_lines["p1"]));
    *inserter = point_t(line_intersection(sensor_1_lines["n1"], sensor_2_lines["p2"]));
    *inserter = point_t(line_intersection(sensor_1_lines["n2"], sensor_2_lines["p1"]));
    *inserter = point_t(line_intersection(sensor_1_lines["n2"], sensor_2_lines["p2"]));

    return intersections;
}

void solve_task_2(const std::vector<std::pair<point_t,point_t>>& sensor_beacon_pairs, val_t max_xy) {
    // We create function which checks whether any sensor sees a point.
    auto observable = [&](const point_t& p) {
        for (const auto& pair: sensor_beacon_pairs) {
            const auto& sensor = pair.first;
            const auto& beacon = pair.second;
            val_t beacon_radius = sensor.dist(beacon);
            val_t p_radius = sensor.dist(p);
            if (p_radius <= beacon_radius) {
                return true;
            }
        }
        return false;
    };

    // We check a small neighborhood surrounding each equation intersection
    val_t neighborhood_size = 3;

    auto get_nonobservable_point = [&](const point_t& p) {
        std::vector<point_t> nonobservable_points;
        for(val_t x = p.x-neighborhood_size; x <= p.x+neighborhood_size; ++x) {
            if ((x < 0) || (x > max_xy)) {
                // Don't check out of bounds points
                continue;
            }
            for(val_t y = p.y-neighborhood_size; y <= p.y+neighborhood_size; ++y) {
                if ((y < 0) || (y > max_xy)) {
                    // Don't check out of bounds points
                    continue;
                }
                point_t p_check(x, y);
                if(!observable(p_check)) {
                    return std::optional<point_t>(p_check);
                }
            }
        }
        return std::optional<point_t>({});
    };

    point_t nonobservable_point(0,0);
    bool found = false;
    for (size_t s_i = 0; s_i < sensor_beacon_pairs.size(); ++s_i) {
        if (found) {
            break;
        }
        const auto& sensor_i = sensor_beacon_pairs[s_i];
        for (size_t s_j = s_i+1; s_j < sensor_beacon_pairs.size(); ++s_j) {
            if (found) {
                break;
            }
            const auto& sensor_j = sensor_beacon_pairs[s_j];
            std::vector<point_t> intersections = intersect_points(sensor_i, sensor_j);
            for(const auto& point: intersections) {
                auto res = get_nonobservable_point(point);
                if(res) {
                    nonobservable_point = res.value();
                    found = true;
                }
            }
        }
    }

    if (!found) {
        throw std::runtime_error("Couldn't find a non-observable point!");
    }

    // Compute and report answer.
    val_t tuning_freq = nonobservable_point.x*4000000+nonobservable_point.y;

    std::cout << "Task 2: " << tuning_freq << std::endl;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    val_t task_1_y = 2000000;
    val_t task_2_max = 4000000;
	ArgParse::ArgParser Parser("AOC Day 15 2022");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("--task-1-y", "y value to use for task 1", &task_1_y);
    Parser.AddArgument("--task-2-max", "max xy value to use for task 2", &task_2_max);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::string temp_line;

    std::regex line_re("x=([\\-\\d]+), y=([\\-\\d]+)");

    std::vector<std::pair<point_t, point_t>> in_data;
    while (std::getline(infile, temp_line)) {
        std::smatch m;
        auto extract_point = [&](std::smatch& m) {
            if (!std::regex_search(temp_line, m, line_re)) {
                std::stringstream ss;
                ss << "missing required match. line was: " << temp_line << std::endl;
                throw std::runtime_error(ss.str());
            }
            std::stringstream ss;
            val_t x;
            ss << m[1];
            ss >> x;
            ss.clear();
            val_t y;
            ss << m[2];
            ss >> y;
            ss.clear();
            return point_t(x, y);
        };
        point_t sensor = extract_point(m);
        temp_line = m.suffix().str();
        point_t beacon = extract_point(m);
        temp_line = m.suffix().str();
        in_data.push_back(std::pair<point_t, point_t>(sensor, beacon));
    }

    std::cout << "Read in data" << std::endl;
    for (const auto& el: in_data) {
        std::cout << "Sensor: " << el.first.str() << " Beacon: " << el.second.str() << std::endl;
    }

    solve_task_1(in_data, task_1_y);

    solve_task_2(in_data, task_2_max);

	return 0;
}

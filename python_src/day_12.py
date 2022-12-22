import argparse
import os
import sys
import tqdm

class Point(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __eq__(self, rhs):
        if (self.x == rhs.x) and (self.y == rhs.y):
            return True
        else:
            return False

    def __add__(self, rhs):
        return Point(self.x+rhs.x, self.y+rhs.y)

    def __sub__(self, rhs):
        return Point(self.x-rhs.x, self.y-rhs.y)

    def __hash__(self):
        return hash((self.x, self.y))

    def mag(self):
        return abs(self.x)+abs(self.y)

    def dist(self, rhs):
        return (self-rhs).mag()

    def neighbors(self):
        ds = [
            Point(0, 1), # Down
            Point(0, -1), # Up
            Point(-1, 0), # Left
            Point(1, 0), # Right
        ]
        ns = []
        for d in ds:
            ns.append(self+d)
        return ns

    def __str__(self):
        return f"({self.x},{self.y})"

    def __repr__(self):
        return f"{self}"


def in_bounds_map(p, topo_map):
    if (p.x < 0) or (p.y < 0):
        return False
    if (p.y >= len(topo_map)):
        return False
    if (p.x >= len(topo_map[0])):
        return False
    return True


def get_elevation_map(p, topo_map):
    return topo_map[p.y][p.x]


def path_distance(start_pos, end_pos, topo_map):
    b_dists = {
        start_pos: 0,
    }
    to_visit = {start_pos}

    def in_bounds(p):
        return in_bounds_map(p, topo_map)

    def get_elevation(p):
        return get_elevation_map(p, topo_map)

    def visit_point(p):
        cur_elevation = get_elevation(p)
        cur_b_dist = b_dists[p]
        neighbors = p.neighbors()
        neighbors = list(filter(
            in_bounds,
            neighbors))
        new_neighbors = []
        for n in neighbors:
            n_elevation = get_elevation(n)
            if n_elevation-cur_elevation <= 1:
                new_neighbors.append(n)

        neighbors = new_neighbors
        new_dist = cur_b_dist+1
        for n in neighbors:
            if n not in b_dists:
                # We haven't seen this point before.
                b_dists[n] = new_dist
                to_visit.add(n)
            else:
                if new_dist < b_dists[n]:
                    # We've seen this point before, but
                    # we found a better distance
                    b_dists[n] = new_dist
                    # We need to visit this particular neighbor again now
                    to_visit.add(n)

    while True:
        visit_points = list(to_visit)
        if end_pos in b_dists:
            # We have the possibility of ending our search now.
            costs = list(map(lambda p: b_dists[p], visit_points))
            min_cost = min(costs)
            if min_cost+1 > b_dists[end_pos]:
                # No nodes to visit which may yield a better path
                break
        # sort points by 
        visit_points = sorted(
            visit_points,
            key=lambda p: b_dists[p]+end_pos.dist(p))
        if len(visit_points) == 0:
            # We can't reach the end!
            return -1
        # Visit the point
        the_point = visit_points[0]

        # Remove this point from the visit list
        to_visit.remove(the_point)
        # Visit the point
        visit_point(the_point)

    return b_dists[end_pos]


if __name__ == "__main__":
    # Define Argument Parser
    parser = argparse.ArgumentParser('Advent of Code Day 12 2022')
    parser.add_argument('--input', help="Path to the input file", type=str, required=True)

    # Parse Arguments
    args = parser.parse_args()

    # Get input filepath
    input_filepath = args.input

    # Check input file exists
    if not os.path.exists(input_filepath):
        print(f"ERROR file {input_filepath} doesn't exist!")
        sys.exit(1)

    # Read data from file
    with open(input_filepath, 'r') as f:
        lines = f.readlines()

    # Store data into map
    topo_map = []
    start_pos = None
    end_pos = None
    for line in lines:
        line = line.strip()
        # Detect start position
        if start_pos is None:
            try:
                x = line.index('S')
                start_pos = Point(x, len(topo_map))
                line = line[:x]+'a'+line[x+1:]
            except ValueError:
                pass
        # Detect end position
        if end_pos is None:
            try:
                x = line.index('E')
                end_pos = Point(x, len(topo_map))
                line = line[:x]+'z'+line[x+1:]
            except ValueError:
                pass
        # Append line to map
        topo_map.append(line)

    print("Map")
    for line in topo_map:
        print(line)

    # Change topo_map to array of integers
    all_viable_starts = []
    for i in range(len(topo_map)):
        line = topo_map[i]
        new_line = []
        for j in range(len(line)):
            ele = ord(line[j])-ord('a')
            if ele == 0:
                all_viable_starts.append(Point(j, i))
            new_line.append(ele)
        topo_map[i] = new_line

    # Solve Task 1
    print(f"Task 1: {path_distance(start_pos, end_pos, topo_map)}")

    # Get all positions with elevation 0.
    path_dists = []
    for s in tqdm.tqdm(all_viable_starts):
        path_dists.append(path_distance(s, end_pos, topo_map))
    path_dists = list(filter(
        lambda d: d >= 0,
        path_dists))
    print(f"Task 2: {min(path_dists)}")

import argparse
import os
import sys

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __add__(self, rhs):
        return Point(self.x+rhs.x, self.y+rhs.y)

    def __sub__(self, rhs):
        return Point(self.x-rhs.x, self.y-rhs.y)

    def __eq__(self, rhs):
        if (self.x == rhs.x) and (self.y == rhs.y):
            return True
        else:
            return False

    def __hash__(self):
        return (self.x, self.y).__hash__()

    def __str__(self):
        return f"{self.x},{self.y}"


dir_map = {
    'R': Point(1, 0),
    'L': Point(-1, 0),
    'U': Point(0, 1),
    'D': Point(0, -1),
}


def move_tail(head, tail):
    ht_diff = head-tail
    if abs(ht_diff.x) > 1 or abs(ht_diff.y) > 1:
        # Difference is large enough to do have to move the tail.
        if abs(ht_diff.x) > 2 or abs(ht_diff.y) > 2:
            raise RuntimeError("Unexpected situation")
        if abs(ht_diff.x) == 2 and abs(ht_diff.y) == 2:
            # We must move diagonally
            diff = Point(ht_diff.x//2, ht_diff.y//2)
            new_tail = tail+diff
        elif abs(ht_diff.x) == 2:
            # x is the large diff.
            diff = Point(ht_diff.x//2, ht_diff.y)
            new_tail = tail+diff
        elif abs(ht_diff.y) == 2:
            # y is the large diff.
            diff = Point(ht_diff.x, ht_diff.y//2)
            new_tail = tail+diff
        else:
            raise RuntimeError("Unexpected situation 2")
    else:
        # head isn't far enough from the tail to do anything.
        new_tail = tail
    return new_tail


def show_knots(knots):
    xs = list(map(lambda p: p.x, knots))
    x_max = max(xs)
    x_min = min(xs)
    ys = list(map(lambda p: p.y, knots))
    y_max = max(ys)
    y_min = min(ys)
    for y in range(y_max, y_min-1, -1):
        line = ""
        for x in range(x_min, x_max+1):
            p = Point(x, y)
            located = False
            for i in range(len(knots)):
                knot = knots[len(knots)-1-i]
                if p == knot:
                    located = True
                    if i == len(knots)-1:
                        to_report = "H"
                    else:
                        k_i = (len(knots)-1)-i
                        to_report = f"{k_i}"
            if located:
                line += to_report
            else:
                line += "."
        print(line)


def sim_step(direction, knots):
    # Move head of knots
    knots[0] = knots[0]+direction
    for i in range(1,len(knots)):
        knots[i] = move_tail(knots[i-1], knots[i])


if __name__ == "__main__":
    # Define Argument Parser
    parser = argparse.ArgumentParser('Advent of Code Day 9 2022')
    parser.add_argument('--input', help="Path to the input file", type=str, required=True)

    # Parse Arguments
    args = parser.parse_args()

    # Get input filepath
    input_filepath = args.input

    # Check input file exists
    if not os.path.exists(input_filepath):
        print(f"ERROR file {input_filepath} doesn't exist!")
        sys.exit(1)

    with open(input_filepath, 'r') as f:
        lines = f.readlines()

    moves = []
    for line in lines:
        parts = line.split()
        D = parts[0]
        S = int(parts[1])
        moves.append((D, S))

    # knots start on top of eachother
    knots = []
    for i in range(2):
        knots.append(Point(0,0))

    tail_visited = set()
    tail_visited.add(knots[-1])

    for d, s in moves:
        direction = dir_map[d]
        for i in range(s):
            sim_step(direction, knots)
            tail_visited.add(knots[-1])

    print(f"Task 1: {len(tail_visited)}")

    # knots start on top of eachother
    knots = []
    for i in range(10):
        knots.append(Point(0,0))

    tail_visited = set()
    tail_visited.add(knots[-1])

    step = 0
    for d, s in moves:
        direction = dir_map[d]
        for i in range(s):
            step += 1
            sim_step(direction, knots)
            tail_visited.add(knots[-1])

    print(f"Task 2: {len(tail_visited)}")

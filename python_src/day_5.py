import argparse
import os
import sys
import re
import copy

# Define Argument Parser
parser = argparse.ArgumentParser('Advent of Code Day 5 2022')
parser.add_argument('--input', help="Path to the input file", type=str, required=True)

# Parse Arguments
args = parser.parse_args()

# Get input filepath
input_filepath = args.input

# Check input file exists
if not os.path.exists(input_filepath):
    print(f"ERROR file {input_filepath} doesn't exist!")
    sys.exit(1)

cargo_lines = []
move_lines = []

with open(input_filepath, 'r') as f:
    lines = f.readlines()

cargo = True
for line in lines:
    if line.strip() == "":
        cargo = False
        continue
    if cargo:
        cargo_lines.append(line.rstrip())
    else:
        move_lines.append(line.rstrip())

num_columns = int(cargo_lines[-1][-1])

cargo_columns = []
for i in range(num_columns):
    cargo_columns.append([])

for line in cargo_lines[:-1]:
    for i in range(num_columns):
        pos = 1+4*i
        if pos >= len(line):
            break
        if line[pos] == " ":
            continue
        else:
            cargo_columns[i].append(line[pos])

move_rgx = re.compile("move ([0-9]*) from ([0-9]*) to ([0-9]*)")
move_data = []
for line in move_lines:
    match = move_rgx.search(line)
    groups = match.groups()
    move_data.append((int(groups[0]), int(groups[1]), int(groups[2])))

def execute_move_9000(a, b):
    """
    Moves a crate from top of a to top of b.
    """
    el = a.pop(0)
    b.insert(0, el)

def execute_moves_9000(columns, moves):
    columns_new = copy.deepcopy(columns)
    for num, col_a, col_b in moves:
        a = columns_new[col_a-1]
        b = columns_new[col_b-1]
        for i in range(num):
            execute_move_9000(a, b)
    return columns_new

def execute_move_9001(n, a, b):
    """
    Moves a crate from top of a to top of b.
    """
    els = []
    for i in range(n):
        el = a.pop(0)
        els.append(el)
    for el in els[::-1]:
        b.insert(0, el)

def execute_moves_9001(columns, moves):
    columns_new = copy.deepcopy(columns)
    for num, col_a, col_b in moves:
        a = columns_new[col_a-1]
        b = columns_new[col_b-1]
        execute_move_9001(num, a, b)
    return columns_new

# Solve Task 1
cols_after_move = execute_moves_9000(cargo_columns, move_data)

string_result = ""
for col in cols_after_move:
    string_result += col[0]

print(f"Task 1: {string_result}")

# Solve Task 2
cols_after_move = execute_moves_9001(cargo_columns, move_data)

string_result = ""
for col in cols_after_move:
    string_result += col[0]

print(f"Task 2: {string_result}")

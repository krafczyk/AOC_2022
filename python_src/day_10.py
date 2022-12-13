import argparse
import os
import sys

class ComputerState(object):
    def __init__(self, X=1, cycle=1):
        self.X = X
        self.cycle = cycle

    def __str__(self):
        return f"State: cycle: {self.cycle} X: {self.X}"

def addx(state, instruction):
    #print("addx")
    #print(f"X: {state.X}")
    #print(f"instruction[1]: {instruction[1]}")
    return ComputerState(
        X=state.X+instruction[1],
        cycle=state.cycle+2)

def noop(state, instruction):
    return ComputerState(
        X=state.X,
        cycle=state.cycle+1
    )

instruction_map = {
    'addx': addx,
    'noop': noop,
}

if __name__ == "__main__":
    # Define Argument Parser
    parser = argparse.ArgumentParser('Advent of Code Day 10 2022')
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

    instructions = []
    for line in lines:
        line_split = line.split()
        if line_split[0] == 'addx':
            line_split[1] = int(line_split[1])
        instructions.append(line_split)

    #for inst in instructions:
    #    print(inst)

    # Task 1
    # Initialize the machine
    state = ComputerState()

    cycles = list(range(1, 240+1))

    register_values = []

    cycle_target_idx = 0
    last_state = state
    for i in range(len(instructions)):
        inst = instructions[i]
        #print(f"considering instruction: {inst}")
        #print(f"current: {last_state}")
        new_state = instruction_map[inst[0]](last_state, inst)
        #print(f"new: {new_state}")
        while cycle_target_idx < len(cycles) and \
                cycles[cycle_target_idx] <= new_state.cycle:
            #print(f"current cycle target: {cycle_target_idx} - {cycles[cycle_target_idx]}")
            if cycles[cycle_target_idx] < new_state.cycle:
                #print("add a")
                register_values.append(last_state.X)
            elif cycles[cycle_target_idx] == new_state.cycle:
                #print("add b")
                register_values.append(new_state.X)
            cycle_target_idx += 1
        # Advance state
        last_state = new_state

    #for c, x in zip(cycles, register_values):
    #    print(f"{c}: {x}")

    cycle_targets = [
        20,
        60,
        100,
        140,
        180,
        220,
    ]

    total = 0
    for c in cycle_targets:
        c_i = c-1
        total += cycles[c_i]*register_values[c_i]

    print(f"Task 1: {total}")

    num_rows = 6
    num_chars = 40

    print("Task 2:")
    current_cycle = 1
    for row_idx in range(num_rows):
        line = ""
        for char_idx in range(num_chars):
            cycle_idx = current_cycle-1
            x = register_values[cycle_idx]
            if abs(x-char_idx) <= 1:
                line += "#"
            else:
                line += "."
            current_cycle += 1
        print(line)

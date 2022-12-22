import argparse
import os
import sys
from enum import Enum


class Order(Enum):
    Right = 1
    Unknown = 0
    Wrong = -1


def test_order(left, right):
    # First, handle the int-int comparison
    if (type(left) is int) and (type(right) is int):
        # Compare two integers
        if left < right:
            return Order.Right
        elif left > right:
            return Order.Wrong
        else:
            return Order.Unknown
    # we have at least one list now.
    # we should transform the int into a list
    if (type(left) is int):
        left = [left]
    if (type(right) is int):
        right = [right]
    for i in range(max(len(left),len(right))):
        try:
            v_l = left[i]
        except IndexError:
            # left doesn't have enough items! in right order!
            return Order.Right
        try:
            v_r = right[i]
        except IndexError:
            # right doesn't have enough items
            return Order.Wrong
        v_res = test_order(v_l, v_r)
        if v_res != Order.Unknown:
            # Found determinant value test
            return v_res
    # No test could determine in this branch.
    return Order.Unknown


def zip_sorted_packet_lists(a, b):
    new_list = []
    while (len(a) > 0) and (len(b) > 0):
        order_check = test_order(a[0], b[0]) 
        if order_check == Order.Right:
            new_list.append(a.pop(0))
        elif order_check == Order.Wrong:
            new_list.append(b.pop(0))
        else:
            raise RuntimeError("Couldn't determine order of two elements!")
    if len(a) > 0:
        new_list += a
    elif len(b) > 0:
        new_list += b
    return new_list

def sort_packet_list(in_list):
    # Special case (a single element)
    if len(in_list) == 1:
        return in_list
    # Special case (two elements)
    if len(in_list) == 2:
        order_check = test_order(in_list[0], in_list[1])
        if order_check == Order.Right:
            return in_list
        elif order_check == Order.Wrong:
            return [in_list[1], in_list[0]]
        else:
            raise RuntimeError("Couldn't compare two elements")
    # Divide list into two halves
    left_list = in_list[:len(in_list)//2]
    right_list = in_list[len(in_list)//2:]
    # Sort the smaller lists
    left_list_sorted = sort_packet_list(left_list)
    right_list_sorted = sort_packet_list(right_list)
    # Zip the sorted lists together
    return zip_sorted_packet_lists(left_list_sorted, right_list_sorted)


if __name__ == "__main__":
    # Define Argument Parser
    parser = argparse.ArgumentParser('Advent of Code Day 13 2022')
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

    packet_pairs = []
    left = None
    for line in lines:
        line = line.strip()
        if line != "":
            if left == None:
                left = eval(line)
            else:
                right = eval(line)
                packet_pairs.append((left, right))
        else:
            left = None

    number_right = 0
    task1 = 0
    for idx, pair in enumerate(packet_pairs):
        result = test_order(*pair)
        if result == Order.Right:
            number_right += 1
            task1 += (idx+1)

    print(f"Task 1: {task1}")

    # Get all packets individually
    all_packets = []
    for pair in packet_pairs:
        all_packets.append(pair[0])
        all_packets.append(pair[1])
    divider_1 = [[2]]
    divider_2 = [[6]]
    all_packets.append(divider_1)
    all_packets.append(divider_2)

    sorted_packets = sort_packet_list(
        all_packets)
   
    idx_1 = sorted_packets.index(divider_1)+1
    idx_2 = sorted_packets.index(divider_2)+1

    print(f"Task 2: {idx_1*idx_2}")

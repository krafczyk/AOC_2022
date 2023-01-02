#include "utilities.h"

int pair_hash(int x, int y) {
    // Here we use something similar to cantor pairing.
    // https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
    unsigned int A = (x >= 0 ? 2*x : -2*x-1);
    unsigned int B = (y >= 0 ? 2*y : -2*y-1);
    int C = (int)((A >= B ? A*A+A+B : A+B*B)/2);
    return ((x < 0 && y < 0) || (x >= 0 && y >= 0) ? C : -C - 1);
}

long pair_hash_l(long x, long y) {
    // Here we use something similar to cantor pairing.
    // https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
    unsigned long A = (x >= 0 ? 2*x : -2*x-1);
    unsigned long B = (y >= 0 ? 2*y : -2*y-1);
    long C = (long)((A >= B ? A*A+A+B : A+B*B)/2);
    return ((x < 0 && y < 0) || (x >= 0 && y >= 0) ? C : -C - 1);
}

template<typename Out>
void split_imp(const std::string& s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split_imp(s, delim, std::back_inserter(elems));
    return elems;
}

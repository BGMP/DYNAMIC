#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include "dynamic/dynamic.hpp"

using namespace std;
using namespace dyn;

#define NUM_VALUES 100

// Load text file as sequence of bytes
vector<uint64_t> load_text_as_integers(const string& filename) {
    vector<uint64_t> values;
    ifstream file(filename);

    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename.c_str());
        return values;
    }

    char c;
    while (file.get(c)) {
        values.push_back(static_cast<uint64_t>(static_cast<unsigned char>(c)));
    }

    file.close();
    return values;
}

// Build packed_spsi from values
packed_spsi build_spsi(const vector<uint64_t>& values) {
    packed_spsi ps;

    for (size_t i = 0; i < values.size(); i++) {
        ps.insert(i, values[i]);
    }

    return ps;
}

int main(int argc, char* argv[]) {
    double time_append;
    clock_t begin, end;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    uint64_t values[NUM_VALUES] = {
        1, 5, 10, 2, 8, 15, 3, 12, 7, 20,
        50, 100, 250, 500, 75, 300, 150, 400, 200, 600,
        1000, 1500, 2000, 2500, 1200, 1800, 2200, 1100, 1900, 2400,
        5000, 7500, 10000, 8000, 6000, 9000, 7000, 8500, 6500, 9500,
        15000, 20000, 25000, 30000, 18000, 22000, 28000, 16000, 24000, 32000,
        50000, 75000, 100000, 60000, 80000, 90000, 70000, 85000, 95000, 65000,
        150000, 200000, 250000, 180000, 220000, 240000, 160000, 210000, 230000, 190000,
        500000, 750000, 1000000, 600000, 800000, 900000, 700000, 850000, 950000, 650000,
        1500000, 2000000, 2500000, 1800000, 2200000, 2400000, 1600000, 2100000, 2300000, 1900000,
        5000000, 7500000, 10000000, 6000000, 8000000, 9000000, 7000000, 8500000, 9500000, 6500000
    };

    // Load and build structure
    vector<uint64_t> initial_values = load_text_as_integers(argv[1]);
    if (initial_values.empty()) {
        return 1;
    }

    packed_spsi ps = build_spsi(initial_values);

    // Append new values and measure time
    begin = clock();
    for(int i = 0; i < NUM_VALUES; i++) {
        ps.insert(ps.size(), values[i]);  // Append at end
    }
    end = clock();
    time_append = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("%f\n", time_append);

    return 0;
}

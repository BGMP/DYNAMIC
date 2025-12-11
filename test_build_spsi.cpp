#include <time.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include "dynamic/dynamic.hpp"

using namespace std;
using namespace dyn;

// Load text file as sequence of bytes (like DYNAMIC does)
vector<uint64_t> load_text_as_integers(const string& filename) {
    vector<uint64_t> values;
    ifstream file(filename);  // TEXT mode, not binary

    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename.c_str());
        return values;
    }

    char c;
    while (file.get(c)) {
        // Convert each character to unsigned char (0-255) then to uint64_t
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
    double time_build;
    clock_t begin, end;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Load values from text file
    vector<uint64_t> values = load_text_as_integers(argv[1]);
    if (values.empty()) {
        return 1;
    }

    // Time the structure build (equivalent to createFT in DDAC_WTB)
    begin = clock();
    packed_spsi ps = build_spsi(values);
    end = clock();

    time_build = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f\n", time_build);

    return 0;
}
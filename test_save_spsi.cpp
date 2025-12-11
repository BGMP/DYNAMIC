#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include "dynamic/dynamic.hpp"

using namespace std;
using namespace dyn;

// Load text file as sequence of bytes (like DYNAMIC does)
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
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        fprintf(stderr, "Example: %s boost.1GB output.spsi\n", argv[0]);
        return 1;
    }
    
    string input_file = argv[1];
    string output_file = argv[2];
    
    printf("Loading dataset from: %s\n", input_file.c_str());
    
    // Load and build structure
    vector<uint64_t> values = load_text_as_integers(input_file);
    if (values.empty()) {
        fprintf(stderr, "Error: No data loaded\n");
        return 1;
    }
    
    printf("Loaded %lu values\n", values.size());
    printf("Building packed_spsi structure...\n");
    
    packed_spsi ps = build_spsi(values);
    
    printf("Structure built with %lu elements\n", ps.size());
    printf("Saving to: %s\n", output_file.c_str());
    
    // Save to file (using DYNAMIC's serialize method)
    ofstream out(output_file, ios::binary);
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file %s\n", output_file.c_str());
        return 1;
    }
    
    ps.serialize(out);
    out.close();
    
    printf("Successfully saved to %s\n", output_file.c_str());
    
    return 0;
}

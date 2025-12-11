#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include "dynamic/dynamic.hpp"

using namespace std;
using namespace dyn;

#define NUM_DELETES 100

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
    double time_delete;
    clock_t begin, end;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    // Load and build structure
    vector<uint64_t> values = load_text_as_integers(argv[1]);
    if (values.empty()) {
        return 1;
    }
    
    packed_spsi ps = build_spsi(values);
    uint64_t original_size = ps.size();
    
    // Check if we have enough elements
    if (original_size < NUM_DELETES) {
        fprintf(stderr, "Error: Dataset too small. Has %lu elements, need at least %d\n",
                original_size, NUM_DELETES);
        return 1;
    }
    
    // Seed random number generator
    srand(42);
    
    // Perform random deletes
    begin = clock();
    for(int i = 0; i < NUM_DELETES; i++) {
        uint64_t current_size = ps.size();
        
        if (current_size == 0) {
            fprintf(stderr, "Error: List is empty at iteration %d\n", i);
            break;
        }
        
        uint64_t pos = rand() % current_size;  // 0-indexed
        ps.remove(pos);
    }
    end = clock();
    
    time_delete = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f\n", time_delete);
    
    return 0;
}

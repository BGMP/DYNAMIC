#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include "dynamic/dynamic.hpp"

using namespace std;
using namespace dyn;

#define NUM_REPLACES 100

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
    double time_replace;
    clock_t begin, end;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    // Values to use for replacement (same as DDAC_WTB)
    uint64_t values[NUM_REPLACES] = {
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
    uint64_t original_size = ps.size();
    
    // Store original values at specific positions
    uint64_t orig_first = ps.at(0);
    uint64_t orig_last = ps.at(original_size - 1);
    uint64_t orig_mid = ps.at(original_size / 2);
    uint64_t orig_pos_100 = ps.at(99);   // position 100 in DDAC = position 99 in DYNAMIC
    uint64_t orig_pos_1000 = ps.at(999); // position 1000 in DDAC = position 999 in DYNAMIC
    
    // TEST 1: Replace at END positions
    uint64_t end_positions[10];
    uint64_t end_old_values[10];
    
    begin = clock();
    
    for(int i = 0; i < 10; i++) {
        uint64_t pos = original_size - 10 + i; // DDAC: original_size - 9 + i (1-indexed), DYNAMIC: original_size - 10 + i (0-indexed)
        end_positions[i] = pos;
        end_old_values[i] = ps.at(pos);
        ps.set(pos, values[i]);
    }
    
    // Verify replaced elements at end
    int errors = 0;
    for(int i = 0; i < 10; i++) {
        uint64_t retrieved = ps.at(end_positions[i]);
        if(retrieved != values[i]) errors++;
    }
    
    // Verify other elements unchanged
    uint64_t check_first = ps.at(0);
    uint64_t check_mid = ps.at(original_size / 2);
    uint64_t check_100 = ps.at(99);
    
    if(check_first != orig_first) errors++;
    if(check_100 != orig_pos_100) errors++;
    if(check_mid != orig_mid) errors++;
    
    // TEST 2: Replace at BEGINNING positions
    uint64_t begin_positions[10];
    uint64_t begin_old_values[10];
    
    for(int i = 0; i < 10; i++) {
        uint64_t pos = i; // DDAC: i + 1 (1-indexed), DYNAMIC: i (0-indexed)
        begin_positions[i] = pos;
        begin_old_values[i] = ps.at(pos);
        ps.set(pos, values[10 + i]);
    }
    
    // Verify replaced elements at beginning
    for(int i = 0; i < 10; i++) {
        uint64_t retrieved = ps.at(begin_positions[i]);
        if(retrieved != values[10 + i]) errors++;
    }
    
    // TEST 3: Replace in MIDDLE positions
    uint64_t mid_start = original_size / 2 - 5;
    uint64_t mid_positions[10];
    uint64_t mid_old_values[10];
    
    for(int i = 0; i < 10; i++) {
        uint64_t pos = mid_start + i;
        mid_positions[i] = pos;
        mid_old_values[i] = ps.at(pos);
        ps.set(pos, values[20 + i]);
    }
    
    // Verify replaced elements in middle
    for(int i = 0; i < 10; i++) {
        uint64_t retrieved = ps.at(mid_positions[i]);
        if(retrieved != values[20 + i]) errors++;
    }
    
    // TEST 4: Replace with VERY LARGE values
    uint64_t large_positions[10] = {999, 1999, 2999, 3999, 4999, 5999, 6999, 7999, 8999, 9999}; // Adjusted for 0-indexed
    uint64_t large_old_values[10];
    
    for(int i = 0; i < 10; i++) {
        large_old_values[i] = ps.at(large_positions[i]);
        uint64_t large_val = values[90 + i];
        ps.set(large_positions[i], large_val);
    }
    
    // Verify large value replacements
    for(int i = 0; i < 10; i++) {
        uint64_t retrieved = ps.at(large_positions[i]);
        uint64_t expected = values[90 + i];
        if(retrieved != expected) errors++;
    }
    
    // TEST 5: Replace with SMALLER values
    uint64_t small_positions[5] = {999, 1999, 2999, 3999, 4999}; // Adjusted for 0-indexed
    
    for(int i = 0; i < 5; i++) {
        uint64_t old_val = ps.at(small_positions[i]);
        uint64_t small_val = i + 1;
        ps.set(small_positions[i], small_val);
    }
    
    // Verify small value replacements
    for(int i = 0; i < 5; i++) {
        uint64_t retrieved = ps.at(small_positions[i]);
        uint64_t expected = i + 1;
        if(retrieved != expected) errors++;
    }
    
    // TEST 6: Verify list size unchanged
    if(ps.size() != original_size) errors++;
    
    // TEST 7: Spot check unreplaced elements
    uint64_t spot_positions[5] = {49999, 59999, 69999, 79999, 89999}; // Adjusted for 0-indexed
    for(int i = 0; i < 5; i++) {
        if(spot_positions[i] < original_size) {
            uint64_t val = ps.at(spot_positions[i]);
        }
    }
    
    end = clock();
    time_replace = (double)(end - begin) / CLOCKS_PER_SEC;
    
    printf("%f\n", time_replace);
    
    return errors;
}

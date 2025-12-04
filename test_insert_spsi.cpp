#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include "dynamic/dynamic.hpp"

using namespace std;
using namespace dyn;

#define NUM_INSERTS 100

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
    clock_t begin, end;
    double time_total;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    // Values to insert (same as DDAC_WTB)
    uint64_t values[NUM_INSERTS] = {
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
    
    // Store some original values before insertion
    uint64_t orig_first = ps.at(0);              // position 0 in DYNAMIC = position 1 in DDAC
    uint64_t orig_last = ps.at(original_size - 1);
    uint64_t orig_mid = ps.at(original_size / 2);
    
    // START TIMING
    begin = clock();
    
    // TEST 1: Insert all values at the END
    for(int i = 0; i < NUM_INSERTS; i++) {
        uint64_t pos = ps.size();  // Insert at end
        ps.insert(pos, values[i]);
    }
    
    // Verify: Original elements
    int errors = 0;
    uint64_t check_first = ps.at(0);
    uint64_t check_last_orig = ps.at(original_size - 1);
    uint64_t check_mid = ps.at(original_size / 2);
    
    if(check_first != orig_first) errors++;
    if(check_mid != orig_mid) errors++;
    if(check_last_orig != orig_last) errors++;
    
    // Verify: Inserted elements at end
    for(int i = 0; i < NUM_INSERTS; i++) {
        uint64_t expected_pos = original_size + i;
        uint64_t retrieved = ps.at(expected_pos);
        if(retrieved != values[i]) errors++;
    }
    
    // TEST 2: Insert at BEGINNING
    uint64_t test_vals[5] = {999999, 888888, 777777, 666666, 555555};
    
    for(int i = 0; i < 5; i++) {
        ps.insert(0, test_vals[i]);  // Insert at position 0 (beginning)
    }
    
    // Verify: Elements at beginning
    for(int i = 0; i < 5; i++) {
        uint64_t retrieved = ps.at(i);
        uint64_t expected = test_vals[4 - i];
        if(retrieved != expected) errors++;
    }
    
    // Verify original first element got shifted
    uint64_t shifted_orig_first = ps.at(5);
    if(shifted_orig_first != orig_first) errors++;
    
    // TEST 3: Insert in MIDDLE
    uint64_t mid_pos = ps.size() / 2;
    uint64_t elem_before = ps.at(mid_pos - 1);
    uint64_t elem_after = ps.at(mid_pos);
    
    ps.insert(mid_pos, 444444);
    
    // Verify: Middle insertion
    uint64_t check_before = ps.at(mid_pos - 1);
    uint64_t check_inserted = ps.at(mid_pos);
    uint64_t check_after = ps.at(mid_pos + 1);
    
    if(check_before != elem_before) errors++;
    if(check_inserted != 444444) errors++;
    if(check_after != elem_after) errors++;
    
    // END TIMING
    end = clock();
    time_total = (double)(end - begin) / CLOCKS_PER_SEC;
    
    printf("%f\n", time_total);
    
    return errors > 0 ? 1 : 0;
}

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include "dynamic/dynamic.hpp"
using namespace std;
using namespace dyn;

int main(int argc, char* argv[]) {
    double time_append;
    clock_t begin, end;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_spsi_file> <numbers_file>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* numbers_file = argv[2];

    // Read numbers from text file
    ifstream numfile(numbers_file);
    if (!numfile) {
        fprintf(stderr, "Error: Cannot open numbers file %s\n", numbers_file);
        return 1;
    }

    vector<uint64_t> numbers;
    uint64_t value;
    while (numfile >> value) {
        numbers.push_back(value);
    }
    numfile.close();

    if (numbers.empty()) {
        fprintf(stderr, "Error: Numbers file is empty\n");
        return 1;
    }

    // Load existing packed_spsi structure from binary file
    ifstream infile(input_file, ios::binary);
    if (!infile) {
        fprintf(stderr, "Error: Cannot open file %s\n", input_file);
        return 1;
    }

    packed_spsi ps;
    ps.load(infile);
    infile.close();

    // Perform append operations with all values from file
    begin = clock();
    for (size_t i = 0; i < numbers.size(); i++) {
        ps.insert(ps.size(), numbers[i]);  // Append at end
    }
    end = clock();
    time_append = (double)(end - begin) / CLOCKS_PER_SEC;

    // Generate output filename: input.N.spsi
    string input_str(input_file);
    string output_file;
    size_t ext_pos = input_str.rfind(".spsi");
    if (ext_pos != string::npos) {
        output_file = input_str.substr(0, ext_pos);
    }
    else {
        output_file = input_str;
    }

    // Append .N.spsi (where N is the number of values appended)
    output_file += "." + to_string(numbers.size()) + ".spsi";

    // Save the modified structure
    ofstream outfile(output_file, ios::binary);
    if (!outfile) {
        fprintf(stderr, "Error: Cannot create output file %s\n", output_file.c_str());
        return 1;
    }
    ps.serialize(outfile);
    outfile.close();

    // Output only the execution time
    printf("%f\n", time_append);

	//printf("Showing the last %lu elements appended:", numbers.size());
    //for (size_t i = ps.size() - numbers.size(); i < ps.size(); i++) {
    //    printf(" %lu\n", ps.at(i));
	//}

    return 0;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

bool is_hex(const std::string& str) {
    return str.length() == 2 && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isxdigit(static_cast<unsigned char>(c));
    });
}

void strip_packet(std::ifstream& input, std::ofstream& output) {
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream iss(line);
        std::string offset;
        if (iss >> offset) {
            std::string hex_value;
            std::string hex_line;
            
            // Collect all hex values, including the first ones
            while (iss >> hex_value) {
                if (is_hex(hex_value)) {
                    hex_line += hex_value;
                } else {
                    // Stop if we hit non-hex data (usually the ASCII representation at the end)
                    break;
                }
            }
            
            if (!hex_line.empty()) {
                output << hex_line << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1], std::ios::in);
    if (!input) {
        std::cerr << "Error opening input file: " << argv[1] << std::endl;
        return 1;
    }

    std::ofstream output(argv[2], std::ios::out);
    if (!output) {
        std::cerr << "Error opening output file: " << argv[2] << std::endl;
        return 1;
    }

    strip_packet(input, output);

    std::cout << "Complete raw hex data has been extracted and written to " << argv[2] << std::endl;

    return 0;
}
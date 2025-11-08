#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <sstream>
#include <iomanip>
#include <cstdint>

// Convert a byte to binary string representation
std::string byteToBinary(uint8_t byte) {
    return "0b" + std::bitset<8>(byte).to_string();
}

// Process a hex value and output its byte representation
void processHexValue(const std::string& hexStr, std::ofstream& outFile) {
    // Remove "0x" prefix if present
    std::string cleanHex = hexStr;
    if (cleanHex.substr(0, 2) == "0x" || cleanHex.substr(0, 2) == "0X") {
        cleanHex = cleanHex.substr(2);
    }
    
    // Convert hex string to unsigned 32-bit integer
    uint32_t value;
    std::stringstream ss;
    ss << std::hex << cleanHex;
    ss >> value;
    
    // Write the original hex value
    outFile << hexStr << std::endl;
    
    // Extract and write each byte (little-endian: LSB first)
    // Byte 0: bits 0-7 (LSB)
    outFile << byteToBinary(value & 0xFF) << std::endl;
    // Byte 1: bits 8-15
    outFile << byteToBinary((value >> 8) & 0xFF) << std::endl;
    // Byte 2: bits 16-23
    outFile << byteToBinary((value >> 16) & 0xFF) << std::endl;
    // Byte 3: bits 24-31 (MSB)
    outFile << byteToBinary((value >> 24) & 0xFF) << std::endl;
    
    // Add blank line between entries
    outFile << std::endl;
}

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    
    // Open input file
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open input file: " << inputFile << std::endl;
        return 1;
    }
    
    // Open output file
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << outputFile << std::endl;
        inFile.close();
        return 1;
    }
    
    // Process each line
    std::string line;
    while (std::getline(inFile, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        if (start != std::string::npos && end != std::string::npos) {
            line = line.substr(start, end - start + 1);
        }
        
        // Process the hex value
        processHexValue(line, outFile);
    }
    
    // Close files
    inFile.close();
    outFile.close();
    
    std::cout << "Conversion complete! Output written to: " << outputFile << std::endl;
    
    return 0;
}

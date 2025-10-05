#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstdint>

class HexToBinaryConverter {
public:
    bool convertFile(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream inFile(inputFile);
        if (!inFile.is_open()) {
            std::cerr << "Error: Could not open input file: " << inputFile << std::endl;
            return false;
        }

        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not create output file: " << outputFile << std::endl;
            inFile.close();
            return false;
        }

        std::string line;
        int lineNumber = 0;
        int totalValues = 0;

        while (std::getline(inFile, line)) {
            lineNumber++;
            
            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            // Remove whitespace and check if line starts with 0x
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.substr(0, 2) != "0x") {
                std::cerr << "Warning: Line " << lineNumber << " does not start with '0x': " << line << std::endl;
                continue;
            }

            // Extract hex string (remove 0x prefix)
            std::string hexString = line.substr(2);
            
            // Validate hex string length (should be even and represent 32-bit values)
            if (hexString.length() != 8) {
                std::cerr << "Warning: Line " << lineNumber << " - Expected 8 hex digits (32-bit), got " 
                          << hexString.length() << ": " << line << std::endl;
                continue;
            }

            // Convert hex string to 32-bit integer
            try {
                uint32_t value = std::stoul(hexString, nullptr, 16);
                
                // Write as little-endian bytes
                outFile.write(reinterpret_cast<const char*>(&value), sizeof(value));
                
                totalValues++;
                
                if (totalValues % 10 == 0) {
                    std::cout << "Processed " << totalValues << " values..." << std::endl;
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Error parsing line " << lineNumber << ": " << line 
                          << " - " << e.what() << std::endl;
            }
        }

        inFile.close();
        outFile.close();

        std::cout << "Conversion completed successfully!" << std::endl;
        std::cout << "Total values converted: " << totalValues << std::endl;
        std::cout << "Output file size: " << (totalValues * 4) << " bytes" << std::endl;

        return true;
    }

    void printFileInfo(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            std::streamsize size = file.tellg();
            std::cout << "Binary file '" << filename << "' size: " << size << " bytes" << std::endl;
            std::cout << "Number of 32-bit values: " << (size / 4) << std::endl;
            file.close();
        }
    }

    void displayBinaryContent(const std::string& filename, int maxValues = 16) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Could not open binary file for reading: " << filename << std::endl;
            return;
        }

        std::cout << "\nFirst " << maxValues << " values from binary file (as hex):" << std::endl;
        
        uint32_t value;
        int count = 0;
        
        while (file.read(reinterpret_cast<char*>(&value), sizeof(value)) && count < maxValues) {
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(8) << value << std::endl;
            count++;
        }
        
        file.close();
        std::cout << std::dec; // Reset to decimal output
    }
};

int main(int argc, char* argv[]) {
    std::string inputFile = "3rdDebugger-Parsed.txt";
    std::string outputFile = "output.bin";
    
    // Handle command line arguments
    if (argc >= 2) {
        inputFile = argv[1];
    }
    if (argc >= 3) {
        outputFile = argv[2];
    }
    
    std::cout << "Hex to Binary Converter" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "Input file: " << inputFile << std::endl;
    std::cout << "Output file: " << outputFile << std::endl << std::endl;

    HexToBinaryConverter converter;
    
    if (converter.convertFile(inputFile, outputFile)) {
        converter.printFileInfo(outputFile);
        converter.displayBinaryContent(outputFile);
        
        std::cout << "\nUsage: " << argv[0] << " [input_file] [output_file]" << std::endl;
        std::cout << "Default: input.txt -> output.bin" << std::endl;
        
        return 0;
    } else {
        std::cerr << "Conversion failed!" << std::endl;
        return 1;
    }
}
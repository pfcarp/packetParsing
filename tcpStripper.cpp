#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

class TcpdumpPayloadExtractor {
private:
    std::string inputFile;
    std::string outputFile;
    
    // Remove the hex offset at the beginning of each line
    std::string stripHexOffset(const std::string& line) {
        // Typical format: "0x0000:  4500 003c ..."
        std::regex offsetPattern("^\\s*0x[0-9a-fA-F]+:\\s+");
        return std::regex_replace(line, offsetPattern, "");
    }
    
    // Clean and format hex string
    std::string cleanHexData(const std::string& hexStr) {
        std::string result;
        
        // Remove spaces and keep only hex digits
        for (char c : hexStr) {
            if (std::isxdigit(c)) {
                result += c;
            }
        }
        
        return result;
    }
    
    bool isPayloadLine(const std::string& line) {
        // Payload lines typically start with hex offset like "0x0000:"
        std::regex payloadPattern("^\\s*0x[0-9a-fA-F]+:");
        return std::regex_search(line, payloadPattern);
    }
    
    bool isPacketHeader(const std::string& line) {
        // Check for various packet header indicators
        return (line.find("IP ") != std::string::npos || 
                line.find("TCP ") != std::string::npos ||
                line.find("UDP ") != std::string::npos ||
                line.find("ethertype") != std::string::npos ||
                (line.find(":") != std::string::npos && 
                 line.find(".") != std::string::npos && 
                 line.length() > 0 && 
                 std::isdigit(line[0])));
    }

public:
    TcpdumpPayloadExtractor(const std::string& input, const std::string& output) 
        : inputFile(input), outputFile(output) {}
    
    bool extract() {
        std::ifstream inFile(inputFile);
        if (!inFile.is_open()) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return false;
        }
        
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot open output file: " << outputFile << std::endl;
            return false;
        }
        
        std::string line;
        int packetCount = 0;
        int filteredPacketCount = 0;
        bool inPayload = false;
        std::string currentPacketData;
        
        while (std::getline(inFile, line)) {
            // Check if this is a new packet header
            if (isPacketHeader(line) && !line.empty() && line[0] != '\t' && line[0] != ' ') {
                
                // Process the previous packet if we collected data
                if (inPayload && !currentPacketData.empty()) {
                    // Check if packet starts with "abba" and ends with "ebbe000000000000"
                    std::string startPattern = "abba";
                    std::string endPattern = "ebbe000000000000";
                    
                    bool startsCorrect = currentPacketData.length() >= startPattern.length() &&
                                        currentPacketData.substr(0, startPattern.length()) == startPattern;
                    bool endsCorrect = currentPacketData.length() >= endPattern.length() &&
                                      currentPacketData.substr(currentPacketData.length() - endPattern.length()) == endPattern;
                    
                    if (startsCorrect && endsCorrect) {
                        filteredPacketCount++;
                        // outFile << "--- Packet " << filteredPacketCount << " ---\n";
                        outFile << currentPacketData << std::endl;
                        // outFile << "--- End of packet " << filteredPacketCount << " ---\n\n";
                    }
                    
                    currentPacketData.clear();
                }
                
                packetCount++;
                inPayload = false;
                continue;
            }
            
            // Check if this is a payload line
            if (isPayloadLine(line)) {
                inPayload = true;
                std::string stripped = stripHexOffset(line);
                std::string hexData = cleanHexData(stripped);
                
                if (!hexData.empty()) {
                    currentPacketData += hexData;
                }
            }
        }
        
        // Process the last packet
        if (inPayload && !currentPacketData.empty()) {
            std::string startPattern = "abba";
            std::string endPattern = "ebbe000000000000";
            
            bool startsCorrect = currentPacketData.length() >= startPattern.length() &&
                                currentPacketData.substr(0, startPattern.length()) == startPattern;
            bool endsCorrect = currentPacketData.length() >= endPattern.length() &&
                              currentPacketData.substr(currentPacketData.length() - endPattern.length()) == endPattern;
            
            if (startsCorrect && endsCorrect) {
                filteredPacketCount++;
                // outFile << "--- Packet " << filteredPacketCount << " ---\n";
                outFile << currentPacketData << std::endl;
                // outFile << "--- End of packet " << filteredPacketCount << " ---\n";
            }
        }
        
        inFile.close();
        outFile.close();
        
        std::cout << "Extraction complete!" << std::endl;
        std::cout << "Total packets processed: " << packetCount << std::endl;
        std::cout << "Filtered packets (matching criteria): " << filteredPacketCount << std::endl;
        std::cout << "Output written to: " << outputFile << std::endl;
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input_tcpdump_file> <output_file>" << std::endl;
        std::cout << "\nExample:" << std::endl;
        std::cout << "  tcpdump -X -r capture.pcap > tcpdump_output.txt" << std::endl;
        std::cout << "  " << argv[0] << " tcpdump_output.txt payload.txt" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    
    TcpdumpPayloadExtractor extractor(inputFile, outputFile);
    
    if (!extractor.extract()) {
        return 1;
    }
    
    return 0;
}
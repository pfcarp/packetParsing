// #include <iostream>
// #include <fstream>
// #include <string>
// #include <vector>
// #include <algorithm>

// std::string flipEndianness(const std::string& chunk) {
//     // Take only the first 8 characters (4 bytes) as mentioned
//     std::string first8 = chunk.substr(0, 8);
    
//     // Flip endianness by swapping pairs of characters (bytes)
//     std::string flipped = "";
//     for (int i = 6; i >= 0; i -= 2) {
//         if (i + 1 < first8.length()) {
//             flipped += first8.substr(i, 2);
//         }
//     }
    
//     return flipped;

//     // return first8;
// }

// std::string processHexData(const std::string& data) {
//     std::string processed = data;
    
//     // Remove "abba" from beginning if present
//     if (processed.length() >= 4 && processed.substr(0, 4) == "abba") {
//         processed = processed.substr(4);
//     }
    
//     // Remove "ebbe" followed by zeros from the end
//     size_t ebbePos = processed.rfind("ebbe");
//     if (ebbePos != std::string::npos) {
//         // Check if everything after "ebbe" is zeros
//         bool allZeros = true;
//         for (size_t i = ebbePos + 4; i < processed.length(); i++) {
//             if (processed[i] != '0') {
//                 allZeros = false;
//                 break;
//             }
//         }
//         if (allZeros) {
//             processed = processed.substr(0, ebbePos);
//         }
//     }
    
//     std::string result = "";
    
//     // Process in 16-character chunks
//     for (size_t i = 0; i < processed.length(); i += 16) {
//         std::string chunk = processed.substr(i, 16);
        
//         // Pad with zeros if chunk is less than 16 characters
//         while (chunk.length() < 16) {
//             chunk += "0";
//         }
        
//         // Skip chunks that are all F's
//         bool allF = true;
//         for (char c : chunk) {
//             if (c != 'f' && c != '0') {
//                 allF = false;
//                 break;
//             }
//         }
        
//         if (!allF) {
//             std::string flipped = flipEndianness(chunk);
//             result += "0x" + flipped + "\n";
//         }
//     }
    
//     return result;
// }

// int main(int argc, char* argv[]) {
//     if (argc != 2) {
//         std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
//         return 1;
//     }
    
//     std::ifstream inputFile(argv[1]);
//     if (!inputFile.is_open()) {
//         std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
//         return 1;
//     }
    
//     std::string line;
//     std::string concatenatedData = "";
    
//     // Read all lines and concatenate them
//     while (std::getline(inputFile, line)) {
//         // Remove any whitespace and newlines
//         line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
//         concatenatedData += line;
//     }
    
//     inputFile.close();
    
//     // Convert to lowercase for consistent processing
//     std::transform(concatenatedData.begin(), concatenatedData.end(), concatenatedData.begin(), ::tolower);
    
//     // Split by "abba" to handle multiple entries
//     std::vector<std::string> entries;
//     size_t start = 0;
//     size_t abbaPos = concatenatedData.find("abba", start);
    
//     if (abbaPos == 0) {
//         // First entry starts with "abba"
//         start = abbaPos;
//     }
    
//     while (abbaPos != std::string::npos) {
//         if (start < abbaPos) {
//             entries.push_back(concatenatedData.substr(start, abbaPos - start));
//         }
        
//         size_t nextAbba = concatenatedData.find("abba", abbaPos + 4);
//         if (nextAbba != std::string::npos) {
//             entries.push_back(concatenatedData.substr(abbaPos, nextAbba - abbaPos));
//             start = nextAbba;
//             abbaPos = nextAbba;
//         } else {
//             // Last entry
//             entries.push_back(concatenatedData.substr(abbaPos));
//             break;
//         }
//     }
    
//     // If no "abba" found, treat entire data as one entry
//     if (entries.empty()) {
//         entries.push_back(concatenatedData);
//     }
    
//     // Process each entry
//     for (const std::string& entry : entries) {
//         std::string result = processHexData(entry);
//         if (!result.empty()) {
//             std::cout << result;
//             if (!result.empty() && result.back() != '\n') {
//                 std::cout << std::endl;
//             }
//         }
//     }
    
//     return 0;
// }


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

std::string flipEndianness(const std::string& chunk) {
    // Take only the first 8 characters (4 bytes) as mentioned
    std::string first8 = chunk.substr(0, 8);
    
    // Flip endianness by swapping pairs of characters (bytes)
    std::string flipped = "";
    for (int i = 6; i >= 0; i -= 2) {
        if (i + 1 < first8.length()) {
            flipped += first8.substr(i, 2);
        }
    }
    
    return flipped;
}

std::string processHexData(const std::string& data) {
    std::string processed = data;
    
    // Remove "abba" from beginning if present
    if (processed.length() >= 4 && processed.substr(0, 4) == "abba") {
        processed = processed.substr(4);
    }
    
    // Remove "ebbe" followed by zeros from the end
    size_t ebbePos = processed.rfind("ebbe");
    if (ebbePos != std::string::npos) {
        // Check if everything after "ebbe" is zeros
        bool allZeros = true;
        for (size_t i = ebbePos + 4; i < processed.length(); i++) {
            if (processed[i] != '0') {
                allZeros = false;
                break;
            }
        }
        if (allZeros) {
            processed = processed.substr(0, ebbePos);
        }
    }
    
    std::string result = "";
    
    // Process in 16-character chunks
    for (size_t i = 0; i < processed.length(); i += 16) {
        std::string chunk = processed.substr(i, 16);
        
        // If chunk is less than 16 characters and we're not at the end,
        // it will naturally grab characters from the next line since
        // we already concatenated all lines into processed string
        // Only pad with zeros if we're at the very end and still short
        if (chunk.length() < 16 && i + 16 >= processed.length()) {
            while (chunk.length() < 16) {
                chunk += "0";
            }
        }
        
        // Skip chunks that are all F's
        bool allF = true;
        for (char c : chunk) {
            if (c != 'f' && c != 'F') {
                allF = false;
                break;
            }
        }
        
        if (!allF) {
            std::string flipped = flipEndianness(chunk);
            if (flipped=="00000000" || flipped=="80000000") continue; // Skip null or async packets
            result += "0x" + flipped + "\n";
        }
    }
    
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
        return 1;
    }
    
    std::string line;
    std::string concatenatedData = "";
    
    // Read all lines and concatenate them
    while (std::getline(inputFile, line)) {
        // Remove any whitespace and newlines
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        concatenatedData += line;
    }
    
    inputFile.close();
    
    // Convert to lowercase for consistent processing
    std::transform(concatenatedData.begin(), concatenatedData.end(), concatenatedData.begin(), ::tolower);
    
    // Split by "abba" to handle multiple entries
    std::vector<std::string> entries;
    size_t start = 0;
    size_t abbaPos = concatenatedData.find("abba", start);
    
    if (abbaPos == 0) {
        // First entry starts with "abba"
        start = abbaPos;
    }
    
    while (abbaPos != std::string::npos) {
        if (start < abbaPos) {
            entries.push_back(concatenatedData.substr(start, abbaPos - start));
        }
        
        size_t nextAbba = concatenatedData.find("abba", abbaPos + 4);
        if (nextAbba != std::string::npos) {
            entries.push_back(concatenatedData.substr(abbaPos, nextAbba - abbaPos));
            start = nextAbba;
            abbaPos = nextAbba;
        } else {
            // Last entry
            entries.push_back(concatenatedData.substr(abbaPos));
            break;
        }
    }
    
    // If no "abba" found, treat entire data as one entry
    if (entries.empty()) {
        entries.push_back(concatenatedData);
    }
    
    // Process each entry
    for (const std::string& entry : entries) {
        std::string result = processHexData(entry);
        if (!result.empty()) {
            std::cout << result;
            if (!result.empty() && result.back() != '\n') {
                std::cout << std::endl;
            }
        }
    }
    
    return 0;
}
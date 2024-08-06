#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

const std::string START_BYTE = "0bb0";
const std::string END_BYTE = "045700";
const uint CHUNK_SIZE = 16;

enum class PacketType
{
    ADDRESS,
    METADATA
};


std::ostream& operator<<(std::ostream& os, const PacketType& packetType)
{
    switch(packetType)
    {
        case PacketType::ADDRESS:
            os << "ADDRESS";
            break;
        case PacketType::METADATA:
            os << "METADATA";
            break;
    }
    return os;
}

std::string reverse_byte_order(const std::string &input)
{
    std::string result;
    result.reserve(input.length());
    for (int i = input.length() - 2; i >= 0; i -= 2)
    {
        result.append(input, i, 2);
    }
    return result;
}

size_t process_packet(const std::string &data, std::string &previous_chunk)
{
    PacketType previous_packetType;
    size_t valid_pairs = 0;
    PacketType packetType = PacketType::METADATA;
    for (size_t i = START_BYTE.length(); i < data.length() - END_BYTE.length(); i += CHUNK_SIZE)
    {
        std::string chunk = data.substr(i, CHUNK_SIZE);

        if (chunk == std::string(CHUNK_SIZE, '0'))
        {
            continue; // Skip zero chunks
        }

        //print chunk
        //std::cout << chunk << std::endl;
        //std::cout << "previous_chunk: " << previous_chunk << std::endl;
        
        // if(chunk == previous_chunk)
        // {
        //     std::cout << "Duplicate chunk found: " << chunk << std::endl;
        //     previous_chunk = "";
        //     continue;
        // }

        packetType = (packetType == PacketType::ADDRESS && (reverse_byte_order(chunk)).substr(0, 2)!="00") ? PacketType::METADATA : (packetType == PacketType::METADATA && (reverse_byte_order(chunk)).substr(0, 2)=="00") ? PacketType::ADDRESS : PacketType::METADATA;
        
        if(packetType == previous_packetType)
        {
            std::cout << "A glitch detected" << std::endl;
        }
        
        std::cout << packetType << " " << reverse_byte_order(chunk) << std::endl;

        //get the first 2 chars of chunks
        std::string first_two_chars = chunk.substr(0, 2);

        
        //assign chunk to previous_chunk for next iteration
        previous_chunk = chunk;
        previous_packetType = packetType;
    }

    std::cout << "---" << std::endl;
    return valid_pairs;
}

std::pair<size_t, size_t> process_all_packets(const std::string &data)
{
    size_t packet_count = 0;
    size_t total_valid_pairs = 0;
    size_t current = 0;
    std::string previous_chunk="";
    while (true)
    {
        size_t packet_start = data.find("0bb0", current);
        if (packet_start == std::string::npos)
            break;

        size_t packet_end = data.find("045700", packet_start);
        if (packet_end == std::string::npos)
            break;

        packet_end += END_BYTE.length(); // Include the "045700" at the end

        if (packet_end - packet_start == 2042)
        {
        
            total_valid_pairs += process_packet(data.substr(packet_start, 2042), previous_chunk);
            //std::cout << "leftover: " << previous_chunk << std::endl;
            packet_count++;
        }

        current = packet_end;
    }

    return {packet_count, total_valid_pairs};
}

int main(int argc, char *argv[])
{
    std::string input_data;

    if (argc > 1)
    {
        std::ifstream file(argv[1]);
        if (!file)
        {
            std::cerr << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
        input_data = std::string(std::istreambuf_iterator<char>(file), {});
    }
    else
    {
        input_data = std::string(std::istreambuf_iterator<char>(std::cin), {});
    }

    // Remove all whitespace from input_data
    input_data.erase(std::remove_if(input_data.begin(), input_data.end(), ::isspace), input_data.end());

    auto [total_packets, total_valid_pairs] = process_all_packets(input_data);

    std::cout << "Total packets processed: " << total_packets << std::endl;
    std::cout << "Total valid address/metadata pairs found: " << total_valid_pairs << std::endl;

    return 0;
}
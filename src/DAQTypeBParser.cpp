// DAQTypeBParser.cpp
#include "DAQTypeBParser.h"
#include "PacketHeader.h"
#include <cstring>

template<typename T>
std::vector<std::vector<T>> splitVector(const std::vector<T>& input, size_t chunkSize) {
    std::vector<std::vector<T>> result;
    size_t total = input.size();
    for (size_t i = 0; i < total; i += chunkSize) {
        size_t end = std::min(i + chunkSize, total);
        result.emplace_back(input.begin() + i, input.begin() + end);
    }
    return result;
}

PacketGroup DAQTypeBParser::parseHeader(const char* data, size_t size) {
    PacketGroup group;
    std::vector<char> raw(data, data + 128); // bigger header
    std::vector<std::vector<char>> splitted = splitVector(raw,32);
    for (int a =0; a<4; a++) {
    	PacketHeader header;
    }  
    return group;

}

std::vector<short> DAQTypeBParser::parseData(const char* data, size_t size, const PacketHeader& header) {
    std::vector<short> result;
    const short* ptr = reinterpret_cast<const short*>(data);
    int n_samples = (header.data_length - 128) / sizeof(short);

    for (int i = 0; i < n_samples; ++i) {
        result.push_back(ptr[i]);
    }

    return result;
}

//size_t DAQTypeBParser::eventSize(const PacketHeader& header) const {
//    return static_cast<size_t>(header.data_length);
//}


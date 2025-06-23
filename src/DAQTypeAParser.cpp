// DAQTypeAParser.cpp
#include "DAQTypeAParser.h"
#include <cstring>

PacketGroup DAQTypeAParser::parseHeader(const char* data, size_t size) {
    PacketGroup group;
    PacketHeader header;
    std::vector<char> raw(data, data + 32);
    header.raw_bytes = raw;

    header.data_length = static_cast<int>((unsigned char)data[0] | (data[1] << 8)); // 예시
    header.channel = static_cast<int>(data[16]);
    header.tcb_trigger_number = static_cast<int>(data[7]); // 예시
    header.tcb_trigger_time = static_cast<unsigned long long>(data[12]) * 1000; // 예시
    
    group.single_header = header;

    return group;
}

std::vector<short> DAQTypeAParser::parseData(const char* data, size_t size, const PacketHeader& header) {
    std::vector<short> result;
    const short* ptr = reinterpret_cast<const short*>(data);
    int n_samples = (header.data_length - 32) / sizeof(short);

    for (int i = 0; i < n_samples; ++i) {
        result.push_back(ptr[i]);
    }

    return result;
}

size_t DAQTypeAParser::eventSize(const PacketHeader& header) const {
    return static_cast<size_t>(header.data_length);
}


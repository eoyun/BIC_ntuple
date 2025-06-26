// DAQTypeAParser.cpp
#include "DAQTypeAParser.h"
#include <cstring>

PacketGroup DAQTypeAParser::parseHeader(const char* data, size_t size) {
    PacketGroup group;
    PacketHeader header;
    std::vector<char> raw(data, data + 32);
    header.raw_bytes = raw;


    int data_length = 0;
    int tcb_trigger_number = 0;
    unsigned long long tcb_trigger_time = 0;
    int channel = 0;

    for (int a=0; a<4; a++) data_length += ((int)(raw.at(a) & 0xFF) << 8*a);
    
    for (int a=0; a<4; a++) tcb_trigger_number += ((int)(raw.at(a+7) & 0xFF) << 8*a);
    int tcb_trigger_fine_time = ((int)raw.at(11) & 0xFF);
    int tcb_trigger_coarse_time = 0;
    for (int a=0; a<3; a++) tcb_trigger_coarse_time += ((int)(raw.at(a+12) & 0xFF) << 8*a);
    tcb_trigger_time = (tcb_trigger_fine_time * 8) + (tcb_trigger_coarse_time * 1000);
    
    //int mid = ((int)header[15] & 0xFF);
    channel = ((int)raw.at(16) & 0xFF);
    if (data_length <10000 && data_length>0)
        header.data_length = data_length;
    else header.data_length = 512;
	
    header.tcb_trigger_time = tcb_trigger_time;
    header.tcb_trigger_number = tcb_trigger_number;
    header.channel = channel;
    group.single_header = header;

    return group;
}

std::vector<short> DAQTypeAParser::parseData(const char* data, size_t size, const PacketHeader& header) {
    std::vector<short> result;
    const short* ptr = reinterpret_cast<const short*>(data+32);
    int n_samples = (header.data_length - 32) / sizeof(short);

    for (int i = 0; i < n_samples; ++i) {
        result.push_back(ptr[i]);
    }

    return result;
}

size_t DAQTypeAParser::eventSize(const PacketHeader& header) const {
    return static_cast<size_t>(header.data_length);
}


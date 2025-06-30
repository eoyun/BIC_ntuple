// DAQTypeBParser.cpp
#include "DAQTypeBParser.h"
#include "PacketHeader.h"
#include <cstring>
#include <iostream>
#include "functions.h"

PacketGroup DAQTypeBParser::parseHeader(const char* data, size_t size) {
    PacketGroup group;
    std::vector<char> raw(data, data + 128); // bigger header
    std::vector<std::vector<char>> splitted = splitVector(raw,32);
    for (int i =0; i<4; i++) {
    	PacketHeader header;
    	header.raw_bytes = splitted.at(i);


    	int data_length=0;
   	int tcb_trigger_number=0;
	unsigned long long tcb_trigger_time = 0;
	int channel = 0;
	
	for (int a=0; a<4; a++) data_length += ((unsigned int)(splitted.at(i).at(a) & 0xFF) << 8*a);
	
	for (int a=0; a<4; a++) tcb_trigger_number += ((unsigned int)(splitted.at(i).at(a+7) & 0xFF) << 8*a);
	int tcb_trigger_fine_time = ((unsigned int)splitted.at(i).at(11) & 0xFF);
	int tcb_trigger_coarse_time = 0;
	for (int a=0; a<3; a++) tcb_trigger_coarse_time += ((unsigned int)(splitted.at(i).at(a+12) & 0xFF) << 8*a);
	tcb_trigger_time = (unsigned long long)tcb_trigger_fine_time * 8 + (unsigned long long)tcb_trigger_coarse_time * 1000;
	
	//int mid = ((int)header[15] & 0xFF);
	channel = ((int)splitted.at(i).at(16) & 0xFF);
	header.data_length = data_length;
	header.tcb_trigger_time = tcb_trigger_time;
	header.tcb_trigger_number = tcb_trigger_number;
	header.channel = channel;

	group.multi_headers.push_back(header);
    }  
    //for (int i = 0;i<32;i++){
    //    for (int j =0;j<4;j++){
    //	    std::cout<<std::hex<<static_cast<unsigned int>(static_cast<unsigned char>(raw.at(i*4 + j)))<<" | ";
    //    }
    //    std::cout<<" |||| ";
    //    for (int j =0;j<4;j++){
    //	    std::cout<<std::hex<<static_cast<unsigned int>(static_cast<unsigned char>(splitted.at(j).at(i)))<<" | ";
    //    }
    //    std::cout<<std::endl;
    //}
    group.is_multi = true;
    return group;

}

std::vector<short> DAQTypeBParser::parseData(const char* data, size_t size, const PacketHeader& header) {
    std::vector<short> result;
    const short* ptr = reinterpret_cast<const short*>(data + 128);
    int n_samples = (header.data_length * 4 - 128) / sizeof(short);

    for (int i = 0; i < n_samples; ++i) {
        result.push_back(ptr[i]);
    }

    return result;
}

size_t DAQTypeBParser::eventSize(const PacketHeader& header) const {
    return static_cast<size_t>(header.data_length * 4);
}


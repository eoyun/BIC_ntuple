// DAQTypeAParser.cpp
#include "DAQTypeCParser.h"
#include <cstring>
#include <iostream>
#include "APixstruct.h"
#include "functions.h"
#include <vector>

PacketGroup DAQTypeCParser::parseHeader(const char* data, size_t size) {
    PacketGroup group;
    APixstruct apix;
    
    std::vector<char> raw(data, data + 16);

    unsigned long long tcb_trigger_time = 0;

    
    int tcb_trigger_fine_time = ((unsigned int)raw.at(7) & 0xFF);
    int tcb_trigger_coarse_time = 0;
    for (int a=0; a<6; a++) tcb_trigger_coarse_time += ((unsigned int)(raw.at(a+8) & 0xFF) << 8*a);
    tcb_trigger_time = (unsigned long long)tcb_trigger_fine_time * 8 + (unsigned long long)tcb_trigger_coarse_time * 1000;
    //std::cout<<"| "<<tcb_trigger_coarse_time<<" | "<<tcb_trigger_fine_time<<" | "<<tcb_trigger_time<<" |"<<std::endl;
    
    //int mid = ((int)header[15] & 0xFF);
    int idx = 16;
    uint8_t tmp = 0x00;
    std::vector<unsigned short> Vaid;
    std::vector<unsigned short> Vpay;
    std::vector<unsigned short> VisCol;
    std::vector<unsigned short> Vch;
    std::vector<unsigned short> Vts;
    std::vector<unsigned short> Vtot;
    while(true){
	uint8_t value = static_cast<uint8_t> (data[idx]);
	uint8_t value_next = static_cast<uint8_t> (data[idx+1]);
	bool flag = true;
        if (value == 0xBC){
	    if (tmp == 0xBC  || value_next == 0xBC) flag = false;
	} 
	else if (value == 0xFF && value_next == 0xFF) break;
	if (flag){
	    unsigned long value = 0;
	    //std::cout<<static_cast<int>(static_cast<uint8_t>(data[idx]))<<" | "<<static_cast<uint8_t>(data[idx+1])<<" | "<<static_cast<uint8_t>(data[idx+2])<<" | "<<static_cast<uint8_t>(data[idx+3])<<" | "<<static_cast<uint8_t>(data[idx+4])<<" | "<<std::endl;

	    value = static_cast<unsigned long>(static_cast<unsigned char>(data[idx])) 
		    |(static_cast<unsigned long>(static_cast<unsigned char>(data[idx + 1])) << 8)
		    |(static_cast<unsigned long>(static_cast<unsigned char>(data[idx + 2])) << 16)
		    |(static_cast<unsigned long>(static_cast<unsigned char>(data[idx + 3])) << 24)
		    |(static_cast<unsigned long>(static_cast<unsigned char>(data[idx + 4])) << 32);
	    //std::cout<<std::hex<<value<<std::endl;
	    idx += 5;
	    unsigned short aid  = ((value & 0x000000001F) >> 0 );
	    unsigned short pay  = ((value & 0x00000000E0) >> 5 );
	    unsigned short isCol= ((value & 0x0000000100) >> 8 );
	    unsigned short rsv1 = ((value & 0x0000000200) >> 9 );
	    unsigned short ch   = ((value & 0x000000FC00) >> 10);
	    unsigned short ts   = ((value & 0x0000FF0000) >> 16);
	    unsigned short rsv2 = ((value & 0x000F000000) >> 24);
	    unsigned short msb  = ((value & 0x00F0000000) >> 28);
	    unsigned short lsb  = ((value & 0xFF00000000) >> 32);

	    unsigned short faid = Flip( aid, 5 );
	    unsigned short fpay = Flip( pay, 3 );
	    unsigned short fch  = Flip( ch , 6 );
	    unsigned short fts  = Flip( ts , 8 );
	    unsigned short fmsb = Flip( msb, 4 );
	    unsigned short flsb = Flip( lsb, 8 );

	    unsigned short tot  = flsb;
	    tot += (fmsb << 8);
	    if ( rsv1 != 0 || rsv2 != 0) continue;
	    if ( fch > 34) continue;
	    Vaid.push_back(faid); 
	    Vpay.push_back(fpay); 
	    VisCol.push_back(isCol); 
	    Vch.push_back(fch);
	    Vts.push_back(fts);
	    Vtot.push_back(tot); 
	    //const unsigned char* d = reinterpret_cast<const unsigned char*>(data + idx);
	    //idx += 5;

            //unsigned short aid   = (d[0] >> 3) & 0x1F;             
            //unsigned short pay   = (d[0]) & 0x07;                  
            //unsigned short isCol = (d[1] >> 7) & 0x01;             
            //unsigned short rsv1  = (d[1] >> 6) & 0x01;             
            //unsigned short ch    = (d[1]) & 0x3F;                  
            //unsigned short ts    = d[2];                           
            //unsigned short rsv2  = (d[3] >> 0) & 0x0F;             
            //unsigned short msb   = (d[3] >> 4) & 0x0F;             
            //unsigned short lsb   = d[4];                           
	    //unsigned short tot = lsb;
	    //tot += (msb << 8 );

	    //std::cout<<"aid : "<<faid << " | pay : "<<fpay <<" | isCol : "<<isCol<<" | rsv1 : "<<rsv1<<" | ch : "<<fch<<" | ts : "<<fts<<" | rsv2 : "<<rsv2<<" | tot : "<<tot<<" | "<<std::endl;
	}
	else {
	    idx ++;
	    tmp = value;
	} 

    }

    apix.tcb_trigger_time = tcb_trigger_time;
    apix.aid = Vaid;
    apix.isCol = VisCol;
    apix.ch = Vch;
    apix.ts = Vts;
    apix.tot = Vtot;

    group.apix_struct = apix;

    group.is_apix = true;

    return group;
}
std::vector<short> DAQTypeCParser::parseData(const char* data, size_t size, const PacketHeader& header, int channel) {
    std::vector<short> dummy;
    return dummy;
}

size_t DAQTypeCParser::eventSize(const PacketHeader& header) const {
    return 0;
}


size_t DAQTypeCParser::eventSize(const char* data) const {

    size_t evt_size;
    int i = 4;
    uint32_t value;
    while (true){
    	value = static_cast<uint8_t>( data[i]) | static_cast<uint8_t>(data[i+1])<<8| static_cast<uint8_t>(data[i+2])<<16|static_cast<uint8_t>(data[i+3])<<24;
	if (i> 1000)break;// for safety
	if (value == 0xFFFFFFFF) break;
	i++;
    }
    //std::cout<<value<<" | "<<i<<std::endl;
    evt_size = static_cast<size_t> (i);
    return evt_size;
}

size_t DAQTypeCParser::findEnd (size_t filesize, const char* data) const{
    size_t i = filesize - 4;
    uint32_t value;
    while (true){
    	value = static_cast<uint8_t>( data[i]) | static_cast<uint8_t>(data[i+1])<<8| static_cast<uint8_t>(data[i+2])<<16|static_cast<uint8_t>(data[i+3])<<24;
	if (value == 0xFFFFFFFF) break;
	i = i - 1;
    }
    return i;

}

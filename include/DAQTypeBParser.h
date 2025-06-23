#ifndef DAQ_TYPE_B_PARSER_H
#define DAQ_TYPE_B_PARSER_H

#include "AbstractDAQParser.h"

// TypeB = NKFADC500

class DAQTypeBParser : public AbstractDAQParser {
public:
    PacketGroup parseHeader(const char* data, size_t size) override;
    std::vector<short> parseData(const char* data, size_t size, const PacketHeader& header) override;
};

#endif


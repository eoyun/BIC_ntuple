#ifndef DAQ_TYPE_D_PARSER_H
#define DAQ_TYPE_D_PARSER_H

#include "AbstractDAQParser.h"

// TypeD : bic

class DAQTypeDParser : public AbstractDAQParser {
public:
    PacketGroup parseHeader(const char* data, size_t size) override;
    std::vector<short> parseData(const char* data, size_t size, const PacketHeader& header, int channel) override;
    size_t eventSize(const PacketHeader& header) const override;
};

#endif


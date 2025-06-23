#ifndef DAQ_TYPE_A_PARSER_H
#define DAQ_TYPE_A_PARSER_H

#include "AbstractDAQParser.h"

// TypeA : jbnu & bic

class DAQTypeAParser : public AbstractDAQParser {
public:
    PacketGroup parseHeader(const char* data, size_t size) override;
    std::vector<short> parseData(const char* data, size_t size, const PacketHeader& header) override;
    size_t eventSize(const PacketHeader& header) const override;
};

#endif


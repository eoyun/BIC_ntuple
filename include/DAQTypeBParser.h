#ifndef DAQ_TYPE_B_PARSER_H
#define DAQ_TYPE_B_PARSER_H

#include "AbstractDAQParser.h"

// TypeB = NKFADC500

class DAQTypeBParser : public AbstractDAQParser {
public:
    PacketGroup parseHeader(const char* data, size_t size) override;
    std::vector<short> parseData(const char* data, size_t size, const PacketHeader& header, int channel ) override;
    size_t eventSize(const PacketHeader& header) const override;  // ⬅️ 꼭 구현해야 함
    size_t eventSize(const char* data) const override;
    size_t findEnd(size_t filesize,const char* data) const override;
};

#endif


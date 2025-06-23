#ifndef ABSTRACT_DAQ_PARSER_H
#define ABSTRACT_DAQ_PARSER_H

#include "PacketHeader.h"
#include <vector>

class AbstractDAQParser {
public:
    virtual ~AbstractDAQParser() = default;

    // Header를 파싱해 구조체로 리턴
    virtual PacketGroup parseHeader(const char* data, size_t size) = 0;

    // Header 이후 데이터를 파싱해서 vector<short>로 리턴
    virtual std::vector<short> parseData(const char* data, size_t size, const PacketHeader& header) = 0;

    // 한 이벤트당 전체 길이 (header + data)
    virtual size_t eventSize(const PacketHeader& header) const = 0;
};

#endif


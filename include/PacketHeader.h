#ifndef PACKET_HEADER_H
#define PACKET_HEADER_H

#include <vector>
#include <cstdint>

struct PacketHeader {
    int data_length = 0;                       // 전체 데이터 길이 (header + payload 포함 or payload만, 파서 기준에 따라)
    unsigned long long tcb_trigger_time = 0;            // 트리거 시간 (fine + coarse)
    int tcb_trigger_number = -1;              // 트리거 번호
    int channel = -1;                         // 채널 번호 (단일 or 4채널이면 -1로 처리 가능)
    
    std::vector<char> raw_bytes;              // 원시 헤더 데이터 저장 (디버깅 및 포맷별 파싱용)

    // 필요하다면 추가 필드
    // int run_number = 0;
    // int board_id = 0;
};

struct PacketGroup {
    // 단일 이벤트용
    PacketHeader single_header;

    // 멀티채널 이벤트용 (4개 채널 등)
    std::vector<PacketHeader> multi_headers;

    // 플래그로 구분
    bool is_multi = false;
};

#endif // PACKET_HEADER_H

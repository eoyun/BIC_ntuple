#ifndef GETFILE_H
#define GETFILE_H

#include <iostream>
#include <filesystem>
#include <vector>
#include <cstddef>  // for size_t
#include "PacketHeader.h"

namespace fs = std::filesystem;


std::vector<int> getMID (int runnum);
class getFile {
public:
	getFile(const int runnum, const int MID);
	~getFile();
	
	const char* data() const;       // 매핑된 데이터의 시작 주소
	const char* cursor() const;       // 매핑된 데이터의 시작 주소
	size_t size() const;            // 매핑된 데이터 크기
	bool isValid() const;       	// 유효한 매핑 여부
	PacketHeader getHeader();
	std::vector<short> getData();

private:
	int mid_;
	char* mapped_;       // 매핑된 주소
	int fd_;             // 파일 디스크립터
	size_t filesize_;    // 파일 크기
	bool valid_;
	char* cursor_;
	PacketHeader header_;

	void mapFile(const std::string& filename);
	void unmapFile();
};

#endif

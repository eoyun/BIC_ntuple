#ifndef GETFILE_H
#define GETFILE_H

#include <iostream>
#include <filesystem>
#include <vector>
#include <cstddef>  // for size_t
#include "PacketHeader.h"
#include "AbstractDAQParser.h"

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
	PacketGroup getNextHeader();
	std::vector<short> getNextData();
	PacketGroup getHeader(const char* pointer_);
	std::vector<short> getData(const char* pointer_, PacketGroup group);
	int cursor_int() {return read_data;}
	bool isMulti() const;
	bool isEnd();
	int MID() {return mid_;}

private:
	int mid_;
	char* mapped_;       // 매핑된 주소
	int fd_;             // 파일 디스크립터
	size_t filesize_;    // 파일 크기
	bool valid_;
	char* cursor_;
	PacketGroup group_;
	int read_data = 0;
	bool multi_ = false;
	bool end_ = false;

	void mapFile(const std::string& filename);
	void unmapFile();

	std::unique_ptr<AbstractDAQParser> parser_;
};

#endif

#include "getFile.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>
#include <string>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/mman.h>    // mmap, munmap
#include <sys/stat.h>    // fstat
#include "DAQTypeAParser.h"
#include "DAQTypeBParser.h"

namespace fs = std::filesystem;

std::vector<int> getMID (int runnum){
	std::vector<int> MIDs;
	std::string path = "/pnfs/knu.ac.kr/data/cms/store/user/yeo/KEKTB202503/Run_" +std::to_string(runnum);
	if (!fs::exists(path)) {
		std::cerr << "Path does not exist: " << path << std::endl;
		return MIDs;
	}

	for (const auto& entry : fs::recursive_directory_iterator(path)) {
		if (entry.is_directory()) {
			std::cout << "Directory: " << entry.path() << std::endl;
			std::regex pattern(path+"/Run_"+std::to_string(runnum)+"_MID_(\\d+)");
			std::smatch match;
			std::string sub_path = entry.path();
			if (std::regex_search(sub_path,match,pattern)){
				int MID = std::stoi(match[1]);
				std::cout<<"MID is "<<MID<<std::endl;
				MIDs.push_back(MID);
			}
			else std::cerr << "Pattern not matched!" << std::endl;
		}

	}
	return MIDs;

}

getFile::getFile(const int runnum, const int MID)
	: fd_(-1),filesize_(0),mapped_(nullptr),valid_(false)
{
	std::string filename;
	if (MID<20){ 
		filename = "/pnfs/knu.ac.kr/data/cms/store/user/yeo/KEKTB202503/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/FADCData_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
		parser_ = std::make_unique<DAQTypeBParser>();
		multi_ = true;
	}
	else if (MID<40) {
		filename = "/pnfs/knu.ac.kr/data/cms/store/user/yeo/KEKTB202503/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/jbnu_daq_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
		parser_ = std::make_unique<DAQTypeAParser>();
	}
	else if (MID<50){
	       filename = "/pnfs/knu.ac.kr/data/cms/store/user/yeo/KEKTB202503/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/bic_daq_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
		parser_ = std::make_unique<DAQTypeAParser>();
	}
	mapFile(filename);
	mid_ = MID;
}

getFile::~getFile() {
	unmapFile();
}

void getFile::mapFile(const std::string& filename) {
	fd_ = open(filename.c_str(), O_RDONLY);
	if (fd_ < 0) {
		perror("open");
		return;
	}

	struct stat sb;
	if (fstat(fd_, &sb) == -1) {
		perror("fstat");
		close(fd_);
		return;
	}

	filesize_ = sb.st_size;
	mapped_ = static_cast<char*>(mmap(nullptr, filesize_, PROT_READ, MAP_PRIVATE, fd_, 0));
	if (mapped_ == MAP_FAILED) {
		perror("mmap");
		mapped_ = nullptr;
		close(fd_);
		return;
	}
	cursor_ = mapped_;

	valid_ = true;
}

void getFile::unmapFile() {
	if (mapped_ && filesize_ > 0) {
		munmap(mapped_, filesize_);
	}
	if (fd_ >= 0) {
		close(fd_);
	}
}

const char* getFile::data() const {
	return mapped_;
}

const char* getFile::cursor()  const {
	//std::cout<<"test : "<<cursor_<<std::endl;
	return cursor_;
}

size_t getFile::size() const {
	return filesize_;
}

bool getFile::isValid() const {
	return valid_;
}

bool getFile::isMulti() const {
	return multi_;
}

bool getFile::isEnd() {
	if (read_data >= filesize_) end_ = true;	
	return end_;

}

PacketGroup getFile::getNextHeader(){
	PacketGroup group = parser_->parseHeader(cursor_, filesize_);
	group_ = group;
	if (multi_){ 
	       	cursor_ += parser_->eventSize(group_.multi_headers.at(1));
	       	read_data += (int) parser_->eventSize(group_.multi_headers.at(1));
	}
	else { 
		cursor_ += parser_->eventSize(group_.single_header); // 다음 이벤트로 이동
		read_data += (int) parser_->eventSize(group_.single_header); // 다음 이벤트로 이동
	}
	//if (multi_) std::cout<<"header info : MID -> "<<mid_<<" channel -> "<<group.multi_headers.at(1).channel<<" data_length -> "<<group.multi_headers.at(1).data_length<<" trigger number -> "<<group.multi_headers.at(1).tcb_trigger_number<<std::endl;
	//else std::cout<<"header info : MID -> "<<mid_<<" channel -> "<<group.single_header.channel<<" data_length -> "<<group.single_header.data_length<<" trigger number -> "<<group.single_header.tcb_trigger_number<<std::endl;
    	return group;
} 

std::vector<short> getFile::getNextData(){
    	if (multi_){
	       	cursor_ += parser_->eventSize(group_.multi_headers.at(1));
	       	read_data += (int) parser_->eventSize(group_.multi_headers.at(1));
		return parser_->parseData(cursor_, filesize_,group_.multi_headers.at(1));
	}
	else {
		cursor_ += parser_->eventSize(group_.single_header); // 다음 이벤트로 이동
		read_data += (int) parser_->eventSize(group_.single_header); // 다음 이벤트로 이동
		return parser_->parseData(cursor_, filesize_,group_.single_header);

	}

}

PacketGroup getFile::getHeader(const char * pointer_){
	PacketGroup group = parser_->parseHeader(pointer_, filesize_);
	return group;
}

std::vector<short> getFile::getData(const char * pointer_, PacketGroup group){
	if (multi_) return parser_->parseData(pointer_,filesize_,group.multi_headers.at(1));
	else return parser_->parseData(pointer_,filesize_,group.single_header);
}

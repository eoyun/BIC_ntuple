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
	if (MID<20) filename = "/pnfs/knu.ac.kr/data/cms/store/user/yeo/KEKTB202503/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/FADCData_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
	else if (MID<40) filename = "/pnfs/knu.ac.kr/data/cms/store/user/yeo/KEKTB202503/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/jbnu_daq_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
	else if (MID<50) filename = "/pnfs/knu.ac.kr/data/cms/store/user/yeo/KEKTB202503/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/bic_daq_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
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
	return cursor_;
}

size_t getFile::size() const {
	return filesize_;
}

bool getFile::isValid() const {
	return valid_;
}

PacketHeader getFile::getHeader(){
	const char* ptr = cursor_;
	const char* end = mapped_ + size();
	PacketHeader header;
	int data_length = 0;
	int tcb_trigger_number = 0;
	unsigned long long tcb_trigger_time = 0;
	int channel = 0;
	if (mid_>20){
		std::vector<char> header_raw(ptr,ptr + 32);
		cursor_ = cursor_ + 32;
		header.raw_bytes = header_raw;
		for (int a=0; a<4; a++) data_length += ((int)(header_raw.at(a) & 0xFF) << 8*a);
		
		/*
			 int run_number = 0;
			 for (int a=0; a<2; a++) run_number += ((int)(header[a+4] & 0xFF) << 8*a);
			 int trigger_type = ((int)header[6] & 0xFF);
		 */
		
		for (int a=0; a<4; a++) tcb_trigger_number += ((int)(header_raw.at(a+7) & 0xFF) << 8*a);
		
		int tcb_trigger_fine_time = ((int)header_raw.at(11) & 0xFF);
		int tcb_trigger_coarse_time = 0;
		for (int a=0; a<3; a++) tcb_trigger_coarse_time += ((int)(header_raw.at(a+12) & 0xFF) << 8*a);
		tcb_trigger_time = (tcb_trigger_fine_time * 8) + (tcb_trigger_coarse_time * 1000);
		
		//int mid = ((int)header[15] & 0xFF);
		channel = ((int)header_raw.at(16) & 0xFF);
	}
	else{
		std::vector<char> header_raw(ptr,ptr + 128);
		cursor_ =  cursor_ + 128;
		header.raw_bytes = header_raw;
	}
	//header.data_length = 2048;
	header.data_length = data_length;
	header.tcb_trigger_time = tcb_trigger_time;
	header.channel = channel;
	std::cout<<"header info : MID -> "<<mid_<<" channel -> "<<channel<<" data_length -> "<<data_length<<" trigger number -> "<<tcb_trigger_number<<std::endl;
	if (ptr + header.data_length > end) std::cout<<"something wrong!!"<<std::endl;
	header_ = header;
	return header;
	
	

} 

std::vector<short> getFile::getData(){
	const char* ptr = cursor_;
	cursor_ = cursor_ + header_.data_length - 32;
	std::vector<short> data_vector;
	return data_vector;
}

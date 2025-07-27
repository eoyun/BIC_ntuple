#include "getFile.h"
#include <iostream>
#include <sstream>
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
#include "DAQTypeCParser.h"
#include "DAQTypeDParser.h"

namespace fs = std::filesystem;

//int APixExist (std::vector<int> MIDs){
//	int idx = -1;
//	for (int i= 0;i<(int )MIDs.size();i++) if (MIDs.at(i) == 128) idx = i;
//	return idx;
//}

std::vector<int> getAPIX (int runnum){
	std::vector<int> APIXs;
	//std::string path = "/home/kobic/KEKTB202506/RawData/Run_" + std::to_string(runnum) + "/Run_"+std::to_string(runnum)+"_MID_"+ std::to_string(mid);
	std::string path = "/home/kobic/25CERNData";
    if (!fs::exists(path)) {
        std::cerr << "Path does not exist: " << path << std::endl;
        return APIXs;
    }

	char buffer[100];
	snprintf(buffer, sizeof(buffer), "Run_%05d", runnum);
	std::string keyword(buffer);


	try {
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_regular_file()) {
           		if (entry.file_size()== 0) {
           		    continue;  // 크기가 0인 파일은 처리하지 않음
           		}
				std::string filename = entry.path().filename().string();
				if (filename.find(keyword) != std::string::npos) {
					//std::cout << filename << std::endl;

					std::stringstream ss(filename);
					std::string token;
					std::vector<std::string> parts;

					while (std::getline(ss, token, '_')) {
						parts.push_back(token);
					}

					size_t dot_pos = parts.back().find('.');
					if (dot_pos != std::string::npos) {
						parts.back() = parts.back().substr(0, dot_pos);
					}
					//std::cout << parts[2] << " " << parts[3] << std::endl;

					//if ( parts[2]!="APIX" ){
					if ( parts[2]=="APIX"){
						APIXs.push_back(stoi(parts[3]));
						std::cout << filename << std::endl;
					}
				}
			}
		}
	} catch (const fs::filesystem_error& e) {
		std::cerr << "Filesystem error: " << e.what() << '\n';
	} catch (const std::exception& e) {
		std::cerr << "General error: " << e.what() << '\n';
	}

/*
	for (const auto& entry : fs::recursive_directory_iterator(path)) {
		if (entry.is_regular_file()) {
			std::cout << "Directory: " << entry.path() << std::endl;
			std::regex pattern(path+"/APIX_daq_(\\d+)_"+std::to_string(runnum)+".dat");
			std::smatch match;
			std::string sub_path = entry.path();
	
			if (std::regex_search(sub_path,match,pattern) && fs::file_size(sub_path) > 0){
			//if (std::regex_search(sub_path,match,pattern)){
				int APIX = std::stoi(match[1]);
				std::cout<<"APIX is "<<APIX<<std::endl;
				APIXs.push_back(APIX);
			}
			else std::cerr << "Pattern not matched!" << std::endl;
		}

	}*/
	return APIXs;


}

std::vector<int> getMID (int runnum){
	std::vector<int> MIDs;
	//std::string path = "/home/kobic/KEKTB202506/RawData/Run_" +std::to_string(runnum);
	std::string path = "/home/kobic/25CERNData";
	if (!fs::exists(path)) {
		std::cerr << "Path does not exist: " << path << std::endl;
		return MIDs;
	}
	char buffer[100];
	snprintf(buffer, sizeof(buffer), "Run_%05d", runnum);
	std::string keyword(buffer);

	try {
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_regular_file()) {
           		if (entry.file_size() == 0) {
           		    continue;  // 크기가 0인 파일은 처리하지 않음
           		}
				std::string filename = entry.path().filename().string();
				if (filename.find(keyword) != std::string::npos) {
					//std::cout << filename << std::endl;

					std::stringstream ss(filename);
					std::string token;
					std::vector<std::string> parts;

					while (std::getline(ss, token, '_')) {
						parts.push_back(token);
					}

					size_t dot_pos = parts.back().find('.');
					if (dot_pos != std::string::npos) {
						parts.back() = parts.back().substr(0, dot_pos);
					}
					//std::cout << parts[2] << " " << parts[3] << std::endl;

					//if ( parts[2]!="APIX" ){
					if ( parts[2]=="FADC" || parts[2]=="JBNU" || parts[2]=="BIC" ){
						MIDs.push_back(stoi(parts[3]));
						std::cout << filename << std::endl;
					}
				}
			}
		}
	} catch (const fs::filesystem_error& e) {
		std::cerr << "Filesystem error: " << e.what() << '\n';
	} catch (const std::exception& e) {
		std::cerr << "General error: " << e.what() << '\n';
	}



/*	for (const auto& entry : fs::recursive_directory_iterator(path)) {
		if (entry.is_directory()) {
			std::cout << "Directory: " << entry.path() << std::endl;
			std::regex pattern(path+"/Run_"+std::to_string(runnum)+"_MID_(\\d+)");
			std::smatch match;
			std::string sub_path = entry.path();
			if (std::regex_search(sub_path,match,pattern)){
				int MID = std::stoi(match[1]);
				
				// check if corresponding .dat file exists and is non-empty
				std::string datfile;
				if (MID < 20) {
					datfile = sub_path + "/FADCData_" + std::to_string(MID) + "_" + std::to_string(runnum) + ".dat";
				} else if (MID < 40) {
					datfile = sub_path + "/jbnu_daq_" + std::to_string(MID) + "_" + std::to_string(runnum) + ".dat";
				} else if (MID < 50) {
					datfile = sub_path + "/bic_daq_" + std::to_string(MID) + "_" + std::to_string(runnum) + ".dat";
				} else {
					std::cout << "MID is " << MID << std::endl;
					MIDs.push_back(MID);
					continue;
					
				}

				if (fs::exists(datfile) && fs::file_size(datfile) > 0) {
					std::cout << "MID is " << MID << std::endl;
					MIDs.push_back(MID);
				} else {
					std::cerr << "Empty or missing file for MID " << MID << ": " << datfile << std::endl;
				}
			}
			else std::cerr << "Pattern not matched!" << std::endl;
		}

	}*/
	return MIDs;

}

getFile::getFile(const int runnum, const int MID)
	: fd_(-1),filesize_(0),mapped_(nullptr),valid_(false)
{
	std::string filename;
	char buffer[200];
	if (MID<20){ 
		//filename = "/home/kobic/KEKTB202506/RawData/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/FADCData_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
		snprintf(buffer,sizeof(buffer),"/home/kobic/25CERNData/Run_%05d_FADC_%d.dat",runnum,MID);
		filename = buffer;
		parser_ = std::make_unique<DAQTypeBParser>();
		multi_ = true;
	}
	else if (MID<40) {
		//filename = "/home/kobic/KEKTB202506/RawData/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/jbnu_daq_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
		snprintf(buffer,sizeof(buffer),"/home/kobic/25CERNData/Run_%05d_JBNU_%d.dat",runnum,MID);
		filename = buffer;
		parser_ = std::make_unique<DAQTypeAParser>();
	}
	else if (MID<50){
	    //filename = "/home/kobic/KEKTB202506/RawData/Run_" +std::to_string(runnum) +"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/bic_daq_"+std::to_string(MID)+"_"+std::to_string(runnum)+".dat";
		snprintf(buffer,sizeof(buffer),"/home/kobic/25CERNData/Run_%05d_BIC_%d.dat",runnum,MID);
		filename = buffer;
		parser_ = std::make_unique<DAQTypeDParser>();
		bic_ = true;
	}
	else {
		std::cout<< "this MID is not BIC or JBNU, FADC | MID : "<<MID<<std::endl;
		return;
	}
	std::cout<<"MID : "<<MID<<std::endl;
	mapFile(filename);
	mid_ = MID;
}

getFile::getFile(const int runnum, const int APIX_flag, const int APixNum)
	: fd_(-1),filesize_(0),mapped_(nullptr),valid_(false)
{
	std::string filename;
	char buffer[200];
	if (APIX_flag == 1){
		//filename = "/home/kobic/KEKTB202506/RawData/Run_"+std::to_string(runnum)+"/Run_"+std::to_string(runnum)+"_MID_"+std::to_string(MID)+"/APIX_daq_"+std::to_string(APixNum)+"_"+std::to_string(runnum)+".dat";
		snprintf(buffer,sizeof(buffer),"/home/kobic/25CERNData/Run_%05d_APIX_%d.dat",runnum,APixNum);
		filename = buffer;
		parser_ = std::make_unique<DAQTypeCParser>();
		apix_ = true;
	}else {
		std::cout<<"no apix flag"<<std::endl;
	}
	mapFile(filename);
	if(apix_) getApixSize();
	aid_file  = APixNum;

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

void getFile::getApixSize() {
	if(apix_ )filesize_ = parser_->findEnd(filesize_,cursor_);
	return;
}

bool getFile::isValid() const {
	return valid_;
}

bool getFile::isMulti() const {
	return multi_;
}

bool getFile::isAPix() const {
	return apix_;
}

bool getFile::isBIC() const {
	return bic_;
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

// out-dated function
std::vector<short> getFile::getNextData(){
    	if (multi_){
	       	cursor_ += parser_->eventSize(group_.multi_headers.at(1));
	       	read_data += (int) parser_->eventSize(group_.multi_headers.at(1));
		return parser_->parseData(cursor_, filesize_,group_.multi_headers.at(1),-1);
	}
	else {
		cursor_ += parser_->eventSize(group_.single_header); // 다음 이벤트로 이동
		read_data += (int) parser_->eventSize(group_.single_header); // 다음 이벤트로 이동
		return parser_->parseData(cursor_, filesize_,group_.single_header,-1);

	}

}

PacketGroup getFile::getHeader(const char * pointer_){
	PacketGroup group = parser_->parseHeader(pointer_, filesize_);
	return group;
}

std::vector<short> getFile::getData(const char * pointer_, PacketGroup group, int channel){
	if (multi_) return parser_->parseData(pointer_,filesize_,group.multi_headers.at(1),channel);
	else return parser_->parseData(pointer_,filesize_,group.single_header,channel);
}
PacketGroup getFile::getCurrentHeader(){
	PacketGroup group = parser_->parseHeader(cursor_, filesize_);
	group_ = group;
    	return group;
}

void getFile::getNextPacket(){
	if (multi_){
	       	cursor_ += parser_->eventSize(group_.multi_headers.at(1));
	       	read_data += (int) parser_->eventSize(group_.multi_headers.at(1));
	}
	else if (apix_){
		read_data += (int) parser_->eventSize(cursor_);
		cursor_ += parser_->eventSize(cursor_);
		
	}
	else {
		cursor_ += parser_->eventSize(group_.single_header); // 다음 이벤트로 이동
		read_data += (int) parser_->eventSize(group_.single_header); // 다음 이벤트로 이동
	}


}


#include <iostream>
#include "functions.h"
#include <vector>
#include <string>

std::vector<Mapping> getMap(const char* filename){
	FILE* fp;
	fp = fopen(filename,"rt");
	int MID;
	int ch;
	char name_tmp[10];
	std::vector<Mapping> result;
	std::cout<<filename<<std::endl;
	if (!fp) {
    		std::cerr << "❌ Error: Failed to open file." << std::endl;
    		perror("fopen"); // 시스템 에러 메시지도 같이 출력됨 (optional)
    		exit(EXIT_FAILURE); // 또는 return {} 등 적절히 처리
	}
	while (fscanf(fp,"%d %d %s",&MID,&ch,name_tmp) == 3){
		Mapping tmp;
		tmp.MID = MID;
		tmp.ch = ch;
		tmp.name = std::string(name_tmp);
		std::cout<<"MID : "<<tmp.MID<<" | ch : "<<tmp.ch<<" | name : "<<tmp.name<<std::endl;
		result.push_back(tmp);
	}
	fclose(fp);
	return result;
}


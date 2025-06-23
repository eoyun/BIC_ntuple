#include "test.h"
#include "getFile.h"
#include <iostream>
#include <vector>

int main( int argc, char * argv[]) {
    int runnum = atoi(argv[1]);
    Converter converter;
    converter.printMessage();
    std::cout<<"run number is "<<runnum<<std::endl;
    std::vector MIDs = getMID(runnum);
    for (auto i : MIDs){
	if (i != 2) continue;
    	getFile *f = new getFile(runnum,i);
	std::cout<<"MID : "<<i<<" | data is "<<static_cast<const void*>(f->data())<<" | size is "<<f->size()<<" | valid "<<f->isValid()<<std::endl;
	if (i ==2){
		while(f->cursor() <= f->data() + f->size()){
			f->getHeader();
			f->getData();
		}
	}
	delete f;
    }
    return 0;
}

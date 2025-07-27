#ifndef APIXSTRUCT_H
#define APIXSTRUCT_H
#include <vector>

struct APixstruct {
	unsigned long long tcb_trigger_time;
	std::vector<unsigned short> aid;
	std::vector<unsigned short> pay;
	std::vector<unsigned short> isCol;
	std::vector<unsigned short> ch;
	std::vector<unsigned short> ts;
	std::vector<unsigned short> tot;

};

#endif

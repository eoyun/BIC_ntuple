#ifndef GETMAP_H
#define GETMAP_H

#include <iostream>
#include "functions.h"
#include <vector>
#include <map>
#include <utility>

struct ChInfo {
    int mid;    // MID
    int ch;     // Channel
    int lr;     // Left/Right
    int modid;  // Module ID
    int col;    // Column
    int row;    // Row
    int isY;    // isY
};

std::vector<Mapping> getMap(const char* filename);//out-dated
std::vector<ChInfo> getChInfo(const std::map<std::pair<int,int>, std::vector<int>>& chMap);




#endif

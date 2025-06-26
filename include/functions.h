#pragma once

#include <cstring>
#include <vector>

struct Mapping {
    int MID;
    int ch;
    std::string name;
    
    bool operator<(const Mapping& other) const {
        return std::tie(MID, ch, name) < std::tie(other.MID, other.ch, other.name);
    }
};

template<typename T>
std::vector<std::vector<T>> splitVector(const std::vector<T>& input, size_t chunkSize) {
    size_t total = input.size();
    size_t split_num = total/chunkSize;
    std::vector<std::vector<T>> result;
    for (size_t r = 0; r < split_num; ++r) {
	
	std::vector<T> tmp;
        for (size_t c = 0; c < chunkSize; ++c) {
		tmp.push_back(input.at(c * split_num +r));
        }
	result.push_back(tmp);
    }
    return result;
}

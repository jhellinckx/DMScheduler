#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <iostream>
#include <vector>

#include "task.hpp"
#include "simulator.hpp"

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec){
	for(const T& t: vec){
		out << t << " ";
	}
	return out;
}	

namespace utils{
}


#endif
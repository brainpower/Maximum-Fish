
#include "StasisPod.hpp"
#include "SimState.hpp"

void StasisPod::freeze(std::shared_ptr<SimState> s){
	_pod.push_back(std::shared_ptr<SimState>(new SimState(*s)));
}

std::shared_ptr<SimState> StasisPod::taw(const size_t i){
	auto ret = _pod[i];
	_pod.erase(_pod.begin()+i);
	return ret;
}

const std::shared_ptr<SimState> StasisPod::peek(const size_t i){
	return _pod[i];
}


#include "StasisPod.hpp"
#include "SimState.hpp"

void StasisPod::freeze(std::shared_ptr<SimState> s){
	_pod.push_front(std::shared_ptr<SimState>(new SimState(*s)));
}

std::shared_ptr<SimState> StasisPod::tawTop(){
	auto ret = _pod.front();
	_pod.pop_front();
	return ret;
}

std::shared_ptr<SimState> StasisPod::taw(const size_t i){
	auto ret = _pod[i];
	_pod.erase(_pod.begin()+i);
	//_pod.erase(_pod.begin(), _pod.begin()+i); // discard states newer than this tawed one???
	return ret;
}

std::shared_ptr<SimState> StasisPod::tawTick(const int i){
	for( auto it = _pod.begin(); (*it)->_currentTick > i ; ++it);
	auto _ret = *it;
	_pod.erase(it);
	//_pod.erase(_pod.begin(), it); // discard states newer than this tawed one???
	return _ret;
}

const std::shared_ptr<SimState> StasisPod::peek(const size_t i){
	return _pod[i];
}

const std::shared_ptr<SimState> StasisPod::peekTop(){
	return _pod.front();
}

const std::shared_ptr<SimState> StasisPod::peekTick(const int i){
	for( auto it = _pod.begin(); (*it)->_currentTick > i ; ++it);
	return *it;
}


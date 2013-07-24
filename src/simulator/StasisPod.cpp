#include <sbe/Engine.hpp>
#include "StasisPod.hpp"
#include "SimState.hpp"


void StasisPod::freeze(std::shared_ptr<SimState> s){
	Engine::out(Engine::SPAM) << "[StasisPod] Freeze" << std::endl;

	if(_pod.empty() || s->currentTick > peekTop()->currentTick){
		_pod.push_front(std::shared_ptr<SimState>(new SimState(*s)));

		Engine::out() << "[StasisPod] Freezed - tick " << peekTop()->currentTick << std::endl;
		return;
	}
	auto it = _pod.begin();
	for(; it != _pod.end() && (*it)->currentTick != s->currentTick ; ++it);
	*it = std::make_shared<SimState>(*s);

	Engine::out() << "[StasisPod] Freezed - tick " << peekTop()->currentTick << " overwritten" << std::endl;
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
	auto it = _pod.begin();
	for( ; it != _pod.end() && (*it)->currentTick > i ; ++it);
	if( it != _pod.end()){
		auto _ret = *it;
		_pod.erase(it);
		//_pod.erase(_pod.begin(), it); // discard states newer than this tawed one???
		return _ret;
	} return nullptr;
}

const std::shared_ptr<const SimState> StasisPod::peek(const size_t i){
	return _pod[i];
}

const std::shared_ptr<const SimState> StasisPod::peekTop(){
	return _pod.empty() ? nullptr : _pod.front();
}

const std::shared_ptr<const SimState> StasisPod::peekTick(const int i){
	auto it = _pod.begin();
	for( ; it != _pod.end() && (*it)->currentTick > i ; ++it);
	return it != _pod.end() ? *it : nullptr;
}

void StasisPod::discardStartingWith(const int tick){
	auto it = _pod.begin();
	for( ; it != _pod.end() && (*it)->currentTick > tick; ++it);
	_pod.erase(_pod.begin(), it);
}

void StasisPod::clear(){
	_pod.clear();
}

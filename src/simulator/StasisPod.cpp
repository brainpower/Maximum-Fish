
void StasisPod::freeze(std::shared_ptr<SimState> s){
	_pod.push_back(std::shared_ptr(new SimState(s)));
}

void StasisPod::taw(const size_t i){
	auto ret = _pod[i];
	_pod.erase(_pod.begin()+i);
	return _ret;
}

const std::shared_ptr<SimState> StasisPod::peek(const size_t i){
	return _pod[i];
}

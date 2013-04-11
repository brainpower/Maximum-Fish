#ifndef STASISPOD_HXX
#define STASISPOD_HXX

class StasisPod {

public:
	StasisPod() {}

	void freeze(std::shared_ptr<SimState> s);
	std::shared_ptr<SimState> taw(const size_t i);
	std::shared_ptr<SimState> look(const size_t i);

private:

	std::deque<std::shared_ptr<SimState>> _pod;
};

#endif STASISPOD_HXX

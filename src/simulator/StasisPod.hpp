#ifndef STASISPOD_HXX
#define STASISPOD_HXX

#include <memory> // shared_ptr
#include <deque>

class SimState;

class StasisPod {

public:
	StasisPod() {}

	void freeze(std::shared_ptr<SimState> s);
	std::shared_ptr<SimState> taw(const size_t i);
	std::shared_ptr<SimState> tawTop();
	std::shared_ptr<SimState> tawTick(const int i);
	const std::shared_ptr<SimState> peek(const size_t i);
	const std::shared_ptr<SimState> peekTop();
	const std::shared_ptr<SimState> peekTick(const int i);

private:
	friend class StasisPodIOPlugin;

	std::deque<std::shared_ptr<SimState>> _pod;
};

#endif // STASISPOD_HXX

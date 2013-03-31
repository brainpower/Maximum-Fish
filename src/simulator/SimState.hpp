
#ifndef SIMSTATE_HPP
#define SIMSTATE_HPP

class SimState {

public:
	explicit SimState(const Simulator &s);

private:

	std::string rnd_state;
	std::shared_ptr<Terrain> _terrain;
	std::shared_ptr<std::list<Creature>> _creatures;
	std::vector<std::shared_ptr<Species>> _species;

	int currentTick;
};

#endif //SIMSTATE_HPP

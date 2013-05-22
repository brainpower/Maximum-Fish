
#ifndef SIMSTATE_HPP
#define SIMSTATE_HPP

#include <boost/thread.hpp> // thread_specific_ptr ?

#include <list>
#include <vector>
#include <random>
#include <memory> // shared_ptr

#include "world/Species.hpp"

class Tile;
class Terrain;
class Creature;

class SimState {

public:
	explicit SimState()
	 : _terrain(nullptr), _currentTick(0), _currentSeed(0), _numGenerated(0) {}

	/// makes deep copy of the state, so all creatures,species etc. of this state are preserved
	explicit SimState(const SimState &s);

	~SimState(){}

private:

	//std::string rnd_state;
	std::shared_ptr<Terrain> _terrain;
	std::list<std::shared_ptr<Creature>> _creatures;
	std::vector<std::shared_ptr<Species>> _species;

	int _currentTick;
	int _currentSeed;

	unsigned int _numGenerated;

	std::shared_ptr<std::mt19937> _seeder;
	std::vector<std::shared_ptr<std::mt19937>> _gens;
	boost::thread_specific_ptr<std::mt19937> _gen;

	friend class Simulator;
	friend class SimStateIOPlugin;
	friend class Generator;
	friend class StasisPod;
};

#endif //SIMSTATE_HPP

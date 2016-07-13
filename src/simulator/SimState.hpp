#ifndef SIMSTATE_HPP
#define SIMSTATE_HPP

#include <list>
#include <vector>
#include <random>
#include <memory> // shared_ptr

class Species;
class Tile;
class Terrain;
class Creature;

class SimState {

public:
	explicit SimState()
	 : terrain(nullptr), species(nullptr), currentTick(0), currentSeed(0), numThreads(0), numGenerated(0) {}

	/// makes deep copy of the state, so all creatures,species etc. of this state are preserved
	explicit SimState(const SimState &s);

	~SimState(){}

private:

	//std::string rnd_state;
	std::shared_ptr<Terrain> terrain;
	std::list<std::shared_ptr<Creature>> creatures;
	std::shared_ptr<std::vector<std::shared_ptr<Species>>> species;

	int currentTick;
	int currentSeed;
	int numThreads;

	unsigned int numGenerated;

	std::shared_ptr<std::mt19937> seeder;
	std::vector<std::shared_ptr<std::mt19937>> gens;

	friend class Simulator;
	friend class SimStateIOPlugin;
	friend class Generator;
	friend class StasisPod;
};

#endif //SIMSTATE_HPP

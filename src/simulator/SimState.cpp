#include "SimState.hpp"

#include <sbe/Engine.hpp>
#include "world/Terrain.hpp"
#include "world/Creature.hpp"

SimState::SimState(const SimState &o)
	: terrain( new Terrain(*o.terrain) ),
	  species(o.species),
	  currentTick(o.currentTick),
	  currentSeed(o.currentSeed),
	  numThreads(o.numThreads),
	  numGenerated(o.numGenerated)
{
	seeder.reset(new std::mt19937(*o.seeder));
	for( auto p : o.gens)
		gens.push_back(std::make_shared<std::mt19937>(*p));

	for ( auto it = o.creatures.begin(); it != o.creatures.end(); ++it )
	{ // deep copy of creatures
		creatures.push_back(std::make_shared<Creature>(**it));
		creatures.back()->updateTileFromPos(terrain);
	}

	Engine::out(Engine::SPAM) << "[SimState] Copied!" << std::endl;
}

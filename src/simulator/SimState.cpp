#include "SimState.hpp"

#include <sbe/Engine.hpp>
#include "world/Terrain.hpp"
#include "world/Creature.hpp"

SimState::SimState(const SimState &o)
	: _currentTick(o._currentTick),
	  _currentSeed(o._currentSeed),
	  _numGenerated(o._numGenerated),
	  _terrain( new Terrain(*o._terrain) ),
	  _species(o._species),
	  _numThreads(o._numThreads)
{
	_seeder.reset(new std::mt19937(*o._seeder));
	for( auto p : o._gens)
		_gens.push_back(std::make_shared<std::mt19937>(*p));

	for ( auto it = o._creatures.begin(); it != o._creatures.end(); ++it )
	{ // deep copy of creatures
		_creatures.push_back(std::make_shared<Creature>(**it));
		_creatures.back()->updateTileFromPos(_terrain);
	}

	Engine::out(Engine::SPAM) << "[SimState] Copied!" << std::endl;
}

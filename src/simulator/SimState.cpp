#include "SimState.hpp"

#include "world/Terrain.hpp"
#include "world/Creature.hpp"

SimState::SimState(const SimState &o)
	: _currentTick(o._currentTick),
	  _currentSeed(o._currentSeed),
	  _numGenerated(o._numGenerated),
	  _terrain( new Terrain(*o._terrain) ),
	  _species(o._species)
{
	_seeder.reset(new std::mt19937(*o._seeder));
	for( auto p : o._gens)
		_gens.push_back(std::shared_ptr<std::mt19937>(new std::mt19937(*p)));

	for ( auto it = o._creatures.begin(); it != o._creatures.end(); ++it )
	{ // deep copy of creatures
		_creatures.push_back(std::shared_ptr<Creature>(new Creature(**it)));
		_creatures.back()->updateTileFromPos(_terrain);
	}

}

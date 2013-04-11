#include "SimState.hpp"

SimState::SimState(const SimState &o)
	: _currentTick(o._currentTick),
	  _currentSeed(_currentSeed),
	  _numGenerated(o._numGenerated),
	  _terrain(o._terrain),
	  _species(o._species){

	auto tl = _terrain.getTileList();
	for(auto it=tl.begin(); it != tl.end(); ++it) // clear creatures list of Tiles, working?
		it->getCreatures()->clear();

	for(auto it=o._creatures.begin(); it != o._creatures.end(); ++it){ // deep copy of creatures
		_creatures.push_back(std::shared_ptr<Creature>(new Creature(**it)));
		_creatures.back().updateTileFromPos(_terrain);
	}

}

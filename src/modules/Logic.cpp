#include "Logic.hpp"

#include <SFML/System.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>


#include "sbe/event/Event.hpp"

#include "simulator/Simulator.hpp"

Logic* Logic::mInstance;

Logic::Logic()
{
	mInstance = this;
}

Logic::~Logic()
{
	//Engine::out() << "World..." << std::endl;
	mInstance = nullptr;
}

void Logic::Init()
{
	Engine::out() << "[Logic] Creating Sim" << std::endl;
	Sim.reset ( new Simulator() );
}

void Logic::DeInit()
{
	Sim.reset();
}




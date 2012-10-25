#include "Logic.h"

#include <SFML/System.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>


#include "sbe/event/Event.h"


Logic* Logic::mInstance;

Logic::Logic()
{
	mInstance = this;
}

Logic::~Logic()
{
	Engine::out() << "World..." << std::endl;
	mInstance = nullptr;
}

void Logic::Init()
{
	Engine::out() << "Loading World, do your stuff here :D" << std::endl;

}

void Logic::DeInit()
{
}




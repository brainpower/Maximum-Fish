#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"

class Simulator : public EventUser
{
	public:
		Simulator();
		virtual ~Simulator() {};
		virtual void HandleEvent( Event& e);
};

#endif // SIMULATOR_H

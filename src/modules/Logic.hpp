#ifndef LOGIC_H
#define LOGIC_H

#include "sbe/Module.hpp"
#include "sbe/event/EventUser.hpp"

#include <memory>

class Simulator;

/// Logic Module for the simulator
class Logic : public Module
{
	public:
		Logic();
		virtual ~Logic();


	private:
		virtual void Init();
		virtual void DeInit();

		static Logic* mInstance;
		std::shared_ptr<Simulator> Sim;
};


#endif // LOGIC_H


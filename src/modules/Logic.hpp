#ifndef LOGIC_H
#define LOGIC_H

#include "sbe/Module.hpp"
#include "sbe/ScriptingEngine.hpp"
#include "sbe/event/EventUser.hpp"

#include <memory>

class Simulator;

/// Logic Module for the simulator
class Logic : public sbe::Module
{
	public:
		Logic();
		virtual ~Logic();


	private:
		virtual void Init();
		virtual void DeInit();

		static Logic* mInstance;
		std::shared_ptr<Simulator> Sim;
		std::shared_ptr<sbe::ScriptingEngine> Script;
};


#endif // LOGIC_H


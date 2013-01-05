#include "Simulator.hpp"

#include <boost/thread.hpp>

Simulator* Simulator::Instance = nullptr;

Simulator::Simulator()
{
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	RegisterForEvent("EVT_TICK");
	RegisterForEvent("EVT_QUIT");
}

void Simulator::HandleEvent(Event& e)
{
	if(e.Is("EVT_TICK"))
	{
		tick();
	}

	if (e.Is("EVT_QUIT"))
	{
		Module::Get()->RequestQuit();
	}
}

void Simulator::tick()
{
	for(std::shared_ptr<Creature> c_ptr: m_creatures)
	{
		//and ya god said live creature !... LIVE !!!
		c_ptr->live();

		//make some freaking event, man
		Event e("UpdateCreatureRenderList");
		e.SetData ( m_creatures );
		Module::Get()->QueueEvent(e, true);
	}
}

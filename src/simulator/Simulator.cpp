#include "Simulator.hpp"

#include <boost/thread.hpp>

Simulator* Simulator::Instance = nullptr;

Simulator::Simulator():
m_pause(false) {
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

	if(e.Is("TOGGLE_SIM_PAUSE"))
	{
		if(m_pause == false)
		{
			m_pause = true;
		} else {
			m_pause = false;
		}
	}

	if (e.Is("EVT_QUIT"))
	{
		Module::Get()->RequestQuit();
	}
}

void Simulator::tick()
{
	while(!m_pause)
	{
		for(std::shared_ptr<Creature> c_ptr: m_creatures)
		{
			//make some freaking event, man
			Event e("UpdateCreatureRenderList");
			e.SetData ( m_creatures );
			Module::Get()->QueueEvent(e, true);

			//and ya god said live creature !... LIVE !!!
			c_ptr->live();
		}
	}
}

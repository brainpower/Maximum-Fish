#include "Simulator.hpp"

#include <boost/thread.hpp>

Simulator::Simulator()
{
	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	RegisterForEvent("EVT_QUIT");
}

void Simulator::HandleEvent(Event& e)
{
	if (e.Is("EVT_QUIT"))
	{
		Module::Get()->RequestQuit();
	}
}

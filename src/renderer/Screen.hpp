#ifndef SCREEN_H
#define SCREEN_H

#include "sbe/event/Event.hpp"
#include "sbe/event/EventUser.hpp"

class SFMLEventConverter;
class DebugWindow;
class MainMenu;
class MiniMap;
class SimView;

namespace sfg { class Desktop; class SFGUI; }
namespace sf { class Clock; }
/**
The hud is the main UI class its the only class directly receiving events from the event system.
They are propagated to all subitems.
This layout allows for multiple sets of interface controls to be used and displayed, e.g if the game menu is opened
the hud for the menu would be set as "main" -ui item and the normal ingame hud would no longer receive key/mouse events
*/
class Screen : public EventUser
{
	public:
		Screen();
		virtual ~Screen() {};


		void Render();
		virtual void HandleEvent(Event& e);

	private:

		void Init();
		std::shared_ptr<SFMLEventConverter> EvtConv;

		std::shared_ptr<DebugWindow> DbgWin;
		std::shared_ptr<MainMenu> MnMnWin;
		std::shared_ptr<MiniMap> MiMap;
		std::shared_ptr<sfg::SFGUI> SFG;
		std::shared_ptr<sfg::Desktop> Desktop;
		std::shared_ptr<sf::Clock> guiclock;
		std::shared_ptr<SimView> SimulatorView;
};

#endif // SCREEN_H


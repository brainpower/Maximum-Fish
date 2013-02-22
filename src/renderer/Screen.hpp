#ifndef SCREEN_H
#define SCREEN_H

#include "sbe/event/Event.hpp"
#include "sbe/event/EventUser.hpp"

class SFMLEventConverter;
class Control;
class InfoPanel;
class DebugWindow;
class MainMenu;
class MiniMap;
class SimView;
//class Manipulator;

namespace sfg { class Desktop; class SFGUI; }
namespace sf { class Clock; }
/**
The hud is the main UI class its the only class directly receiving events from the event system.
They are propagated to all subitems.
This layout allows for multiple sets of interface controls to be used and displayed, e.g if the game menu is opened
the hud for the menu would be set as "main" -ui item and the normal ingame hud would no longer receive key/mouse events
*/
class Screen : public EventUser, public sf::NonCopyable
{
	public:
		Screen();
		virtual ~Screen() {};


		void Render();
		virtual void HandleEvent(Event& e);
		bool KeyEventCatcher;


		static Screen* GetScreenObj()
		{
			return Instance;
		}

		void setCameraViewPort ( const sf::FloatRect& FR);
	private:

		bool Fullscreen;
		/**
		Counts the amount of windows which have received an OnMouseEnter but no OnMouseLeave yet.
		If this is > 0 we prevent events being sent to the simview, as they should be handled in the sfg ui
		*/
		int ActiveWindows;
		void OnWindowEntered();
		void OnWindowLeft();

		void Init();
		std::shared_ptr<SFMLEventConverter> EvtConv;

        std::shared_ptr<Control> Contr;
        std::shared_ptr<InfoPanel> IPan;
        //std::shared_ptr<Manipulator> Man;
		std::shared_ptr<DebugWindow> DbgWin;
		std::shared_ptr<MainMenu> MnMnWin;
		std::shared_ptr<MiniMap> MiMap;
		std::shared_ptr<sfg::SFGUI> SFG;
		std::shared_ptr<sfg::Desktop> Desktop;
		std::shared_ptr<sf::Clock> guiclock;
		std::shared_ptr<SimView> SimulatorView;

		static Screen* Instance;
};

#endif // SCREEN_H


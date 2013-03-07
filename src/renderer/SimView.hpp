#ifndef SIMVIEW_H
#define SIMVIEW_H

#include "sbe/event/SFMLEventUser.hpp"
#include <memory>

class Control;
class InfoPanel;
class DebugWindow;
class MainMenu;
class MiniMap;
class SimActors;
//class Manipulator;

class SimView : public SFMLEventUser
{
	public:

		SimView();
		~SimView();

		// Handles Clicks on the Terrain
		void HandleSfmlEvent ( const sf::Event& e);

		void InitRenderer();
		void InitDesktop();

		/// loads all Textures, Shaders etc required for rendering
		bool LoadResources();

		void Render();
	private:

		/// our mediator between Sim and Renderer, creates and updates various Actors from the simevents
		std::unique_ptr<SimActors> ISim;

		// various desktop items
        std::unique_ptr<Control> Contr;
        std::unique_ptr<InfoPanel> IPan;
        //std::unique_ptr<Manipulator> Man;
		std::unique_ptr<DebugWindow> DbgWin;
		std::unique_ptr<MainMenu> MnMnWin;
		std::unique_ptr<MiniMap> MiMap;

};

#endif // SIMVIEW_H


#ifndef SIMVIEW_H
#define SIMVIEW_H

#include "sbe/event/SFMLEventUser.hpp"
#include <memory>

class Control;
class InfoPanel;
class MainMenu;
class GraphBook;
class SimActors;
class NewSimWindow;

namespace sbe {
	class DebugWindow;
}

//class Manipulator;

/// Handles initialisation and configuration of the renderer/desktop etc
class SimView : public sbe::SFMLEventUser
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

	private:

		/// our mediator between Sim and Renderer, creates and updates various Actors from the simevents
		std::unique_ptr<SimActors> ISim;

		// various desktop items
        std::unique_ptr<Control> Contr;
        std::unique_ptr<InfoPanel> IPan;
        //std::unique_ptr<Manipulator> Man;
		std::unique_ptr<sbe::DebugWindow> DbgWin;
		std::unique_ptr<NewSimWindow> NewWin;
		std::unique_ptr<MainMenu> MnMnWin;
		std::unique_ptr<GraphBook> GraBo;

};

#endif // SIMVIEW_H


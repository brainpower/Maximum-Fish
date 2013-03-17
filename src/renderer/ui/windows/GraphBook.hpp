#ifndef GRAPHBOOK_H
#define GRAPHBOOK_H

#include "sbe/event/EventUser.hpp"
#include "sbe/geom/Point.hpp"
#include "sbe/gfx/GraphPlotter.hpp"

#include <SFGUI/Window.hpp>
#include <SFGUI/Notebook.hpp>
#include <SFML/System/Clock.hpp>

/// an overview of different graphs
class GraphBook : public sbe::EventUser
{
	public:

		GraphBook( const Geom::Vec2 Size = Geom::Vec2(400, 400) );
		virtual ~GraphBook() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow( const Geom::Vec2 Size );
		void AddNewGraph( std::string displayName );
		void UpdateGraph();
		void PlotGraph( std::shared_ptr<sbe::GraphPlotter>& G );
		void updatePosition();

		sfg::Window::Ptr Win;
		sfg::Notebook::Ptr Tabs;
		sf::Clock UpdateTimer;
};

#endif // GRAPHBOOK_H


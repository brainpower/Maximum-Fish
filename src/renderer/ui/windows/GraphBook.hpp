#ifndef GRAPHBOOK_H
#define GRAPHBOOK_H

#include "sbe/event/EventUser.hpp"
#include "sbe/geom/Point.hpp"
#include "sbe/gfx/GraphPlotter.hpp"

#include <SFGUI/SharedPtr.hpp>
namespace sfg
{
	class Window;
	class Notebook;
}
#include <SFML/System/Clock.hpp>

/// an overview of different graphs
class GraphBook : public sbe::EventUser
{
	public:

		GraphBook( const Geom::Vec2 Size = Geom::Vec2( 400, 400 ) );
		virtual ~GraphBook() {};

		virtual void HandleEvent( Event& e );

	private:
		void CreateWindow( const Geom::Vec2 Size );
		void AddNewGraph( std::string displayName );
		void UpdateGraph();
		void PlotGraph( std::shared_ptr<sbe::GraphPlotter>& G );
		void updatePosition();

		sfg::SharedPtr<sfg::Window> Win;
		sfg::SharedPtr<sfg::Notebook> Tabs;
		sf::Clock UpdateTimer;
};

#endif // GRAPHBOOK_H


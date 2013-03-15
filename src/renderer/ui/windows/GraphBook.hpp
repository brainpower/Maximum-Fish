#ifndef GRAPHBOOK_H
#define GRAPHBOOK_H

#include "sbe/event/EventUser.hpp"
#include "sbe/geom/Point.hpp"
#include "sbe/gfx/GraphPlotter.hpp"

#include <SFGUI/SFGUI.hpp>

/// an overview of different graphs
class GraphBook : public EventUser
{
	public:

		GraphBook( const Geom::Vec2 Size = Geom::Vec2(400, 400));
		virtual ~GraphBook() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Vec2 Size);
		void TabsChangeClick ();
		void PlotGraph( std::shared_ptr<GraphPlotter>& G );
		void updatePosition();

		sfg::Window::Ptr Win;
};

#endif // GRAPHBOOK_H


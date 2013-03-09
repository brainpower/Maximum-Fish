#ifndef MINIMAP_H
#define MINIMAP_H

#include "sbe/event/EventUser.hpp"
#include "sbe/geom/Point.hpp"

#include <SFGUI/SFGUI.hpp>

/// a minimap
class MiniMap : public EventUser
{
	public:

		MiniMap( const Geom::Vec2 Size = Geom::Vec2(250, 250));
		virtual ~MiniMap() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Vec2 Size);
        void BtnResumeClick();
        void BtnExitClick();
		void updatePosition();

		sfg::Window::Ptr Win;
		unsigned int currentlabeltext;

};

#endif // MINIMAP_H


#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "sbe/event/EventUser.hpp"
#include "renderer/items/DetailsEmpty.hpp"
#include "renderer/items/DetailsCreature.hpp"
#include "renderer/items/DetailsSpecies.hpp"
#include "renderer/items/DetailsTile.hpp"

#include <SFGUI/SFGUI.hpp>

class Manipulator : public EventUser
{
	public:

		Manipulator( const Geom::Point& RelativePosition = Geom::Point(280, 0), const Geom::Vec2 Size = Geom::Vec2(300, 600));
		virtual ~Manipulator() {};

		virtual void HandleEvent( Event& e);
        bool ThisNotInfoPanel;
    private:
		void SwitchToInfoPanel();
        void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);
        void updatePosition();
		void SetInformation_1();
		void SetInformation_2();
		void ResetInformation();

		sfg::Window::Ptr Win;
        sfg::Label::Ptr Information;
};

#endif // MANIPULATOR_H


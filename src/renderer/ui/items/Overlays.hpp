#ifndef OVERLAYS_HPP
#define OVERLAYS_HPP

#include <SFGUI/Widget.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Frame.hpp>
#include <SFGUI/Box.hpp>

#include "sbe/event/EventUser.hpp"
#include "sbe/gfx/MapPlotter.hpp"
#include "sbe/sfg/List.hpp"

#include <memory>
#include <map>
#include <string>

/// shows a list of overlays and provides options ans preview
class Overlays : EventUser
{
	public:
		Overlays();
		sfg::Widget::Ptr Get();
		/**
			Events Handled:
				Event 			| 		Data
			--------------------|---------------------------
			OVERLAY_CLICK		| std::string
			DISPLAY_MAP			| std::shared_ptr<MapPlotter>
		*/
		void HandleEvent( Event& e);

		void ClearOverlays();

	private:

		void ShowMap( std::string& name );
		void AddMap( std::shared_ptr<MapPlotter>& M );

		sfg::Button::Ptr ClearButton;
		sfg::Frame::Ptr CurrentFrame;
		sfg::Box::Ptr myBox;

		std::map<std::string, std::shared_ptr<MapPlotter>> Maps;
		sfgList myOverlays;

};

#endif // OVERLAYS_HPP


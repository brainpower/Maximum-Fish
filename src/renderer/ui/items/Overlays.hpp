#ifndef OVERLAYS_HPP
#define OVERLAYS_HPP

#include <SFGUI/Widget.hpp>
#include <SFGUI/Frame.hpp>
#include <SFGUI/Box.hpp>

#include "sbe/event/EventUser.hpp"
#include "sbe/gfx/MapPlotter.hpp"
#include "sbe/sfg/List.hpp"
#include "sbe/gfx/Actor.hpp"

#include <memory>
#include <map>
#include <string>

/// shows a list of overlays and provides options ans preview
class Overlays : sbe::EventUser
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
			RESET_UI    | Remove all overlays
		*/
		void HandleEvent( Event& e);

		/// Removes current overlay
		void ClearOverlays();
		/// Toggle rendering of current overlay
		void ToggleRendering();
		/// Disable rendering of all overlays
		void HideAll();
		/// toggle live updating of overlays
		void ToggleLive();
		/// update overlays right now
		void ForceUpdate();

	private:

		void ShowMap( const std::string& name );
		void AddMap( std::shared_ptr<sbe::MapPlotter>& M );

		void CreateActor( const std::string& name );
		void RemoveActor( const std::string& name );

		sfg::Frame::Ptr CurrentFrame;
		sfg::Box::Ptr myBox;

		std::map<std::string, std::pair<std::shared_ptr<sbe::MapPlotter>, std::shared_ptr<sbe::Actor>>> Maps;
		sbe::sfgList myOverlays;

};

#endif // OVERLAYS_HPP


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
	class Box;
	class Entry;
	class Image;
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

		/// initializer for the Window
		void CreateWindow( const Geom::Vec2 Size );

		/// called from eventhandler, adding a new graph and optionBox
		void AddNewGraph( std::string displayName, std::shared_ptr<sbe::GraphPlotter> graph );

		/// redraw the graphplotter to the image
		void PlotGraph();

		/// rearange the sfg::window
		void updatePosition();

		/// signal-connection to make hviewingrangebox visible
		void hViewingRange();

		/// signal-connection to make vviewingrangebox visible
		void vViewingRange();

		/// spellchecker (only numbers are allowed)
		void EntryTextChange();

		/// called whenever the graphplotter is updated or modified
		void UpdateGraphSettings();

		/// called from event-handler on enter, executing the changes from the active entry
		void handleEntryInput( int entry );

		/// determines if listenling to enter (should be active when an entry is active)
		int ListenToActionKeys;

		/// flag to avoid spellchecking while spellchecking
		bool textchangeavoidrecursive;


		/// signal-connection for entry
		void HViewingRangeFromEntryGainFocus();

		/// signal-connection for entry
		void HViewingRangeToEntryGainFocus();

		/// signal-connection for entry
		void VViewingRangeFromEntryGainFocus();

		/// signal-connection for entry
		void VViewingRangeToEntryGainFocus();

		/// signal-connection for entry
		void EntryLostFocus();

		/// Deque where all pointer-tuples from the different Graphs are saved
		std::deque< std::tuple<
				std::shared_ptr<sbe::GraphPlotter>,
				sfg::SharedPtr<sfg::Image>,
				sfg::SharedPtr<sfg::Box>,
				sfg::SharedPtr<sfg::Box>,
				sfg::SharedPtr<sfg::Entry>,
				sfg::SharedPtr<sfg::Entry>,
				sfg::SharedPtr<sfg::Entry>,
				sfg::SharedPtr<sfg::Entry> > > graphPlotterList;


		/// I will name the this function "Current-Tab-Tuple", because it will give you the tuple from the list at the current tab of the graphbook.
		std::tuple<
				std::shared_ptr<sbe::GraphPlotter>,
				sfg::SharedPtr<sfg::Image>,
				sfg::SharedPtr<sfg::Box>,
				sfg::SharedPtr<sfg::Box>,
				sfg::SharedPtr<sfg::Entry>,
				sfg::SharedPtr<sfg::Entry>,
				sfg::SharedPtr<sfg::Entry>,
				sfg::SharedPtr<sfg::Entry> > cTT();

		/// The SFGUI-Window (Parent-Widget) of GraphBook
		sfg::SharedPtr<sfg::Window> Win;

		/// The Notebook in the Window
		sfg::SharedPtr<sfg::Notebook> Tabs;

		/// Clock for auto-updating the visible graph
		sf::Clock UpdateTimer;
};

#endif // GRAPHBOOK_H


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

		/**
			Handles (data):
			ADD_GRAPH_TO_BOOK : add a new graphplotter to the list (shared_ptr<GraphPlotter>)
		*/
		virtual void HandleEvent( Event& e );

	private:

		/// tuple in which all pointers are saved
		struct graphTuple
		{
			std::shared_ptr<sbe::GraphPlotter> plotter;
			sfg::SharedPtr <sfg::Image>        image;
			sfg::SharedPtr <sfg::Box>          hBox;
			sfg::SharedPtr <sfg::Box>          vBox;
			sfg::SharedPtr <sfg::Entry>        hFrom;
			sfg::SharedPtr <sfg::Entry>        hTo;
			sfg::SharedPtr <sfg::Entry>        vFrom;
			sfg::SharedPtr <sfg::Entry>        vTo;
			Geom::Point              hlimit;
			Geom::Point              vlimit;

				/// constructor for graphTuple
				graphTuple( std::shared_ptr<sbe::GraphPlotter> _plotter,
				            sfg::SharedPtr <sfg::Image>        _image,
				            sfg::SharedPtr <sfg::Box>          _hBox,
				            sfg::SharedPtr <sfg::Box>          _vBox,
				            sfg::SharedPtr <sfg::Entry>        _hFrom,
				            sfg::SharedPtr <sfg::Entry>        _hTo,
				            sfg::SharedPtr <sfg::Entry>        _vFrom,
				            sfg::SharedPtr <sfg::Entry>        _vTo )
					{
						plotter = _plotter;
						image   = _image;
						hBox    = _hBox;
						vBox    = _vBox;
						hFrom   = _hFrom;
						hTo     = _hTo;
						vFrom   = _vFrom;
						vTo     = _vTo;
						hlimit = {0,0};
						vlimit = {0,0};
					}
		};

		/// initializer for the Window
		void CreateWindow( const Geom::Vec2 Size );

		/// called from eventhandler, adding a new graph and optionBox
		void AddNewGraph( std::string displayName, std::shared_ptr<sbe::GraphPlotter> graph );

		/// redraw the graphplotter to the image
		void PlotGraph( graphTuple& GT );
		void PlotCurrentGraph( );

		/// rearange the sfg::window
		void updatePosition();

		/// signal-connection to make hviewingrangebox visible
		void hViewingRange();

		/// signal-connection to make vviewingrangebox visible
		void vViewingRange();

		/// spellchecker (only numbers are allowed)
		void EntryTextChange();

		/// called whenever the graphplotter is updated or modified
		void UpdateGraphSettings( graphTuple& GT );

		/// check if Graphbook has a valid tab
		bool hasValidTab();

		/// called from event-handler on enter, executing the changes from the active entry
		void handleEntryInput( int entry );

		///
		int entryVal( sfg::SharedPtr <sfg::Entry>& E  );

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

		/// Deque where all graphtuples are saved
		std::deque<graphTuple> graphTupleList;


		/// I will name the this function "Current-Tab-Tuple", because it will give you the tuple from the list at the current tab of the graphbook.
		graphTuple& cTT();

		/// The SFGUI-Window (Parent-Widget) of GraphBook
		sfg::SharedPtr<sfg::Window> Win;

		/// The Notebook in the Window
		sfg::SharedPtr<sfg::Notebook> Tabs;

		/// Clock for auto-updating the visible graph
		sf::Clock UpdateTimer;
};

#endif // GRAPHBOOK_H


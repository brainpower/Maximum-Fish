#include "GraphBook.hpp"

#include <SFGUI/Window.hpp>
#include <SFGUI/Notebook.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Image.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/RadioButton.hpp>
#include <SFGUI/Entry.hpp>

using namespace sfg;

GraphBook::GraphBook( const Geom::Vec2 Size )
 : ListenToActionKeys( 0 ),
 textchangeavoidrecursive( true )
{
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "TOGGLE_SHOW_GRAPHBOOK" );
	RegisterForEvent( "ADD_GRAPH_TO_BOOK" );
	RegisterForEvent( "KEY_PRESSED_ENTER" );
	RegisterForEvent( "debug_reload_graph" );
	CreateWindow( Size );
	Win->Show( false );
}

void GraphBook::CreateWindow( const Geom::Vec2 Size )
{
	Win = Window::Create( Window::Style::BACKGROUND | Window::Style::TITLEBAR | Window::Style::RESIZE | Window::Style::SHADOW );
	Win->SetTitle( "GraphBook" );
		Tabs = Notebook::Create();
		Tabs->SetTabPosition( Notebook::TabPosition::LEFT );
	Win->Add( Tabs );
	Win->SetRequisition( sf::Vector2f( Size.x, Size.y ) );
	updatePosition();

	Module::Get()->QueueEvent( Event( "SCREEN_ADD_WINDOW", Win ) );
}

void GraphBook::HandleEvent( Event& e )
{
	if ( UpdateTimer.getElapsedTime() > sf::seconds( 2.0 ) && Win->IsGloballyVisible() )
	{
		UpdateGraphSettings(cTT());
		PlotGraph(cTT());
		UpdateTimer.restart();
	}
	else if ( e.Is( "TOGGLE_SHOW_GRAPHBOOK" ) )
	{
		if ( Win->IsGloballyVisible() )
		{
			Win->Show( false );
		}
		else
		{
			updatePosition();
			PlotGraph(cTT());//not realy clicked, but for updating the current tab
			Win->Show( true );
			Win->GrabFocus();
			UpdateTimer.restart();
		}
	}
	else if ( e.Is( "ADD_GRAPH_TO_BOOK", typeid( std::pair<std::string, std::shared_ptr<sbe::GraphPlotter>> ) ) )
	{
		auto x = boost::any_cast<std::pair<std::string, std::shared_ptr<sbe::GraphPlotter>>>( e.Data() );
		AddNewGraph( x.first, x.second );
	}
	else if ( e.Is( "KEY_PRESSED_ENTER" ) && ListenToActionKeys )
	{
		handleEntryInput( ListenToActionKeys );
		//reviewEntryInput();
	}
	else if ( e.Is( "debug_reload_graph" ) )
	{
		UpdateGraphSettings(cTT());
		PlotGraph(cTT());
	}
}

void GraphBook::EntryTextChange()
{
	if ( !hasValidTab() )
		return;
	if ( !textchangeavoidrecursive )
	{
		SharedPtr<Entry> entry;
		switch ( ListenToActionKeys )
		{
		case 1:
			entry = cTT().hFrom;
			break;
		case 2:
			entry = cTT().hTo;
			break;
		case 3:
			entry = cTT().vFrom;
			break;
		case 4:
			entry = cTT().vTo;
			break;
		default:
			Engine::out( Engine::INFO ) << "[GraphBook] Warning!! Text from unidentified Entry changed." << std::endl;
			return;
		}
		std::string text = entry->GetText().toAnsiString();
		int cursorPos = entry->GetCursorPosition();
		if ( cursorPos != 0 && !(std::isdigit( text[cursorPos - 1] ) || text[cursorPos -1] == '-' ) )
		{
			text.erase( cursorPos-1 , 1 );
			cursorPos--;
		}
		textchangeavoidrecursive = true;
		entry->SetText( text );
		entry->SetCursorPosition( cursorPos );
	}
	else
	{
		textchangeavoidrecursive = false;
	}
}
///@TODO: set for current tab
void GraphBook::hViewingRange()
{
	if ( !hasValidTab() )
		return;
	cTT().hBox->Show( !cTT().hBox->IsGloballyVisible() );
	UpdateGraphSettings(cTT());
}

void GraphBook::vViewingRange()
{
	if ( !hasValidTab() )
		return;
	cTT().vBox->Show( !cTT().vBox->IsGloballyVisible() );
	UpdateGraphSettings(cTT());
}

void GraphBook::HViewingRangeFromEntryGainFocus()
{
	ListenToActionKeys = 1;
}

void GraphBook::HViewingRangeToEntryGainFocus()
{
	ListenToActionKeys = 2;
}

void GraphBook::VViewingRangeFromEntryGainFocus()
{
	ListenToActionKeys = 3;
}

void GraphBook::VViewingRangeToEntryGainFocus()
{
	ListenToActionKeys = 4;
}

void GraphBook::EntryLostFocus()
{
	ListenToActionKeys = 0;
}

void GraphBook::handleEntryInput( int entry )
{
	if ( !hasValidTab() )
		return;

	auto& curTuple = cTT();

	// just load all entrys
	Geom::Point hlimits  = { entryVal(curTuple.hFrom), entryVal(curTuple.hTo) };
	Geom::Point vlimits  = { entryVal(curTuple.vFrom), entryVal(curTuple.vTo) };
	if ( hlimits.x > hlimits.y ) hlimits.x = hlimits.y;
	if ( vlimits.x > vlimits.y ) vlimits.x = vlimits.y;

	curTuple.hlimit = hlimits;
	curTuple.vlimit = vlimits;

	// set new values in GraphPlotter
	UpdateGraphSettings(curTuple);
	textchangeavoidrecursive = true;
}

void GraphBook::AddNewGraph( std::string displayName, std::shared_ptr<sbe::GraphPlotter> graph )
{
	Box::Ptr box = Box::Create( Box::Orientation::VERTICAL, 3.0f );
		Image::Ptr I = Image::Create();
	box->Pack( I, true, true );
		Box::Ptr spacer = Box::Create();
		//box without requisition will not expand
		spacer->SetRequisition( sf::Vector2f( 1, 1 ) );
	box->Pack( spacer, true, true );
		Box::Ptr optionBox = Box::Create( Box::Orientation::HORIZONTAL, 3.0f );
			Box::Ptr hAxisBox = Box::Create( Box::Orientation::VERTICAL, 3.0f );
				RadioButton::Ptr hViewingRangeAll = RadioButton::Create( "auto-adjust horizontal axis." );
				hViewingRangeAll->SetActive( true );
				RadioButton::Ptr hViewingRangeSelection = RadioButton::Create( "show selection on horizontal axis", hViewingRangeAll->GetGroup() );
				hViewingRangeAll->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::hViewingRange, this );
				Box::Ptr hViewingRangeBox = Box::Create( Box::Orientation::HORIZONTAL );
					Entry::Ptr hViewingRangeFromEntry = Entry::Create( "0" );
						hViewingRangeFromEntry->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::HViewingRangeFromEntryGainFocus , this );
						hViewingRangeFromEntry->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						hViewingRangeFromEntry->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
					Entry::Ptr hViewingRangeToEntry = Entry::Create( "5000" );
						hViewingRangeToEntry->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::HViewingRangeToEntryGainFocus , this );
						hViewingRangeToEntry->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						hViewingRangeToEntry->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
				hViewingRangeBox->Pack( hViewingRangeFromEntry, true, true );
				hViewingRangeBox->Pack( Label::Create( "-" ), false, false );
				hViewingRangeBox->Pack( hViewingRangeToEntry, true, true );
				hViewingRangeBox->Show( false );
			hAxisBox->Pack( hViewingRangeAll, false, false );
			hAxisBox->Pack( hViewingRangeSelection, false, false );
			hAxisBox->Pack( hViewingRangeBox, false, false );
		optionBox->Pack( hAxisBox );
			Box::Ptr vAxisBox = Box::Create( Box::Orientation::VERTICAL, 3.0f );
				RadioButton::Ptr vViewingRangeAll = RadioButton::Create( "auto-adjust vertical axis." );
				vViewingRangeAll->SetActive( true );
				RadioButton::Ptr vViewingRangeSelection = RadioButton::Create( "show selection on vertical axis", vViewingRangeAll->GetGroup() );
				vViewingRangeAll->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::vViewingRange, this );
				Box::Ptr vViewingRangeBox = Box::Create( Box::Orientation::HORIZONTAL );
					Entry::Ptr vViewingRangeFromEntry = Entry::Create( "0" );
						vViewingRangeFromEntry->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::VViewingRangeFromEntryGainFocus , this );
						vViewingRangeFromEntry->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						vViewingRangeFromEntry->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
					Entry::Ptr vViewingRangeToEntry = Entry::Create( "5000" );
						vViewingRangeToEntry->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::VViewingRangeToEntryGainFocus , this );
						vViewingRangeToEntry->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						vViewingRangeToEntry->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
				vViewingRangeBox->Pack( vViewingRangeFromEntry, true, true );
				vViewingRangeBox->Pack( Label::Create( "-" ), false, false );
				vViewingRangeBox->Pack( vViewingRangeToEntry, true, true );
				vViewingRangeBox->Show( false );
			vAxisBox->Pack( vViewingRangeAll, false, false );
			vAxisBox->Pack( vViewingRangeSelection, false, false );
			vAxisBox->Pack( vViewingRangeBox, false, false );
		optionBox->Pack( vAxisBox );
		//uncomment the following line to ensure space for axis-range-entrys even if they are not visible
		//optionBox->SetRequisition( sf::Vector2f( optionBox->GetAllocation().width, optionBox->GetAllocation().height ) );
	box->Pack( optionBox, false, false );
	Label::Ptr label = Label::Create( displayName );
	label->GetSignal( Label::OnLeftClick ).Connect( &GraphBook::PlotCurrentGraph, this );
	Tabs->SetCurrentPage( Tabs->AppendPage( box, label ) );

	auto y = graphTuple( graph, I, hViewingRangeBox, vViewingRangeBox,
	                          hViewingRangeFromEntry, hViewingRangeToEntry,
	                          vViewingRangeFromEntry, vViewingRangeToEntry );
	y.hlimit.x = 0;
	y.hlimit.y = 5000;
	y.vlimit.x = 0;
	y.vlimit.y = 5000;

	graphTupleList.push_back( y );
	Engine::out() << "setting new graph" << std::endl;
	UpdateGraphSettings( y );

	Engine::out() << "set new graph" << std::endl;
}

void GraphBook::UpdateGraphSettings( graphTuple& GT )
{
	auto G = GT.plotter;
	if ( !G )
		return;

	Geom::Vec2 start = Geom::Vec2( GT.hlimit.x, GT.vlimit.x );
	Geom::Vec2 stop  = Geom::Vec2( GT.hlimit.y,   GT.vlimit.y   );
	G->getGraph().AxisStart = start;
	G->getGraph().AxisSize.x  = ( ((start.x<0)?stop.x:stop.x - start.x) );
	G->getGraph().AxisSize.y  = ( ((start.y<0)?stop.y:stop.y - start.y) );
	G->getGraph().dynX = !( GT.hBox->IsGloballyVisible() );
	G->getGraph().dynY = !( GT.vBox->IsGloballyVisible() );
	G->updateVertexArrays();
	G->printSettings();
	PlotGraph(GT);
}

GraphBook::graphTuple& GraphBook::cTT()
{
	/// the way to prevent return value if current page = -1 is to check hasValidTab() first
	int i = Tabs->GetCurrentPage();
	return graphTupleList.at( i );
}

void GraphBook::PlotCurrentGraph() { PlotGraph(cTT()); }

void GraphBook::PlotGraph( graphTuple& GT )
{
	auto G = GT.plotter;

	if ( !G ) {
		Engine::out( Engine::ERROR ) << "[GraphBook::PlotGraph] INVALID POINTER!" << std::endl;
		return;
	}
	if ( !G->isValid() )
	{
		Engine::out() << "[GraphBook::PlotGraph] GraphPlotter is not valid." << std::endl;
		return;
	}

	sf::RenderTexture tex;
	tex.create( G->getGraph().Size.x, G->getGraph().Size.y );
	G->draw( tex );
	tex.display();
	GT.image->SetImage( tex.getTexture().copyToImage() );
}

bool GraphBook::hasValidTab()
{
	return ( Tabs->GetCurrentPage() >= 0 );
}

void GraphBook::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , ( winSize.y - Allocation.height ) ) );
}

int GraphBook::entryVal( sfg::SharedPtr <sfg::Entry>& E )
{
	int value = boost::lexical_cast<int>( E->GetText().toAnsiString().empty() ? "0" : E->GetText().toAnsiString() );
	E->SetText( boost::lexical_cast<std::string>( value ) );
	E->SetCursorPosition( 99 );
	return value;
}

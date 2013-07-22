#include "GraphBook.hpp"

#include <SFGUI/Window.hpp>
#include <SFGUI/Notebook.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Image.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/RadioButton.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Separator.hpp>

#include <boost/lexical_cast.hpp>

#include <string>

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
	if ( UpdateTimer.getElapsedTime() > sf::seconds( 1.0 ) && Win->IsGloballyVisible() )
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
		handleEntryInput();
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
		case 5:
			entry = cTT().hEndEntry;
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

void GraphBook::hViewingRangeAllToggle()
{
	if ( !hasValidTab() )
		return;
	auto t = cTT();
	UpdateGraphSettings( t );
}

void GraphBook::hViewingRangeSelectionToggle()
{
	if ( !hasValidTab() )
		return;
	auto t = cTT();
	t.hRangeBox->Show( !t.hRangeBox->IsGloballyVisible() );
	UpdateGraphSettings( t );
}

void GraphBook::hViewingRangeEndToggle()
{
	if ( !hasValidTab() )
		return;
	auto t = cTT();
	t.hEndBox->Show( !t.hEndBox->IsGloballyVisible() );
	UpdateGraphSettings( t );
}

void GraphBook::vViewingRange()
{
	if ( !hasValidTab() )
		return;
	auto t = cTT();
	t.vRangeBox->Show( !t.vRangeBox->IsGloballyVisible() );
	UpdateGraphSettings( t );
}

void GraphBook::vViewingLogarithmicToggle()
{
	if ( !hasValidTab() )
		return;
	auto t = cTT();
	t.vLogAxBox->Show( t.vLogAxBtn->IsActive() );
	UpdateGraphSettings( t );
}

void GraphBook::HViewingRangeFromEntryGainFocus()
{
	ListenToActionKeys = 1;
}

void GraphBook::HViewingRangeToEntryGainFocus()
{
	ListenToActionKeys = 2;
}

void GraphBook::HViewingRangeEndEntryGainFocus()
{
	ListenToActionKeys = 5;
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

void GraphBook::handleEntryInput()
{
	if ( !hasValidTab() )
		return;

	auto& curTuple = cTT();

	// just load all entrys
	Geom::Point hlimits = { entryVal(curTuple.hFrom), entryVal(curTuple.hTo) };
	Geom::Point vlimits = { entryVal(curTuple.vFrom), entryVal(curTuple.vTo) };
	if ( hlimits.x > hlimits.y ) hlimits.x = hlimits.y;
	if ( vlimits.x > vlimits.y ) vlimits.x = vlimits.y;

	curTuple.hLimit = hlimits;
	curTuple.vLimit = vlimits;
	curTuple.hEndOffset = entryVal( curTuple.hEndEntry );

	// set new values in GraphPlotter
	UpdateGraphSettings( curTuple );
	textchangeavoidrecursive = true;
}

void GraphBook::AddNewGraph( std::string displayName, std::shared_ptr<sbe::GraphPlotter> graph )
{
	auto t = graphTuple( 0, 5000, 0, 40000, 50 );
	t.plotter = graph;
	Box::Ptr box = Box::Create( Box::Orientation::VERTICAL, 3.0f );
		t.image = Image::Create();
	box->Pack( t.image, false, false );
		Box::Ptr spacer = Box::Create();
		//box without requisition will not expand
		spacer->SetRequisition( sf::Vector2f( 1, 1 ) );
	box->Pack( spacer, true, true );
		Box::Ptr optionBox = Box::Create( Box::Orientation::HORIZONTAL, 3.0f );
			Box::Ptr hAxisBox = Box::Create( Box::Orientation::VERTICAL, 3.0f );
				t.hRB0 = RadioButton::Create( "auto-adjust horizontal axis." );
				t.hRB0->SetActive( true );
				t.hRB0->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::hViewingRangeAllToggle, this );
				t.hRB1 = RadioButton::Create( "show selection on horizontal axis", t.hRB0->GetGroup() );
				t.hRB1->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::hViewingRangeSelectionToggle, this );
				t.hRangeBox = Box::Create( Box::Orientation::HORIZONTAL );
					t.hFrom = Entry::Create( boost::lexical_cast<std::string>( t.hLimit.x ) );
						t.hFrom->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::HViewingRangeFromEntryGainFocus , this );
						t.hFrom->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						t.hFrom->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
					t.hTo = Entry::Create( boost::lexical_cast<std::string>( t.hLimit.y ) );
						t.hTo->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::HViewingRangeToEntryGainFocus , this );
						t.hTo->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						t.hTo->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
				t.hRangeBox->Pack( t.hFrom, true, true );
				t.hRangeBox->Pack( Label::Create( "-" ), false, false );
				t.hRangeBox->Pack( t.hTo, true, true );
				t.hRangeBox->Show( false );
				t.hRB2 = RadioButton::Create( "show only the last", t.hRB0->GetGroup() );
				t.hRB2->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::hViewingRangeEndToggle, this );
				t.hEndBox = Box::Create( Box::Orientation::HORIZONTAL );
					t.hEndEntry = Entry::Create( boost::lexical_cast<std::string>( t.hEndOffset ) );
						t.hEndEntry->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::HViewingRangeEndEntryGainFocus , this );
						t.hEndEntry->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						t.hEndEntry->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
				t.hEndBox->Pack( t.hEndEntry, true, true );
				t.hEndBox->Pack( Label::Create( "  Ticks" ), true, true );
				t.hEndBox->Show( false );
			hAxisBox->Pack( t.hRB0, false, false );
			hAxisBox->Pack( t.hRB1, false, false );
			hAxisBox->Pack( t.hRangeBox, false, false );
			hAxisBox->Pack( t.hRB2, false, false );
			hAxisBox->Pack( t.hEndBox, false, false );
		optionBox->Pack( hAxisBox );
			Box::Ptr vAxisBox = Box::Create( Box::Orientation::VERTICAL, 3.0f );
				t.vRB0 = RadioButton::Create( "auto-adjust vertical axis." );
				t.vRB0->SetActive( true );
				t.vRB1 = RadioButton::Create( "show selection on vertical axis", t.vRB0->GetGroup() );
				t.vRB0->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::vViewingRange, this );
				t.vRangeBox = Box::Create( Box::Orientation::HORIZONTAL );
					t.vFrom = Entry::Create( boost::lexical_cast<std::string>(t.vLimit.x ) );
						t.vFrom->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::VViewingRangeFromEntryGainFocus , this );
						t.vFrom->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						t.vFrom->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
					t.vTo = Entry::Create( boost::lexical_cast<std::string>( t.vLimit.y ) );
						t.vTo->GetSignal( Entry::OnGainFocus ).Connect( &GraphBook::VViewingRangeToEntryGainFocus , this );
						t.vTo->GetSignal( Entry::OnLostFocus ).Connect( &GraphBook::EntryLostFocus , this );
						t.vTo->GetSignal( Entry::OnTextChanged ).Connect( &GraphBook::EntryTextChange , this );
				t.vRangeBox->Pack( t.vFrom, true, true );
				t.vRangeBox->Pack( Label::Create( "-" ), false, false );
				t.vRangeBox->Pack( t.vTo, true, true );
				t.vRangeBox->Show( false );
				t.vLogAxBtn = CheckButton::Create( "Make vertical axis logarithmic" );
				t.vLogAxBtn->GetSignal( CheckButton::OnToggle ).Connect( &GraphBook::vViewingLogarithmicToggle, this );
				t.vLogAxBox = Box::Create( Box::Orientation::HORIZONTAL, 3.0f );
					t.logBase2 = RadioButton::Create( "2     " );
					t.logBase2->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::vViewingLogarithmicToggle, this );
					t.logBaseE = RadioButton::Create( "e     ", t.logBase2->GetGroup() );
					t.logBaseE->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::vViewingLogarithmicToggle, this );
					t.logBase10 = RadioButton::Create( "10     ", t.logBase2->GetGroup() );
					t.logBase10->GetSignal( RadioButton::OnToggle ).Connect( &GraphBook::vViewingLogarithmicToggle, this );
					t.logBase10->SetActive( true );
				t.vLogAxBox->Pack( Label::Create( "Base:   " ), false, false );
				t.vLogAxBox->Pack( t.logBase2, false, false );
				t.vLogAxBox->Pack( t.logBaseE, false, false );
				t.vLogAxBox->Pack( t.logBase10, false, false );
				t.vLogAxBox->Show( false );
			vAxisBox->Pack( t.vRB0, false, false );
			vAxisBox->Pack( t.vRB1, false, false );
			vAxisBox->Pack( t.vRangeBox, false, false );
			vAxisBox->Pack( Separator::Create(), false, false );
			vAxisBox->Pack( t.vLogAxBtn, false, false );
			vAxisBox->Pack( t.vLogAxBox, false, false );
		optionBox->Pack( Separator::Create( Separator::Orientation::VERTICAL ), false, false );
		optionBox->Pack( vAxisBox, false, false );
		//uncomment the following line to ensure space for axis-range-entrys even if they are not visible
		//optionBox->SetRequisition( sf::Vector2f( optionBox->GetAllocation().width, optionBox->GetAllocation().height ) );
	box->Pack( optionBox, false, false );
	Label::Ptr label = Label::Create( displayName );
	label->GetSignal( Label::OnLeftClick ).Connect( &GraphBook::PlotCurrentGraph, this );
	Tabs->SetCurrentPage( Tabs->AppendPage( box, label ) );

	graphTupleList.push_back( t );
	Engine::out() << "setting new graph" << std::endl;
	UpdateGraphSettings( t );

	Engine::out() << "set new graph" << std::endl;
}

void GraphBook::UpdateGraphSettings( graphTuple& GT )
{
	auto G = GT.plotter;
	if ( !G )
		return;

	Geom::Vec2 startCorner = Geom::Vec2( GT.hLimit.x, GT.vLimit.x );
	Geom::Vec2 stopCorner  = Geom::Vec2( GT.hLimit.y, GT.vLimit.y );
	G->getGraph().AxisStart = startCorner;
	int activeRadioButton = ( GT.hRB0->IsActive() ? 0 : ( GT.hRB1->IsActive() ? 1 : 2 ) );
	G->getGraph().dynX = ( activeRadioButton == 0 );
	G->getGraph().AxisSize.x = ( activeRadioButton == 1 ? ( stopCorner.x - startCorner.x ) : GT.hEndOffset );
	if ( activeRadioButton == 2 )
		G->getGraph().AxisStart.x = -1 * GT.hEndOffset;
	//----
	activeRadioButton = ( GT.vRB0->IsActive() ? 0 : 1 );
	G->getGraph().dynY = ( activeRadioButton == 0 );
	G->getGraph().AxisSize.y = stopCorner.y - startCorner.y;
	G->getGraph().logScale = ( GT.vLogAxBtn->IsActive() );
	G->getGraph().logBase = ( GT.logBase2->IsActive() ? 2 : ( GT.logBaseE->IsActive() ? 2.71828 : 10 ) );

	G->getGraph().logScale = GT.vLogAxBtn->IsActive();

	G->updateVertexArrays();
	//G->printSettings();
	PlotGraph( GT );
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

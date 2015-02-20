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

#include "sbe/Config.hpp"

using namespace sfg;

GraphBook::GraphBook()
 : ListenToActionKeys( 0 ),
 textchangeavoidrecursive( true )
{
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "TOGGLE_SHOW_GRAPHBOOK" );
	RegisterForEvent( "ADD_GRAPH_TO_BOOK" );
	RegisterForEvent( "KEY_PRESSED_ENTER" );
	RegisterForEvent( "RESET_UI" );
	RegisterForEvent( "debug_reload_graph" );
	InitWindow( glm::ivec2( Engine::getCfg()->get<int>("sim.plots.size.x")+60,Engine::getCfg()->get<int>("sim.plots.size.y")+40 ) );
	Win->Show( false );
}

void GraphBook::InitWindow( const glm::ivec2 Size )
{
	Win = Window::Create( Window::Style::BACKGROUND | Window::Style::TITLEBAR | Window::Style::RESIZE | Window::Style::SHADOW );
	Win->SetTitle( "GraphBook" );
		Tabs = Notebook::Create();
		Tabs->SetTabPosition( Notebook::TabPosition::LEFT );
		// add an empty label, notebook requires this
		Label::Ptr label = Label::Create( "No graph" );
        Label::Ptr label2 = Label::Create( "There are no graphs available at this time." );
        Tabs->SetCurrentPage( Tabs->AppendPage( label2, label ) );
	Win->Add( Tabs );
	Win->SetRequisition( sf::Vector2f( Size.x, Size.y ) );
	updatePosition();

	Module::Get()->QueueEvent( Event( "SCREEN_ADD_WINDOW", Win ) );
}

void GraphBook::HandleEvent( Event& e )
{
	if ( UpdateTimer.getElapsedTime() > sf::seconds( 1.0 ) && Win->IsGloballyVisible() )
	{
	    if ( hasValidTab() )
        {
            UpdateGraphSettings(cTT());
            PlotGraph(cTT());
        }
		UpdateTimer.restart();
	}

	if ( e.Is( "TOGGLE_SHOW_GRAPHBOOK" ) )
	{
		if ( Win->IsGloballyVisible() )
		{
			Win->Show( false );
		}
		else
		{
			if ( hasValidTab() )
                PlotGraph( cTT() ); //not really clicked, but for updating the current tab
			Win->Show( true );
			Win->GrabFocus();
			updatePosition();
			UpdateTimer.restart();
		}
	}
	else if ( e.Is( "ADD_GRAPH_TO_BOOK", typeid( std::pair<std::string, std::shared_ptr<sbe::GraphPlotter>> ) ) )
	{
	    //Engine::out() << "[GraphBook] Got ADD_GRAPH_TO_BOOK Event" << std::endl;
		auto x = boost::any_cast<std::pair<std::string, std::shared_ptr<sbe::GraphPlotter>>>( e.Data() );
		AddNewGraph( x.first, x.second );
	}
	else if ( e.Is( "KEY_PRESSED_ENTER" ) && ListenToActionKeys )
	{
		handleEntryInput();
		//reviewEntryInput();
	}
	else if ( e.Is( "RESET_UI" ))
	{
	    Engine::out() << "[GraphBook] clearing." << std::endl;
	    clearGraphs();
	}
	else if ( e.Is( "debug_reload_graph" ) )
	{
	    if ( !hasValidTab() ) return;

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
		std::shared_ptr<Entry> entry;
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
	glm::ipoint2 hlimits = { entryVal(curTuple.hFrom), entryVal(curTuple.hTo) };
	glm::ipoint2 vlimits = { entryVal(curTuple.vFrom), entryVal(curTuple.vTo) };
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
				t.hRB0->GetSignal( RadioButton::OnToggle ).Connect( std::bind( &GraphBook::hViewingRangeAllToggle, this ));
				t.hRB1 = RadioButton::Create( "show selection on horizontal axis", t.hRB0->GetGroup() );
				t.hRB1->GetSignal( RadioButton::OnToggle ).Connect( std::bind( &GraphBook::hViewingRangeSelectionToggle, this ));
				t.hRangeBox = Box::Create( Box::Orientation::HORIZONTAL );
					t.hFrom = Entry::Create( boost::lexical_cast<std::string>( t.hLimit.x ) );
						t.hFrom->GetSignal( Entry::OnGainFocus ).Connect( std::bind( &GraphBook::HViewingRangeFromEntryGainFocus , this ));
						t.hFrom->GetSignal( Entry::OnLostFocus ).Connect( std::bind( &GraphBook::EntryLostFocus , this ));
						t.hFrom->GetSignal( Entry::OnTextChanged ).Connect( std::bind( &GraphBook::EntryTextChange , this ));
					t.hTo = Entry::Create( boost::lexical_cast<std::string>( t.hLimit.y ) );
						t.hTo->GetSignal( Entry::OnGainFocus ).Connect( std::bind( &GraphBook::HViewingRangeToEntryGainFocus , this ));
						t.hTo->GetSignal( Entry::OnLostFocus ).Connect( std::bind( &GraphBook::EntryLostFocus , this ));
						t.hTo->GetSignal( Entry::OnTextChanged ).Connect( std::bind( &GraphBook::EntryTextChange , this ));
				t.hRangeBox->Pack( t.hFrom, true, true );
				t.hRangeBox->Pack( Label::Create( "-" ), false, false );
				t.hRangeBox->Pack( t.hTo, true, true );
				t.hRangeBox->Show( false );
				t.hRB2 = RadioButton::Create( "show only the last", t.hRB0->GetGroup() );
				t.hRB2->GetSignal( RadioButton::OnToggle ).Connect( std::bind( &GraphBook::hViewingRangeEndToggle, this ));
				t.hEndBox = Box::Create( Box::Orientation::HORIZONTAL );
					t.hEndEntry = Entry::Create( boost::lexical_cast<std::string>( t.hEndOffset ) );
						t.hEndEntry->GetSignal( Entry::OnGainFocus ).Connect( std::bind( &GraphBook::HViewingRangeEndEntryGainFocus , this ));
						t.hEndEntry->GetSignal( Entry::OnLostFocus ).Connect( std::bind( &GraphBook::EntryLostFocus , this ));
						t.hEndEntry->GetSignal( Entry::OnTextChanged ).Connect( std::bind( &GraphBook::EntryTextChange , this ));
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
				t.vRB0->GetSignal( RadioButton::OnToggle ).Connect( std::bind( &GraphBook::vViewingRange, this ));
				t.vRangeBox = Box::Create( Box::Orientation::HORIZONTAL );
					t.vFrom = Entry::Create( boost::lexical_cast<std::string>(t.vLimit.x ) );
						t.vFrom->GetSignal( Entry::OnGainFocus ).Connect( std::bind( &GraphBook::VViewingRangeFromEntryGainFocus , this ));
						t.vFrom->GetSignal( Entry::OnLostFocus ).Connect( std::bind( &GraphBook::EntryLostFocus , this ));
						t.vFrom->GetSignal( Entry::OnTextChanged ).Connect( std::bind( &GraphBook::EntryTextChange , this ));
					t.vTo = Entry::Create( boost::lexical_cast<std::string>( t.vLimit.y ) );
						t.vTo->GetSignal( Entry::OnGainFocus ).Connect( std::bind( &GraphBook::VViewingRangeToEntryGainFocus , this ));
						t.vTo->GetSignal( Entry::OnLostFocus ).Connect( std::bind( &GraphBook::EntryLostFocus , this ));
						t.vTo->GetSignal( Entry::OnTextChanged ).Connect( std::bind( &GraphBook::EntryTextChange , this ));
				t.vRangeBox->Pack( t.vFrom, true, true );
				t.vRangeBox->Pack( Label::Create( "-" ), false, false );
				t.vRangeBox->Pack( t.vTo, true, true );
				t.vRangeBox->Show( false );
				t.vLogAxBtn = CheckButton::Create( "Make vertical axis logarithmic" );
				t.vLogAxBtn->GetSignal( CheckButton::OnToggle ).Connect( std::bind( &GraphBook::vViewingLogarithmicToggle, this ));
			vAxisBox->Pack( t.vRB0, false, false );
			vAxisBox->Pack( t.vRB1, false, false );
			vAxisBox->Pack( t.vRangeBox, false, false );
			vAxisBox->Pack( Separator::Create(), false, false );
			vAxisBox->Pack( t.vLogAxBtn, false, false );
		optionBox->Pack( Separator::Create( Separator::Orientation::VERTICAL ), false, false );
		optionBox->Pack( vAxisBox, false, false );
		//uncomment the following line to ensure space for axis-range-entrys even if they are not visible
		//optionBox->SetRequisition( sf::Vector2f( optionBox->GetAllocation().width, optionBox->GetAllocation().height ) );
	box->Pack( optionBox, false, false );
	Label::Ptr label = Label::Create( displayName );
	label->GetSignal( Label::OnLeftClick ).Connect( std::bind( &GraphBook::PlotCurrentGraph, this ));


	Tabs->SetCurrentPage( Tabs->AppendPage( box, label ) );

    // first Tab, remove the placeholder
    Label::Ptr tmp = std::dynamic_pointer_cast<Label>( Tabs->GetNthTabLabel(0) );
    if ( Tabs->GetPageCount() == 2 && tmp->GetText() == "No graph" )
        Tabs->RemovePage(0);


	graphTupleList.push_back( t );
	UpdateGraphSettings( t );

	Engine::out() << "[GraphBook] New Graph: " << displayName << std::endl;
}

void GraphBook::UpdateGraphSettings( graphTuple& GT )
{
	auto G = GT.plotter;
	if ( !G )
		return;

	glm::ivec2 startCorner = glm::ivec2( GT.hLimit.x, GT.vLimit.x );
	glm::ivec2 stopCorner  = glm::ivec2( GT.hLimit.y, GT.vLimit.y );
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
	G->getGraph().logBase = 2;

	G->getGraph().logScale = GT.vLogAxBtn->IsActive();

	G->updateVertexArrays();
	//G->printSettings();
}

GraphBook::graphTuple& GraphBook::cTT()
{
	/// the way to prevent return value if current page = -1 is to check hasValidTab() first
	int i = Tabs->GetCurrentPage();
	//Engine::out() << "Tab index accessed is: " << i << std::endl;
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
    if ( Tabs->GetCurrentPage() < 0 || Tabs->GetPageCount() <= 0 ) return false;

    // we have tabs, check for "valid" tabs, not the placeholder one

//    Engine::out() << "[GraphBook] Hasvalidtab: " << Tabs->GetPageCount() << " Tabs" << std::endl;


    Label::Ptr tmp = std::dynamic_pointer_cast<Label>( Tabs->GetNthTabLabel(0) );
//    Engine::out() << "[GraphBook] Label: " << tmp->GetText().toAnsiString() << " Tabs" << std::endl;
    if ( tmp->GetText() == "No graph" )
        return false;

    return true;
}

void GraphBook::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , ( winSize.y - Allocation.height ) ) );
}

int GraphBook::entryVal( std::shared_ptr<sfg::Entry>& E )
{
    int value = boost::lexical_cast<int>( E->GetText().toAnsiString().empty() ? "0" : E->GetText().toAnsiString() );
	E->SetText( boost::lexical_cast<std::string>( value ) );
	E->SetCursorPosition( 99 );
	return value;
}

void GraphBook::clearGraphs()
{
    while ( Tabs->GetPageCount() > 1 )
        Tabs->RemovePage(0);

    Label::Ptr label = Label::Create( "No graph" );
    Label::Ptr label2 = Label::Create( "There are no graphs available at this time." );
	Tabs->SetCurrentPage( Tabs->AppendPage( label2, label ) );

    Tabs->RemovePage(0);

    //Tabs->SetCurrentPage( -1 );
    graphTupleList.clear();
}

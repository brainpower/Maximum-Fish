#include "InfoPanel.hpp"
#include "sbe/gfx/Screen.hpp"

#include "SFGUI/Window.hpp"
#include "SFGUI/Notebook.hpp"
#include "SFGUI/Label.hpp"

//using namespace sfg is not possible because Engine is ambiguous

InfoPanel::InfoPanel( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
	RegisterForEvent( "TOGGLE_SHOW_INFOPANEL" );
	RegisterForEvent( "TOGGLE_SELECT_MAN" );
	RegisterForEvent( "WINDOW_RESIZE" );
	RegisterForEvent( "TOGGLE_FULLSCREEN" );
	CurrentActivePage = 0;
	SelectorManipulator = 0;
	CreateWindow( RelativePosition, Size );
}

void InfoPanel::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
	//make and configure window
	Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::SHADOW );
	Win->SetRequisition( sf::Vector2f( 10, 10 ) );
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
	// set Allocation (instead of calling updatePosition, because size is unknown)
	Win->SetAllocation( sf::FloatRect( ( appSize.x - 300 ), 0, 300, appSize.y ) );

		//create toplevel notebook
		Topleveltabs = sfg::Notebook::Create();
		//Topleveltabs->SetState( Widget::State::INSENSITIVE );

			//create selectorBox
			MySelector.reset( new Selector );
			SelectionBox = MySelector->Get();
			sfg::Label::Ptr selectorlabel = sfg::Label::Create( "Selection" );

			//create manipulatorBox
			MyManipulator.reset( new Manipulator );
			ManipulationBox = MyManipulator->Get();
			sfg::Label::Ptr manipulatorlabel = sfg::Label::Create( "Manipulation" );

			MyOverlay.reset( new Overlays );

		//pack all into toplevelnotebook
		Topleveltabs->AppendPage( SelectionBox, selectorlabel );
		Topleveltabs->AppendPage( ManipulationBox, manipulatorlabel );
		Topleveltabs->AppendPage( MyOverlay->Get(), sfg::Label::Create( "Overlays" ) );
		Topleveltabs->GetSignal( sfg::Label::OnMouseLeftPress ).Connect( std::bind( &InfoPanel::CheckTabSwitchPermission, this ));

	//add wholeselectionbox to window
	Win->Add( Topleveltabs );
	Win->SetTitle("InfoPanel [F1]");
	updatePosition();

	Module::Get()->QueueEvent( Event( "SCREEN_ADD_WINDOW", Win ) );
}

void InfoPanel::CheckTabSwitchPermission()
{
	//check wether a TabSwitch is called or normal click
	if ( Topleveltabs->GetPrelightTab() == -1 ) { return; }

	if ( Topleveltabs->GetPrelightTab() == CurrentActivePage || Topleveltabs->GetPrelightTab() == 2 )
	{
		CurrentActivePage = Topleveltabs->GetCurrentPage();
	}
	else if ( CurrentActivePage == 2 )
	{
		Topleveltabs->SetCurrentPage( SelectorManipulator );
	}
	else // 0->1, 1->0
	{
		Topleveltabs->SetCurrentPage( CurrentActivePage );
	}
}

void InfoPanel::HandleEvent( Event& e )
{
	if ( e.Is( "WINDOW_RESIZE" ) || e.Is( "TOGGLE_FULLSCREEN" ) )
	{
		updatePosition();
	}
	else if ( e.Is( "TOGGLE_SHOW_INFOPANEL" ) )
	{
		if ( Win->IsGloballyVisible() )
		{
			Win->Show( false );
		}
		else
		{
			updatePosition();
			Win->Show( true );
			Win->GrabFocus();
		}
	}
	else if ( e.Is( "TOGGLE_SELECT_MAN" ) )
	{
		if ( Topleveltabs->GetCurrentPage() == 1 )
		{
			Topleveltabs->SetCurrentPage( 0 );
			CurrentActivePage = 0;
			SelectorManipulator = 0;
		}
		else if ( Topleveltabs->GetCurrentPage() == 0 )
		{
			Topleveltabs->SetCurrentPage( 1 );
			CurrentActivePage = 1;
			SelectorManipulator = 1;
		}
	}
}

void InfoPanel::updatePosition()
{
	//get widgetAllocation:
	sf::FloatRect widgetAllocation = Win->GetAllocation();
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
	//set new widgetAllocation:
	Win->SetAllocation(	sf::FloatRect( ( appSize.x - ( widgetAllocation.width ) ) , 0 , widgetAllocation.width , appSize.y ) );
}

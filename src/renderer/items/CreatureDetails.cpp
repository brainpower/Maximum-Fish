#include "CreatureDetails.hpp"



CreatureDetails::CreatureDetails( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    RegisterForEvent( "TOGGLE_SHOW_CREATUREDETAILS" );
    RegisterForEvent( "WINDOW_RESIZE" );

	currentlabeltext = 0;
	CreateWindow(RelativePosition, Size);
	Win->Show(false);
}

void CreatureDetails::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
	Win = sfg::Window::Create();
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR | sfg::Window::Style::SHADOW  | sfg::Window::Style::RESIZE );
    Win->SetRequisition( sf::Vector2f( 10 , 10 ) );


	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
    // set Allocation (instead of calling updatePosition, because size is unknown)
    Win->SetAllocation(sf::FloatRect( ( appSize.x - 300 ) , 0 , 300 , appSize.y ));


	// Create a window and add the box layouter to it. Also set the window's title.

	Win->SetTitle( "Creature Details" );

	Event e("SCREEN_ADD_WINDOW");
	e.SetData( Win );
	Module::Get()->QueueEvent( e );
}

void CreatureDetails::HandleEvent( Event& e )
{
	if (e.Is("WINDOW_RESIZE"))
    {
        updatePosition();
    }
	else if (e.Is("TOGGLE_SHOW_CREATUREDETAILS"))
    {
        if (Win->IsGloballyVisible())
		{
			Win->Show(false);
		}
        else
		{
			updatePosition();
			Win->Show(true);
			Win->GrabFocus();
		}
    }
}

void CreatureDetails::updatePosition()
{
    //get widgetAllocation:
	sf::FloatRect widgetAllocation = Win->GetAllocation();
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
	//set new widgetAllocation:
	Win->SetAllocation(	sf::FloatRect( ( appSize.x - (widgetAllocation.width) ) , 0 , widgetAllocation.width , appSize.y ) );
	}

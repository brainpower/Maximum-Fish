#include "CreatureDetails.hpp"



CreatureDetails::CreatureDetails( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    RegisterForEvent( "TOGGLE_SHOW_CREATUREDETAILS" );

	currentlabeltext = 0;
	CreateWindow(RelativePosition, Size);
	Win->Show(false);
}

void CreatureDetails::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
	Win = sfg::Window::Create();
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR | sfg::Window::Style::SHADOW  | sfg::Window::Style::RESIZE );

	//Win->SetPosition( sf::Vector2f(RelativePosition.x(), RelativePosition.y() ) );
	//Win->SetRequisition( sf::Vector2f(Size.x(), Size.y() ) );



    Win->SetRequisition( sf::Vector2f(Size.x(), Size.y() ) );
    updatePosition();


	// Create a window and add the box layouter to it. Also set the window's title.

	Win->SetTitle( "Creature Details" );

	Event e("SCREEN_ADD_WINDOW");
	e.SetData( Win );
	Module::Get()->QueueEvent( e );
}

void CreatureDetails::HandleEvent( Event& e )
{
	if (e.Is("MOUSE_BUTTON_1"))
    {
        //@TODO: buttonclick
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
    //get size:
	sf::FloatRect Allocation = Win->GetAllocation();
	//get new position:
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	//set new size:
	Win->SetRequisition(sf::Vector2f((Allocation.width),(winSize.y)));
	//set new position:
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , 0 ) );
	}

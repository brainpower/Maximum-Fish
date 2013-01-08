#include "CreatureDetails.hpp"



CreatureDetails::CreatureDetails( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    //RegisterForEvent( "EVENT_NAME" );

	currentlabeltext = 0;
	CreateWindow(RelativePosition, Size);
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
	//if (e.Is("EVENT_NAME"))
	{

	}
}

void CreatureDetails::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , ( winSize.y - Allocation.height ) ) );
}

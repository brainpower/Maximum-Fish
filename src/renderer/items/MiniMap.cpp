
#include "MiniMap.hpp"


MiniMap::MiniMap( const Geom::Vec2 Size)

{
    RegisterForEvent( "EVT_FRAME" );
    RegisterForEvent( "MOUSE_BUTTON_1" );
	RegisterForEvent( "TOGGLE_SHOW_MINIMAP" );

    currentlabeltext = 0;
    CreateWindow(Size);
}

void MiniMap::CreateWindow( const Geom::Vec2 Size )
{
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR | sfg::Window::Style::SHADOW );

    //@TODO: create minimap

    Win->SetRequisition( sf::Vector2f(Size.x(), Size.y() ) );
    updatePosition();


    // main box, vertical
    sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::VERTICAL, 3.0f ) );
    //box->Pack( elementname, bool, bool);
    //box->Pack( elementname, bool, bool);


    // Create a window and add the box layouter to it. Also set the window's title.

    Win->SetTitle( "Mini Map [M]" );
    Win->Add( box );

    Event e("SCREEN_ADD_WINDOW");
    e.SetData( Win );
    Module::Get()->QueueEvent( e );
}

void MiniMap::HandleEvent( Event& e)
{
    if (e.Is("MOUSE_BUTTON_1"))
    {
        //@TODO: buttonclick
    }
	else if (e.Is("TOGGLE_SHOW_MINIMAP"))
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

void MiniMap::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , ( winSize.y - Allocation.height ) ) );
}

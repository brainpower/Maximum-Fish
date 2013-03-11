#include "GraphBook.hpp"

GraphBook::GraphBook( const Geom::Vec2 Size)

{
    RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "KEY_SHOW_GRAPHS" );

    CreateWindow(Size);
    Win->Show(false);
}

void GraphBook::CreateWindow( const Geom::Vec2 Size )
{
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::RESIZE | sfg::Window::Style::SHADOW );

    //@TODO: create graphs

    Win->SetRequisition( sf::Vector2f(Size.x, Size.y ) );
    updatePosition();


    sfg::Notebook::Ptr tabs( sfg::Notebook::Create() );
    tabs->SetTabPosition( sfg::Notebook::TabPosition::LEFT );
		sfg::Box::Ptr ageBox( sfg::Box::Create() );
		sfg::Box::Ptr populationBox( sfg::Box::Create() );
	tabs->AppendPage( ageBox, sfg::Label::Create( "Age" ) );
	tabs->AppendPage( populationBox, sfg::Label::Create( "Population" ) );


    Win->Add( tabs );

    Event e("SCREEN_ADD_WINDOW");
    e.SetData( Win );
    Module::Get()->QueueEvent( e );
}

void GraphBook::HandleEvent( Event& e)
{
    if (e.Is("KEY_SHOW_GRAPHS"))
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

void GraphBook::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , ( winSize.y - Allocation.height ) ) );
}

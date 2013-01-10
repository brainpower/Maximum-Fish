
#include "Control.hpp"


Control::Control( const Geom::Vec2 Size)

{
    RegisterForEvent( "WINDOW_RESIZE" );

    currentlabeltext = 0;
    CreateWindow(Size);
    Win->Show(true);
}

void Control::CreateWindow( const Geom::Vec2 Size )
{
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::SHADOW );


    Win->SetRequisition( sf::Vector2f(Size.x(), Size.y() ) );
    updatePosition();


    // main box, vertical
    sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ) );

    sfg::Button::Ptr btnDbgWin( sfg::Button::Create( "Debug" ) );
    btnDbgWin->GetSignal( sfg::Widget::OnLeftClick ).Connect( &Control::BtnDbgWinClick, this );

    sfg::Button::Ptr btnCreDetWin( sfg::Button::Create( "Creature" ) );
    btnCreDetWin->GetSignal( sfg::Widget::OnLeftClick ).Connect( &Control::BtnCreDetWinClick, this );

    sfg::Button::Ptr btnMnMnWin( sfg::Button::Create( "MainMenu" ) );
    btnMnMnWin->GetSignal( sfg::Widget::OnLeftClick ).Connect( &Control::BtnMnMnWinClick, this );

    sfg::Button::Ptr btnMiMapWin( sfg::Button::Create( "MiniMap" ) );
    btnMiMapWin->GetSignal( sfg::Widget::OnLeftClick ).Connect( &Control::BtnMiMapWinClick, this );

    box->Pack( btnDbgWin, false, false);
    box->Pack( btnCreDetWin, false, false);
    box->Pack( btnMnMnWin, false, false);
    box->Pack( btnMiMapWin, false, false);


    // Create a window and add the box layouter to it.


    Win->Add( box );

    Event e("SCREEN_ADD_WINDOW");
    e.SetData( Win );
    Module::Get()->QueueEvent( e );
}

void Control::HandleEvent( Event& e)
{
   if (e.Is("WINDOW_RESIZE"))
    {
        updatePosition();
    }

}

void Control::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( 0 , ( winSize.y - Allocation.height ) ) );
}
void Control::BtnDbgWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_CONSOLE") , true );
}

void Control::BtnCreDetWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_CREATUREDETAILS") , true );
}

void Control::BtnMnMnWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_MAINMENU") , true );
}

void Control::BtnMiMapWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_MINIMAP") , true );
}

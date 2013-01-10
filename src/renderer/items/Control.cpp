
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

    sfg::Button::Ptr btnDbgWin(    sfg::ToggleButton::Create( "Debug" ) );
    sfg::Button::Ptr btnCreDetWin( sfg::ToggleButton::Create( "Creature" ) );
    sfg::Button::Ptr btnMnMnWin(   sfg::ToggleButton::Create( "MainMenu" ) );
    sfg::Button::Ptr btnMiMapWin(  sfg::ToggleButton::Create( "MiniMap" ) );
    sfg::Button::Ptr btnSimPause(  sfg::ToggleButton::Create( "SimPause" ) );

    btnDbgWin->GetSignal(    sfg::ToggleButton::OnToggle ).Connect( &Control::BtnDbgWinClick, this );
    btnCreDetWin->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnCreDetWinClick, this );
    btnMnMnWin->GetSignal(   sfg::ToggleButton::OnToggle ).Connect( &Control::BtnMnMnWinClick, this );
    btnMiMapWin->GetSignal(  sfg::ToggleButton::OnToggle ).Connect( &Control::BtnMiMapWinClick, this );
    btnSimPause->GetSignal(  sfg::ToggleButton::OnToggle ).Connect( &Control::BtnSimPauseClick, this );

    box->Pack( btnDbgWin,    false, false);
    box->Pack( btnCreDetWin, false, false);
    box->Pack( btnMnMnWin,   false, false);
    box->Pack( btnMiMapWin,  false, false);
    box->Pack( btnSimPause,  false, false);


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
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_CONSOLE") );
}

void Control::BtnCreDetWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_CREATUREDETAILS") );
}

void Control::BtnMnMnWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_MAINMENU") );
}

void Control::BtnMiMapWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_MINIMAP") );
}

void Control::BtnSimPauseClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SIM_PAUSE"), true );
}

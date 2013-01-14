#include "Control.hpp"

Control::Control( const Geom::Vec2 Size)

{
    RegisterForEvent( "WINDOW_RESIZE" );
    RegisterForEvent( "KEY_SHOW_CONSOLE" );
    RegisterForEvent( "KEY_SHOW_INFOPANEL" );
    RegisterForEvent( "KEY_SHOW_MAINMENU" );
    RegisterForEvent( "KEY_SHOW_MINIMAP" );
    RegisterForEvent( "KEY_SIM_PAUSE" );

    currentlabeltext = 0;
    CreateWindow(Size);
    Win->Show(true);
}

void Control::CreateWindow( const Geom::Vec2 Size )
{
    Win = sfg::Window::Create();
    Win->SetStyle(0);


    Win->SetRequisition( sf::Vector2f(Size.x, Size.y) );
    updatePosition();


    // main box, vertical
    sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::HORIZONTAL, 5.0f ) );

    BtnDbgWin =   sfg::ToggleButton::Create( "Console" );
    BtnIPanWin =  sfg::ToggleButton::Create( "InfoPanel" );
    BtnMnMnWin =  sfg::ToggleButton::Create( "MainMenu" );
    BtnMiMapWin = sfg::ToggleButton::Create( "MiniMap" );
    BtnSimPause = sfg::ToggleButton::Create( "SimPause" );

    //init the first look of the buttons BEFORE they are connected with their actions.
    BtnDbgWin->SetActive(true);
    BtnIPanWin->SetActive(true);

    BtnDbgWin->GetSignal(   sfg::ToggleButton::OnToggle ).Connect( &Control::BtnDbgWinClick, this );
    BtnIPanWin->GetSignal(  sfg::ToggleButton::OnToggle ).Connect( &Control::BtnIPanWinClick, this );
    BtnMnMnWin->GetSignal(  sfg::ToggleButton::OnToggle ).Connect( &Control::BtnMnMnWinClick, this );
    BtnMiMapWin->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnMiMapWinClick, this );
    BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnSimPauseClick, this );

    box->Pack( BtnDbgWin,   false, false);
    box->Pack( BtnIPanWin,  false, false);
    box->Pack( BtnMnMnWin,  false, false);
    box->Pack( BtnMiMapWin, false, false);
    box->Pack( BtnSimPause, false, false);


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
    else if (e.Is("KEY_SHOW_CONSOLE"))
    {
        BtnDbgWin->SetActive(!BtnDbgWin->IsActive());
    }
    else if (e.Is("KEY_SHOW_INFOPANEL"))
    {
        BtnIPanWin->SetActive(!BtnIPanWin->IsActive());
    }
    else if (e.Is("KEY_SHOW_MAINMENU"))
    {
        BtnMnMnWin->SetActive(!BtnMnMnWin->IsActive());
    }
    else if (e.Is("KEY_SHOW_MINIMAP"))
    {
        BtnMiMapWin->SetActive(!BtnMiMapWin->IsActive());
    }
    else if (e.Is("KEY_SIM_PAUSE"))
    {
        BtnSimPause->SetActive(!BtnSimPause->IsActive());
    }

}

void Control::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( -5 , ( winSize.y - Allocation.height + 5 ) ) );
}
void Control::BtnDbgWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_CONSOLE") );
}

void Control::BtnIPanWinClick()
{
    Module::Get()->QueueEvent( Event("TOGGLE_SHOW_INFOPANEL") );
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

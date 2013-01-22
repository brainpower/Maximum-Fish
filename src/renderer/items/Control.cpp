#include "Control.hpp"

Control::Control( const Geom::Vec2 Size)

{
    RegisterForEvent( "WINDOW_RESIZE" );
    RegisterForEvent( "KEY_SHOW_CONSOLE" );
    RegisterForEvent( "KEY_SHOW_INFOPANEL" );
    RegisterForEvent( "KEY_SHOW_MAINMENU" );
    RegisterForEvent( "KEY_SHOW_MINIMAP" );
    RegisterForEvent( "KEY_SIM_PAUSE" );
    RegisterForEvent( "LOCK_SIM_ON_PAUSE" );

    currentlabeltext = 0;
    CreateWindow(Size);
    Win->Show(true);
    simPauseLock = false;
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
    BtnSimPause = sfg::ToggleButton::Create( "Pause" );
    BtnSimReset =       sfg::Button::Create( "Reset" );

    //init the first look of the buttons BEFORE they are connected with their actions.
    BtnDbgWin->SetActive(true);
    BtnIPanWin->SetActive(true);
    BtnSimPause->SetActive(true);

    BtnDbgWin->GetSignal(   sfg::ToggleButton::OnToggle ).Connect( &Control::BtnDbgWinClick, this );
    BtnIPanWin->GetSignal(  sfg::ToggleButton::OnToggle ).Connect( &Control::BtnIPanWinClick, this );
    BtnMnMnWin->GetSignal(  sfg::ToggleButton::OnToggle ).Connect( &Control::BtnMnMnWinClick, this );
    BtnMiMapWin->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnMiMapWinClick, this );
    simPauseConnectionSerial = BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnSimPauseClick, this );
    BtnSimReset->GetSignal( sfg::Button::OnLeftClick ).Connect( &Control::BtnSimResetClick, this );

    box->Pack( BtnDbgWin,   false, false);
    box->Pack( BtnIPanWin,  false, false);
    box->Pack( BtnMnMnWin,  false, false);
    box->Pack( BtnMiMapWin, false, false);
    box->Pack( BtnSimPause, false, false);
    box->Pack( BtnSimReset, false, false);


    // Create a window and add the box layouter to it.


    Win->Add( box );

    Event e("SCREEN_ADD_WINDOW");
    e.SetData( Win );
    Module::Get()->QueueEvent( e );
}

void Control::HandleEvent( Event& e)
{
   if (e.Is("WINDOW_RESIZE"))
    {   // WINDOW_RESIZE is used here to ensure the Bottom position
        // of the ControlButtons.
        updatePosition();
    }
    else if (e.Is("KEY_SHOW_CONSOLE"))
    {   // KEY_SHOW_CONSOLE is sent whenever an action toggles the
        // visibility of the DebugWindow other than the ControlButton.
        // (Most likely this is the assigned keyboardkey.)
        BtnDbgWin->SetActive(!BtnDbgWin->IsActive());
    }
    else if (e.Is("KEY_SHOW_INFOPANEL"))
    {   // KEY_SHOW_INFOPANEL is sent whenever an action toggles the
        // visibility of the InfoPanel other than the ControlButton.
        // (Most likely this is the assigned keyboardkey.)
        BtnIPanWin->SetActive(!BtnIPanWin->IsActive());
    }
    else if (e.Is("KEY_SHOW_MAINMENU"))
    {   // KEY_SHOW_MAINMENU is sent whenever an action toggles the
        // visipility of the MainMenu other than the ControlButton.
        // (Most likely this is the assigned keyboardkey or the Resume
        // in the Mainmenu.)
        BtnMnMnWin->SetActive(!BtnMnMnWin->IsActive());
    }
    else if (e.Is("KEY_SHOW_MINIMAP") && !simPauseLock)
    {   // KEY_SHOW_MINIMAP is sent whenever an action toggles the
        // visibility of the MiniMap other than the ControlButton.
        // (Most likely this is the assigned keyboardkey.)
        BtnMiMapWin->SetActive(!BtnMiMapWin->IsActive());
    }
    else if (e.Is("KEY_SIM_PAUSE"))
    {   // KEY_SIM_PAUSE is sent whenever an action toggles the Simulation
        // to its paused state other than the ControlButton.
        // (Most likely this is the assigned keyboardkey or some
        // kind of OptionsWindow.)
        BtnSimPause->SetActive(!BtnSimPause->IsActive());
    }
    else if (e.Is("LOCK_SIM_ON_PAUSE"))
    {
        simPauseLock = !simPauseLock;
        BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Disconnect(simPauseConnectionSerial);
        BtnSimPause->SetActive(!BtnSimPause->IsActive());
        simPauseConnectionSerial = BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnSimPauseClick, this );
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
    if (simPauseLock)
    {
        BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Disconnect(simPauseConnectionSerial);
        BtnSimPause->SetActive(!BtnSimPause->IsActive());
        simPauseConnectionSerial = BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnSimPauseClick, this );
    }
    Module::Get()->QueueEvent( Event("TOGGLE_SIM_PAUSE"), true );
}

void Control::BtnSimResetClick()
{
    Module::Get()->QueueEvent( Event("RESET_SIMULATION"), true );
}

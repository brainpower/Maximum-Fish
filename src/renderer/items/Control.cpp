#include "Control.hpp"

Control::Control( const Geom::Vec2 Size)
// simulation starts paused, so start with one lock
 : simPauseLockLevel(1)
{
    RegisterForEvent( "WINDOW_RESIZE" );
    RegisterForEvent( "KEY_SHOW_CONSOLE" );
    RegisterForEvent( "KEY_SHOW_INFOPANEL" );
    RegisterForEvent( "KEY_SHOW_MAINMENU" );
    RegisterForEvent( "KEY_SHOW_MINIMAP" );
    RegisterForEvent( "KEY_SIM_PAUSE" );
    RegisterForEvent( "SIM_ON_PAUSE_LOCK" );
    RegisterForEvent( "SIM_FROM_PAUSE_RELEASE" );

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
        BtnSimPause = sfg::ToggleButton::Create( "Pause" );
        BtnSimReset =       sfg::Button::Create( "Reset" );

        sfg::Box::Ptr framesframe( sfg::Box::Create( sfg::Box::HORIZONTAL, 0 ) );
            Framesdisplay = sfg::Entry::Create();
            Framesdisplay->SetRequisition( sf::Vector2f( 40, 0 ) );
            Framesdisplay->SetState( sfg::Widget::State::INSENSITIVE );
            Framesdisplay->SetText( boost::lexical_cast<std::string>(Frames) );
            sfg::Button::Ptr down = sfg::Button::Create( "<" );
            sfg::Button::Ptr up   = sfg::Button::Create( ">" );
                down->GetSignal( sfg::Button::OnLeftClick ).Connect( &Control::BtnFramesDownClick, this );
                up->GetSignal(   sfg::Button::OnLeftClick ).Connect( &Control::BtnFramesUpClick, this );

            framesframe->Pack( down, false, false );
            framesframe->Pack( Framesdisplay, false, false );
            framesframe->Pack( up, false, false );

        //init the first look of the buttons BEFORE they are connected with their actions.
        BtnDbgWin->SetActive(true);
        BtnIPanWin->SetActive(true);
        BtnSimPause->SetActive(true);

        BtnDbgWin->GetSignal(   sfg::ToggleButton::OnToggle ).Connect( &Control::BtnDbgWinClick, this );
        BtnIPanWin->GetSignal(  sfg::ToggleButton::OnToggle ).Connect( &Control::BtnIPanWinClick, this );
        BtnMnMnWin->GetSignal(  sfg::ToggleButton::OnToggle ).Connect( &Control::BtnMnMnWinClick, this );
        BtnMiMapWin->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnMiMapWinClick, this );
            simPauseConnectionSerial =
        BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnSimPauseClick, this );
        BtnSimReset->GetSignal( sfg::Button::OnLeftClick    ).Connect( &Control::BtnSimResetClick, this );

        box->Pack( BtnDbgWin,   false, false);
        box->Pack( BtnIPanWin,  false, false);
        box->Pack( BtnMnMnWin,  false, false);
        box->Pack( BtnMiMapWin, false, false);
        box->Pack( BtnSimPause, false, false);
        box->Pack( BtnSimReset, false, false);
        box->Pack( framesframe );


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
    else if (e.Is("SIM_ON_PAUSE_LOCK"))
    {
        //if ( BtnSimPause->IsActive() )
        //{
            SimPause(true);
        //}
        //else
        //{
        //    BtnSimPause->SetActive(true);
        //}
    }
    else if (e.Is("SIM_FROM_PAUSE_RELEASE"))
    {
        //if ( !BtnSimPause->IsActive() )
        //{
            SimPause(false);
        //}
        //else
        //{
        //    BtnSimPause->SetActive(false);
        //}
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
    SimPause( BtnSimPause->IsActive() );
}

void Control::SimPause(bool _p)
{
    if (_p) //on locking
    {
        if (simPauseLockLevel == 0) //first locking with pause
			Module::Get()->QueueEvent( Event( "SIM_PAUSE" ), true );
        simPauseLockLevel++;

        //BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Disconnect(simPauseConnectionSerial);
        //BtnSimPause->SetActive(false);
        //simPauseConnectionSerial = BtnSimPause->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Control::BtnSimPauseClick, this );
    } else     {
        simPauseLockLevel--;

        if ( simPauseLockLevel == 0 )
            Module::Get()->QueueEvent( Event("SIM_UNPAUSE"), true );
    }

    Engine::out() << "[Control] PauseLock: " << simPauseLockLevel << std::endl;
}

void Control::BtnSimResetClick()
{
    BtnSimPause->SetActive(true);
    Module::Get()->QueueEvent( Event("RESET_SIMULATION"), true );
}

void Control::BtnFramesDownClick()
{
    if ( Frames <= 5 ){return;}
    Frames -= 5;
    Framesdisplay->SetText(boost::lexical_cast<std::string>(Frames));
    Event e = Event("SET_SIM_TPS");
    e.SetData(Frames);
    Module::Get()->QueueEvent( e, true );
}

void Control::BtnFramesUpClick()
{
    if ( Frames >= 500 ){return;}
    Frames += 5;
    Framesdisplay->SetText( boost::lexical_cast<std::string>(Frames) );
    Event e = Event("SET_SIM_TPS");
    e.SetData(Frames);
    Module::Get()->QueueEvent( e, true );
}

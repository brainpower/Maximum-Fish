#include "Control.hpp"

#include "sbe/Config.hpp"
#include "sbe/sfg/Message.hpp"

#include <SFGUI/ToggleButton.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Box.hpp>

#include "sbe/gfx/Screen.hpp"

using sbe::Screen;
using namespace sfg;

Control::Control( const Geom::Vec2 Size )
// simulation starts paused, so start with one lock
 : Frames( 0 ), simPauseLockLevel( 1 )
{
	RegisterForEvent( "WINDOW_RESIZE" );
	RegisterForEvent( "TOGGLE_FULLSCREEN" );
	RegisterForEvent( "KEY_SHOW_CONSOLE" );
	RegisterForEvent( "KEY_SHOW_INFOPANEL" );
	RegisterForEvent( "KEY_SHOW_MAINMENU" );
	RegisterForEvent( "KEY_SHOW_GRAPHBOOK" );
	RegisterForEvent( "KEY_SIM_PAUSE" );
	RegisterForEvent( "PAUSELOCK_DOWN" );
	RegisterForEvent( "PAUSELOCK_UP" );

	Frames = Engine::getCfg()->get<int>( "system.ui.control.simFrameRate" );

	CreateWindow( Size );
	Win->Show( true );
}

void Control::CreateWindow( const Geom::Vec2 Size )
{
	Win = Window::Create();
	Win->SetStyle( 0 );
	Win->SetRequisition( sf::Vector2f( Size.x, Size.y ) );

	// main box, vertical
	Box::Ptr box( Box::Create( Box::HORIZONTAL, 5.0f ) );

		BtnMnMnWin =  ToggleButton::Create(  "MainMenu [Esc]" );
		BtnIPanWin =  ToggleButton::Create( "InfoPanel [F1]" );
		BtnSimPause = ToggleButton::Create(     "Pause [F2]" );
		BtnDbgWin =   ToggleButton::Create(   "Console [F3]" );
		BtnGraBoWin = ToggleButton::Create( "GraphBook [F4]" );
		BtnSimReset =       Button::Create(     "Reset [F5]" );
		BtnSimSingleFrame = Button::Create(     ">|" );
		BtnSimFrames = Button::Create(     ">>|" );

		Box::Ptr framesframe( Box::Create( Box::HORIZONTAL, 0 ) );
			Framesdisplay = Entry::Create();
			Framesdisplay->SetRequisition( sf::Vector2f( 40, 0 ) );
			Framesdisplay->SetState( Widget::State::INSENSITIVE );
			Framesdisplay->SetText( boost::lexical_cast<std::string>( Frames ) );
			Button::Ptr down = Button::Create( "<" );
			Button::Ptr up   = Button::Create( ">" );
				down->GetSignal( Button::OnLeftClick ).Connect( &Control::BtnFramesDownClick, this );
				down->GetSignal( Button::OnLeftClick ).Connect( &Screen::OnHandledEvent, Screen::get() );
				up->GetSignal(   Button::OnLeftClick ).Connect( &Control::BtnFramesUpClick, this );
				up->GetSignal(   Button::OnLeftClick ).Connect( &Screen::OnHandledEvent, Screen::get() );

			framesframe->Pack( down, false, false );
			framesframe->Pack( Framesdisplay, false, false );
			framesframe->Pack( up, false, false );

		//init the first look of the buttons BEFORE they are connected with their actions.
		BtnDbgWin->SetActive( true );
		BtnIPanWin->SetActive( true );
		BtnSimPause->SetActive( true );

		BtnDbgWin->GetSignal(   ToggleButton::OnToggle ).Connect( &Control::BtnDbgWinClick, this );
		BtnDbgWin->GetSignal(   ToggleButton::OnToggle ).Connect( &Screen::OnHandledEvent, Screen::get() );
		BtnIPanWin->GetSignal(  ToggleButton::OnToggle ).Connect( &Control::BtnIPanWinClick, this );
		BtnIPanWin->GetSignal(  ToggleButton::OnToggle ).Connect( &Screen::OnHandledEvent, Screen::get() );
		BtnMnMnWin->GetSignal(  ToggleButton::OnToggle ).Connect( &Control::BtnMnMnWinClick, this );
		BtnMnMnWin->GetSignal(  ToggleButton::OnToggle ).Connect( &Screen::OnHandledEvent, Screen::get() );
		BtnGraBoWin->GetSignal( ToggleButton::OnToggle ).Connect( &Control::BtnGraBoWinClick, this );
		BtnGraBoWin->GetSignal( ToggleButton::OnToggle ).Connect( &Screen::OnHandledEvent, Screen::get() );
			simPauseConnectionSerial =
		BtnSimPause->GetSignal( ToggleButton::OnToggle ).Connect( &Control::BtnSimPauseClick, this );
		BtnSimPause->GetSignal( ToggleButton::OnToggle ).Connect( &Screen::OnHandledEvent, Screen::get() );
		BtnSimReset->GetSignal( Button::OnLeftClick    ).Connect( &Control::BtnSimResetClick, this );
		BtnSimReset->GetSignal( Button::OnLeftClick    ).Connect( &Screen::OnHandledEvent, Screen::get() );
		BtnSimSingleFrame->GetSignal( Button::OnLeftClick    ).Connect( &Control::BtnSimSingleFrameClick, this );
		BtnSimSingleFrame->GetSignal( Button::OnLeftClick    ).Connect( &Screen::OnHandledEvent, Screen::get() );
		BtnSimFrames->GetSignal( Button::OnLeftClick    ).Connect( &Control::BtnSimFramesClick, this );
		BtnSimFrames->GetSignal( Button::OnLeftClick    ).Connect( &Screen::OnHandledEvent, Screen::get() );

		box->Pack( BtnMnMnWin,  false, false );
		box->Pack( BtnIPanWin,  false, false );
		box->Pack( BtnSimPause, false, false );
		box->Pack( BtnDbgWin,   false, false );
		box->Pack( BtnGraBoWin, false, false );
		box->Pack( BtnSimReset, false, false );
		box->Pack( framesframe );
		box->Pack( BtnSimSingleFrame, false, false );
		box->Pack( BtnSimFrames, false, false );

	Win->Add( box );
	updatePosition();

	Module::Get()->QueueEvent( Event( "SCREEN_ADD_WINDOW", Win ) );
}

void Control::HandleEvent( Event& e )
{
	if ( e.Is( "WINDOW_RESIZE" ) || e.Is( "TOGGLE_FULLSCREEN" ) )
	{   // WINDOW_RESIZE is used here to ensure the Bottom position
		// of the ControlButtons.
		updatePosition();
	}
	else if ( e.Is( "KEY_SHOW_CONSOLE" ) )
	{   // KEY_SHOW_CONSOLE is sent whenever an action toggles the
		// visibility of the DebugWindow other than the ControlButton.
		// (Most likely this is the assigned keyboardkey.)
		BtnDbgWin->SetActive( !BtnDbgWin->IsActive() );
	}
	else if ( e.Is( "KEY_SHOW_INFOPANEL" ) )
	{   // KEY_SHOW_INFOPANEL is sent whenever an action toggles the
		// visibility of the InfoPanel other than the ControlButton.
		// (Most likely this is the assigned keyboardkey.)
		BtnIPanWin->SetActive( !BtnIPanWin->IsActive() );
	}
	else if ( e.Is( "KEY_SHOW_MAINMENU" ) )
	{   // KEY_SHOW_MAINMENU is sent whenever an action toggles the
		// visipility of the MainMenu other than the ControlButton.
		// (Most likely this is the assigned keyboardkey or the Resume
		// in the Mainmenu.)
		BtnMnMnWin->SetActive( !BtnMnMnWin->IsActive() );
	}
	else if ( e.Is( "KEY_SHOW_GRAPHBOOK" ) && !simPauseLock )
	{   // KEY_SHOW_GRAPHBOOK is sent whenever an action toggles the
		// visibility of the GraphBook other than the ControlButton.
		// (Most likely this is the assigned keyboardkey.)
		BtnGraBoWin->SetActive( !BtnGraBoWin->IsActive() );
	}
	else if ( e.Is( "KEY_SIM_PAUSE" ) )
	{   // KEY_SIM_PAUSE is sent whenever an action toggles the Simulation
		// to its paused state other than the ControlButton.
		// (Most likely this is the assigned keyboardkey or some
		// kind of OptionsWindow.)
		BtnSimPause->SetActive( !BtnSimPause->IsActive() );
	}
	else if ( e.Is( "PAUSELOCK_UP" ) )
	{
		SimPause( true );
	}
	else if ( e.Is( "PAUSELOCK_DOWN" ) )
	{
		SimPause( false );
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
	Module::Get()->QueueEvent( Event( "TOGGLE_SHOW_DBGWIN" ) );
}

void Control::BtnIPanWinClick()
{
	Module::Get()->QueueEvent( Event( "TOGGLE_SHOW_INFOPANEL" ) );
}

void Control::BtnMnMnWinClick()
{
	Module::Get()->QueueEvent( Event( "TOGGLE_SHOW_MAINMENU" ) );
}

void Control::BtnGraBoWinClick()
{
	Module::Get()->QueueEvent( Event( "TOGGLE_SHOW_GRAPHBOOK" ) );
}

void Control::BtnSimPauseClick()
{
	SimPause( BtnSimPause->IsActive() );
	BtnSimPause->SetLabel( BtnSimPause->IsActive() ? "Play > [F2]" : "Pause || [F2]" );
}

void Control::BtnSimFramesClick()
{
	Module::Get()->QueueEvent( Event( "SIM_UNPAUSE", Frames ), true );
}

void Control::BtnSimSingleFrameClick()
{
	Module::Get()->QueueEvent( Event( "SIM_UNPAUSE", (unsigned int)1 ), true );
}

void Control::SimPause( bool up )
{
	if ( up ) //on locking
	{
		if ( simPauseLockLevel == 0 ) //first locking with pause
			Module::Get()->QueueEvent( Event( "SIM_PAUSE" ), true );
		simPauseLockLevel++;
	}
	else
	{
		simPauseLockLevel--;
		if ( simPauseLockLevel == 0 )
			Module::Get()->QueueEvent( Event( "SIM_UNPAUSE" ), true );
	}

	Engine::out() << "[Control] PauseLock: " << simPauseLockLevel << std::endl;
}

void Control::BtnSimResetClick()
{
	BtnSimPause->SetActive( true );
	Module::Get()->QueueEvent( Event( "RESET_SIMULATION" ), true );
}

void Control::BtnFramesDownClick()
{
	if ( Frames <= 5 ){ return; }
	if ( Frames <= 50 ){ Frames -= 5; }
	else if ( Frames <= 150 ){ Frames -= 10; }
	else { Frames -= 25; }
	Framesdisplay->SetText( boost::lexical_cast<std::string>( Frames ) );

	Module::Get()->QueueEvent( Event( "SET_SIM_TPS", Frames ), true );
}

void Control::BtnFramesUpClick()
{
	if ( Frames >= Engine::getCfg()->get<int>( "system.ui.control.simFrameLimit" ) ){ return; }
	if ( Frames >= 150 ){ Frames += 25; }
	else if ( Frames >= 50 ){ Frames += 10; }
	else { Frames += 5; }
	Framesdisplay->SetText( boost::lexical_cast<std::string>( Frames ) );

	Module::Get()->QueueEvent( Event( "SET_SIM_TPS", Frames ), true );
}

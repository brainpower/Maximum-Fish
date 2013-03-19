#include "MainMenu.hpp"
#include "sbe/sfg/Message.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Button.hpp"
#include "SFGUI/Box.hpp"

//using namespace sfg is not possible because Engine is ambiguous

MainMenu::MainMenu( const Geom::Vec2 Size )
{
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "MESSAGE_ANSWER_MAINMENU" );
	RegisterForEvent( "TOGGLE_SHOW_MAINMENU" );

	currentlabeltext = 0;
	CreateWindow( Size );
	Win->Show( false );
}

void MainMenu::CreateWindow( const Geom::Vec2 Size )
{
	Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR | sfg::Window::Style::SHADOW );

	Win->SetRequisition( sf::Vector2f( Size.x, Size.y ) );
	updatePosition();

	// create buttons and link them to methods
	sfg::Button::Ptr btnResume( sfg::Button::Create( "Resume" ) );
	btnResume->GetSignal( sfg::Button::OnLeftClick ).Connect( &MainMenu::BtnResumeClick, this );

	// exit button
	sfg::Button::Ptr btnExit( sfg::Button::Create( "Exit Program" ) );
	btnExit->GetSignal( sfg::Button::OnLeftClick ).Connect( &MainMenu::BtnExitClick, this );
	// save and load
	sfg::Button::Ptr btnSave( sfg::Button::Create( "Save Simulation" ) );
	btnSave->GetSignal( sfg::Button::OnLeftClick ).Connect( &MainMenu::BtnSaveClick, this );
	sfg::Button::Ptr btnLoad( sfg::Button::Create( "Load Simulation" ) );
	btnLoad->GetSignal( sfg::Button::OnLeftClick ).Connect( &MainMenu::BtnLoadClick, this );
	// options
	sfg::Button::Ptr btnOptions( sfg::Button::Create( "Options" ) );

	// main box, vertical
	sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::VERTICAL, 3.0f ) );
	box->Pack( btnResume, false, false );
	box->Pack( btnSave, false, false );
	box->Pack( btnLoad, false, false );
	box->Pack( btnOptions, false, false );
	box->Pack( btnExit, false, false );

	// Create a window and add the box layouter to it. Also set the window's title.

	Win->SetTitle( "Main Menu [ESC]" );
	Win->Add( box );

	Module::Get()->QueueEvent( Event( "SCREEN_ADD_WINDOW", Win ) );
}

void MainMenu::HandleEvent( Event& e )
{
	if ( e.Is( "TOGGLE_SHOW_MAINMENU" ) )
	{
		if ( Win->IsGloballyVisible() )
		{
			Win->Show( false );
			Module::Get()->QueueEvent( Event( "SIM_FROM_PAUSE_RELEASE" ), true );
		}
		else
		{
			updatePosition();
			Win->Show( true );
			Win->GrabFocus();
			Module::Get()->QueueEvent( Event( "SIM_ON_PAUSE_LOCK" ), true );
		}
	}
	else if ( e.Is( "MESSAGE_ANSWER_MAINMENU" ) )
	{
		/*Engine::out() << "======debug mainmenu answer" << std::endl;
	}
	else if ( e.Is( "MESSAGE_ANSWER_MAINMENU", typeid( bool ) ) && boost::any_cast<bool>( e.Data() ) )
	{*/
		Module::Get()->QueueEvent( Event( "EVT_QUIT" ) , true );
	}
}

void MainMenu::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width )/2 , ( winSize.y - Allocation.height )/2   ) );
}

void MainMenu::BtnResumeClick()
{
	Module::Get()->QueueEvent( Event( "KEY_SHOW_MAINMENU" ) );
}

void MainMenu::BtnSaveClick()
{
	Module::Get()->QueueEvent( Event( "EVT_SAVE_WHOLE" ), true );
}

void MainMenu::BtnLoadClick()
{
	Module::Get()->QueueEvent( Event( "EVT_LOAD_WHOLE" ), true );
}

void MainMenu::BtnExitClick()
{
	std::shared_ptr<sbe::Message> m ( new sbe::Message( sbe::Message::Type::CHOICE, "Quit Program", "This will exit your program.\nPlease remember to save your progress first.", "MESSAGE_ANSWER_MAINMENU" ) );
	Module::Get()->QueueEvent( Event( "NEW_MESSAGE", m ) );
}

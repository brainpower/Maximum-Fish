
#include "MainMenu.hpp"


MainMenu::MainMenu( const Geom::Vec2 Size)

{
    RegisterForEvent( "EVT_FRAME" );
    RegisterForEvent( "MOUSE_BUTTON_1" );
	RegisterForEvent( "TOGGLE_SHOW_MAINMENU" );

    currentlabeltext = 0;
    CreateWindow(Size);
    Win->Show(false);
}

void MainMenu::CreateWindow( const Geom::Vec2 Size )
{
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR | sfg::Window::Style::SHADOW );

    Win->SetRequisition( sf::Vector2f( Size.x, Size.y ) );
    updatePosition();

    // create buttons and link them to methods
    sfg::Button::Ptr btnResume( sfg::Button::Create( "Resume" ) );
    btnResume->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenu::BtnResumeClick, this );

    // exit button and confirmation
    BtnExit = sfg::Button::Create( "Exit Program" );
    BtnExit->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenu::BtnExitClick, this );
    ExitConfirmation = sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f );
        sfg::Button::Ptr btnExitCancel( sfg::Button::Create( "Abort" ) );
        btnExitCancel->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenu::BtnExitCancelClick, this );
        sfg::Button::Ptr btnExitConfirm( sfg::Button::Create( "Confirm" ) );
        btnExitConfirm->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenu::BtnExitConfirmClick, this );
        ExitConfirmation->Pack( btnExitCancel, true, true );
        ExitConfirmation->Pack( btnExitConfirm, true, true );
        ExitConfirmation->Show( false );


    // save and load
    sfg::Button::Ptr btnSave( sfg::Button::Create( "Save Simulation" ) );
    btnSave->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenu::BtnSaveClick, this );
    sfg::Button::Ptr btnLoad( sfg::Button::Create( "Load Simulation" ) );
    btnLoad->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenu::BtnLoadClick, this );

    // options
    sfg::Button::Ptr btnOptions( sfg::Button::Create( "Options" ) );

    // main box, vertical
    sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::VERTICAL, 3.0f ) );
    box->Pack( btnResume, false, false );
    box->Pack( btnSave, false, false );
    box->Pack( btnLoad, false, false );
    box->Pack( btnOptions, false, false );
    box->Pack( BtnExit, false, false );
    box->Pack( ExitConfirmation, false, false );


    // Create a window and add the box layouter to it. Also set the window's title.

    Win->SetTitle( "Main Menu [ESC]" );
    Win->Add( box );

    Module::Get()->QueueEvent( Event( "SCREEN_ADD_WINDOW", Win ) );
}

void MainMenu::HandleEvent( Event& e)
{
    if ( e.Is( "MOUSE_BUTTON_1" ) )
    {
        //@TODO: buttonclick
    }
	else if ( e.Is( "TOGGLE_SHOW_MAINMENU" ) )
    {
        if (Win->IsGloballyVisible())
		{
			Win->Show( false );
			BtnExitCancelClick();
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
    ExitConfirmation->Show( true );
    BtnExit->Show( false );
}

void MainMenu::BtnExitCancelClick()
{
    ExitConfirmation->Show( false );
    BtnExit->Show( true );
}

void MainMenu::BtnExitConfirmClick()
{
    ExitConfirmation->Show( false );
    BtnExit->Show( true );
    Module::Get()->QueueEvent( Event( "EVT_QUIT" ) , true );
}

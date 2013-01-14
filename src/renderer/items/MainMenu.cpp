
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

    //create buttons and link them to methods
    sfg::Button::Ptr btnResume( sfg::Button::Create( "Resume" ) );
    btnResume->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenu::BtnResumeClick, this );
    sfg::Button::Ptr btnExit( sfg::Button::Create( "Exit Program" ) );
    btnExit->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenu::BtnExitClick, this );

    Win->SetRequisition( sf::Vector2f(Size.x, Size.y ) );
    updatePosition();


    // main box, vertical
    sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::VERTICAL, 3.0f ) );
    box->Pack( btnResume, false, false);
    box->Pack( btnExit, false, false);


    // Create a window and add the box layouter to it. Also set the window's title.

    Win->SetTitle( "Main Menu [ESC]" );
    Win->Add( box );

    Event e("SCREEN_ADD_WINDOW");
    e.SetData( Win );
    Module::Get()->QueueEvent( e );
}

void MainMenu::HandleEvent( Event& e)
{
    if (e.Is("MOUSE_BUTTON_1"))
    {
        //@TODO: buttonclick
    }
	else if (e.Is("TOGGLE_SHOW_MAINMENU"))
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

void MainMenu::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width )/2 , ( winSize.y - Allocation.height )/2   ) );
}

void MainMenu::BtnResumeClick()
{
    Win->Show(false);
}

void MainMenu::BtnExitClick()
{
    Module::Get()->QueueEvent( Event("EVT_QUIT") , true );
}

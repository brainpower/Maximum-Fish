
#include "MainMenue.hpp"



MainMenue::MainMenue( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    RegisterForEvent( "EVT_FRAME" );
    RegisterForEvent( "MOUSE_BUTTON_1" );
	RegisterForEvent( "TOGGLE_SHOW_MAINMENUE" );

    currentlabeltext = 0;
    CreateWindow(RelativePosition, Size);
}

void MainMenue::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
    Win = sfg::Window::Create();

    //create buttons and link them to methods
    sfg::Button::Ptr btnResume( sfg::Button::Create( "Resume" ) );
    btnResume->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenue::BtnResumeClick, this );
    sfg::Button::Ptr btnExit( sfg::Button::Create( "Exit Program" ) );
    btnExit->GetSignal( sfg::Widget::OnLeftClick ).Connect( &MainMenue::BtnExitClick, this );

    Win->SetPosition( sf::Vector2f(RelativePosition.x(), RelativePosition.y() ) );
    //Win->SetRequisition( sf::Vector2f(Size.x(), Size.y() ) );

    // main box, vertical
    sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::VERTICAL, 3.0f ) );
    box->Pack( btnResume, false, false);
    box->Pack( btnExit, false, false);


    // Create a window and add the box layouter to it. Also set the window's title.

    Win->SetTitle( "Main Menue [ESC]" );
    Win->Add( box );

    Event e("SCREEN_ADD_WINDOW");
    e.SetData( Win );
    Module::Get()->QueueEvent( e );
}

void MainMenue::HandleEvent( Event& e)
{
    if (e.Is("MOUSE_BUTTON_1"))
    {
        //@TODO: buttonclick
    }
	else if (e.Is("TOGGLE_SHOW_MAINMENUE"))
    {
        if (Win->IsGloballyVisible())
            Win->Show(false);
        else if (!Win->IsGloballyVisible())
            Win->Show(true);
    }
}

void MainMenue::BtnResumeClick()
{
    Win->Show(false);
}

void MainMenue::BtnExitClick()
{
    Module::Get()->QueueEvent( Event("EVT_QUIT") , true );
}

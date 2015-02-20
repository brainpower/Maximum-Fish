#include "MainMenu.hpp"

#include "sbe/sfg/FileBrowse.hpp"
#include "sbe/sfg/Message.hpp"

#include "SFGUI/Window.hpp"
#include "SFGUI/Button.hpp"
#include "SFGUI/Box.hpp"

//using namespace sfg is not possible because Engine is ambiguous

MainMenu::MainMenu( const glm::ivec2 Size )
{
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "MESSAGE_ANSWER_MAINMENU" );
	RegisterForEvent( "TOGGLE_SHOW_MAINMENU" );
	RegisterForEvent( "FB_CANCEL" );
	RegisterForEvent( "FB_OK" );

	RegisterForEvent("EVT_SAVE_GOOD");
	RegisterForEvent("EVT_SAVE_BAD");
	RegisterForEvent("EVT_LOAD_BAD");
	RegisterForEvent("EVT_LOAD_GOOD");

	currentlabeltext = 0;
	InitWindow( Size );
	Win->Show( false );
}

void MainMenu::InitWindow( const glm::ivec2 Size )
{
	Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR | sfg::Window::Style::SHADOW );

	Win->SetRequisition( sf::Vector2f( Size.x, Size.y ) );
	updatePosition();

	// create buttons and link them to methods
	sfg::Button::Ptr btnResume( sfg::Button::Create( "Resume" ) );
	btnResume->GetSignal( sfg::Button::OnLeftClick ).Connect( std::bind( &MainMenu::BtnResumeClick, this ));

	sfg::Button::Ptr btnNew( sfg::Button::Create( "New Simulation" ) );
	btnNew->GetSignal( sfg::Button::OnLeftClick ).Connect( std::bind( &MainMenu::BtnNewClick, this ));

	// exit button
	sfg::Button::Ptr btnExit( sfg::Button::Create( "Exit Program" ) );
	btnExit->GetSignal( sfg::Button::OnLeftClick ).Connect( std::bind( &MainMenu::BtnExitClick, this ));
	// save and load
	sfg::Button::Ptr btnSave( sfg::Button::Create( "Save Simulation" ) );
	btnSave->GetSignal( sfg::Button::OnLeftClick ).Connect( std::bind( &MainMenu::BtnSaveClick, this ));
	sfg::Button::Ptr btnLoad( sfg::Button::Create( "Load Simulation" ) );
	btnLoad->GetSignal( sfg::Button::OnLeftClick ).Connect( std::bind( &MainMenu::BtnLoadClick, this ));
	// options
	sfg::Button::Ptr btnOptions( sfg::Button::Create( "Options" ) );

	// main box, vertical
	sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 3.0f ) );
	box->Pack( btnResume, false, false );
	box->Pack( btnNew, false, false );
	box->Pack( btnSave, false, false );
	box->Pack( btnLoad, false, false );
	box->Pack( btnOptions, false, false );
	box->Pack( btnExit, false, false );

	// Create a window and add the box layouter to it. Also set the window's title.

	Win->SetTitle( "Main Menu [ESC]" );
	Win->Add( box );


	Module::Get()->QueueEvent( Event( "SCREEN_ADD_WINDOW", Win ) );
	fb = std::make_shared<sbe::FileBrowse> ( glm::ivec2{600,400} );

}

void MainMenu::HandleEvent( Event& e )
{
	if ( e.Is( "TOGGLE_SHOW_MAINMENU" ) )
	{
		if ( Win->IsGloballyVisible() )
		{
			Win->Show( false );
			Module::Get()->QueueEvent( Event( "PAUSELOCK_DOWN" ), true );
		}
		else
		{
			updatePosition();
			Win->Show( true );
			Win->GrabFocus();
			Module::Get()->QueueEvent( Event( "PAUSELOCK_UP" ), true );
		}
	}
	else if ( e.Is( "MESSAGE_ANSWER_MAINMENU", typeid(bool) ) )
	{
		/*Engine::out() << "======debug mainmenu answer" << std::endl;
	}
	else if ( e.Is( "MESSAGE_ANSWER_MAINMENU", typeid( bool ) ) && boost::any_cast<bool>( e.Data() ) )
	{*/
		if ( boost::any_cast<bool>(e.Data()) ) Module::Get()->QueueEvent( Event( "EVT_QUIT" ) , true );
	}
    else if ( e.Is("EVT_SAVE_BAD", typeid(std::string)  ) )
	{
		Module::Get()->QueueEvent( Event("CLOSE_MESSAGE", std::string("Saving...") ) );
		std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::OK , "SAVE ERROR!", boost::any_cast< std::string >(e.Data())) );
		Module::Get()->QueueEvent( Event("NEW_MESSAGE", M) );

	}
    else if (  e.Is("EVT_LOAD_BAD", typeid(std::string)) )
	{
		Module::Get()->QueueEvent( Event("CLOSE_MESSAGE", std::string("Loading...") ) );
		std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::OK , "LOAD ERROR!", boost::any_cast< std::string >(e.Data())) );
		Module::Get()->QueueEvent( Event("NEW_MESSAGE", M) );

	}
	else if ( e.Is("EVT_LOAD_GOOD") )
	{
        Module::Get()->QueueEvent( Event("CLOSE_MESSAGE", std::string("Loading...") ) );
		std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::OK , "LOAD OK!", "Loading successfull!") );
		Module::Get()->QueueEvent( Event("NEW_MESSAGE", M) );

	}
	else if ( e.Is("EVT_SAVE_GOOD") )
    {
        Module::Get()->QueueEvent( Event("CLOSE_MESSAGE", std::string("Saving...") ) );
        std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::OK , "SAVE OK!", "Saving successfull!") );
		Module::Get()->QueueEvent( Event("NEW_MESSAGE", M) );
    }

	//~ else if( e.Is( "FB_CANCEL" )){
		// default action when presing Cancel in fb
	//~ }
	//~ else if( e.Is( "FB_OK" )){
		// default action when presing OK in fb
	//~ }
}

void MainMenu::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width )/2 , ( winSize.y - Allocation.height )/2   ) );
}

void MainMenu::BtnNewClick()
{
	Module::Get()->QueueEvent( Event( "KEY_SHOW_NEWSIM" ) );
}

void MainMenu::BtnResumeClick()
{
	Module::Get()->QueueEvent( Event( "KEY_SHOW_MAINMENU" ) );
}

void MainMenu::BtnSaveClick()
{
	fb->setTitle("Select folder to save to...");
	fb->setOkEvt("EVT_SAVE_WHOLE");
	fb->setOkAction( [](const std::string& fb){
                 	std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::MODAL , "Saving...", "Saving simulation, please wait...") );
                    Module::Get()->QueueEvent( Event("NEW_MESSAGE", M) );
                 } );
	fb->show();

	//Module::Get()->QueueEvent( Event( "EVT_SAVE_WHOLE" ), true );
}

void MainMenu::BtnLoadClick()
{
	fb->setTitle("Select folder to load from...");
	fb->setOkEvt("EVT_LOAD_WHOLE");
    fb->setOkAction( [](const std::string& fb){
                std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::MODAL , "Loading...", "Loading simulation, please wait...") );
                Module::Get()->QueueEvent( Event("NEW_MESSAGE", M) );
             } );
	fb->show();
	//Module::Get()->QueueEvent( Event( "EVT_LOAD_WHOLE" ), true );


}

void MainMenu::BtnExitClick()
{
	std::shared_ptr<sbe::Message> m ( new sbe::Message( sbe::Message::Type::CHOICE, "Quit Program", "This will exit your program.\nPlease remember to save your progress first.", "MESSAGE_ANSWER_MAINMENU" ) );
	Module::Get()->QueueEvent( Event( "NEW_MESSAGE", m ) );
}

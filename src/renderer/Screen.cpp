#include "Screen.hpp"

#include "sbe/Engine.hpp"

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

#include "sbe/event/SFMLEvent.hpp"

#include "sbe/Module.hpp"

#include "renderer/items/DebugWindow.hpp"
#include "renderer/items/MainMenu.hpp"

#include <SFML/System/Clock.hpp>
#include <SFGUI/SFGUI.hpp>


// ############# SCREEN ####

Screen::Screen()
{

	EvtConv.reset( new SFMLEventConverter() );
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "EVT_QUIT" );
	RegisterForEvent( "SCREEN_ADD_WINDOW" );

	//RegisterForSFMLEvent(sf::Event::EventType::KeyReleased);
	//RegisterForSFMLEvent(sf::Event::EventType::MouseButtonReleased);

	// convert key inputs to an event
	EvtConv->AddEventConversion( sf::Event::Closed , "EVT_QUIT", true );
	EvtConv->AddKeyConversion( sf::Keyboard::Key::F3 ,     "TOGGLE_SHOW_CONSOLE"   );
	EvtConv->AddKeyConversion( sf::Keyboard::Key::Escape , "TOGGLE_SHOW_MAINMENU" );

	Init();
}

void Screen::Init()
{
	Engine::out() << "[Screen] Creating Window..." << std::endl;
	// create the renderwindow
	Engine::GetApp().create( sf::VideoMode ( 800, 600 ), "Maximum-Fish!" );

	// must be created before using SFGUI
	SFG.reset ( new sfg::SFGUI() );

	// top-level container for all SFGUI widgets
	Desktop.reset ( new sfg::Desktop() );

	DbgWin.reset ( new DebugWindow() );
	MnMnWin.reset ( new MainMenu() );

	// We're not using SFML to render anything in this program, so reset OpenGL
    // states. Otherwise we wouldn't see anything.
    Engine::GetApp().resetGLStates();


	guiclock.reset( new sf::Clock() );
}


void Screen::Render()
{
	// Process Hardware/SFML Events
	sf::Event sfEvent;

	while ( Engine::GetApp().pollEvent ( sfEvent ) )
	{
		// Try to consume the event, if that fails try to convert it
		Desktop->HandleEvent( sfEvent );

		// give it to the converter
		EvtConv->HandleEvent( sfEvent );
	}

	// don't draw if the window is closed
	if (!Engine::GetApp().isOpen()) return;

	// update desktop
	Desktop->Update( guiclock->restart().asSeconds() );

	// Clear screen
	Engine::GetApp().clear();

	// draw SFGUI
	SFG->Display( Engine::GetApp() );

	// Blit
	Engine::GetApp().display();
}

void Screen::HandleEvent(Event& e)
{
	if (e.Is("EVT_FRAME"))
	{
		Render();
	}
	else if (e.Is("SCREEN_ADD_WINDOW"))
	{
		if (e.Data().type() == typeid( sfg::Window::Ptr ) )
		{
			sfg::Window::Ptr P = boost::any_cast<sfg::Window::Ptr>(e.Data());
			Engine::out() << "[Screen] Adding Window " << P->GetTitle().toAnsiString() << std::endl;
			Desktop->Add(P);
		}
		else
		{
			Engine::out() << "[Screen] SCREEN_ADD_WIDGET Event with wrong parameters" << std::endl;
		}
	}
	else if (e.Is("EVT_QUIT"))
	{
		Module::Get()->RequestQuit();
	}
}

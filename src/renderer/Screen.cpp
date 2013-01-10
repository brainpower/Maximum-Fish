#include "Screen.hpp"

#include "sbe/Engine.hpp"
#include "sbe/ResourceManager.hpp"

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

#include "sbe/event/SFMLEvent.hpp"

#include "sbe/Module.hpp"

#include "sbe/ImageSet.hpp"

#include "renderer/items/Control.hpp"
#include "renderer/items/CreatureDetails.hpp"
#include "renderer/items/DebugWindow.hpp"
#include "renderer/items/MainMenu.hpp"
#include "renderer/items/MiniMap.hpp"
#include "renderer/items/SimView.hpp"

#include <SFML/System/Clock.hpp>
#include <SFGUI/SFGUI.hpp>


// ############# SCREEN ####

Screen* Screen::Instance = nullptr;


Screen::Screen()
 : Fullscreen(false)
{

	Instance = this;
	KeyEventCatcher = false;

	EvtConv.reset( new SFMLEventConverter() );
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "EVT_QUIT" );
	RegisterForEvent( "SCREEN_ADD_WINDOW" );
	RegisterForEvent( "TOGGLE_FULLSCREEN" );

	//RegisterForSFMLEvent(sf::Event::EventType::KeyReleased);
	//RegisterForSFMLEvent(sf::Event::EventType::MouseButtonReleased);

	// convert key inputs to an event
	EvtConv->AddEventConversion( sf::Event::Closed ,       "EVT_QUIT", true );
	EvtConv->AddKeyConversion( sf::Keyboard::Key::F3 ,     "TOGGLE_SHOW_CONSOLE" );

	EvtConv->AddKeyConversion( sf::Keyboard::Key::F2 ,     "EVT_SAVE_TERRAIN", true );

	EvtConv->AddKeyConversion( sf::Keyboard::Key::Escape , "TOGGLE_SHOW_MAINMENU" );
	EvtConv->AddKeyConversion( sf::Keyboard::Key::M ,      "TOGGLE_SHOW_MINIMAP" );
	EvtConv->AddKeyConversion( sf::Keyboard::Key::P ,      "TOGGLE_SIM_PAUSE", true );
	EvtConv->AddKeyConversion( sf::Keyboard::Key::C ,      "TOGGLE_SHOW_CREATUREDETAILS" );
	EvtConv->AddKeyConversion( sf::Keyboard::Key::F11 ,    "TOGGLE_FULLSCREEN" );
	EvtConv->AddEventConversion( sf::Event::Resized ,      "WINDOW_RESIZE" );

	Init();
}

void Screen::Init()
{
	Engine::out(Engine::INFO) << "[Screen] Creating Window..." << std::endl;
	// create the renderwindow
	Engine::GetApp().create( sf::VideoMode ( 800, 600 ), "Maximum-Fish!" );

	// must be created before using SFGUI
	SFG.reset ( new sfg::SFGUI() );

	// top-level container for all SFGUI widgets
	Desktop.reset ( new sfg::Desktop() );

    Contr.reset   ( new Control() );
    CreDet.reset  ( new CreatureDetails() );
	DbgWin.reset  ( new DebugWindow() );
	MnMnWin.reset ( new MainMenu() );
	MiMap.reset   ( new MiniMap() );


	SimulatorView.reset ( new SimView() );


	// We're not using SFML to render anything in this program, so reset OpenGL
    // states. Otherwise we wouldn't see anything.
    Engine::GetApp().resetGLStates();


	guiclock.reset( new sf::Clock() );


	auto txts = Engine::GetIO()->loadPath<sf::Image>( "Creatures_serious.tga" );
	if (txts.size() == 1) Engine::GetResMgr()->add(txts[0], "Creatures_serious.tga");

	std::shared_ptr<ImageSet> I( new ImageSet( "Creatures", "Creatures_serious.tga", Geom::Point( 0, 0 ), Geom::Point(0,0), Geom::Vec2( 16,16), Geom::Vec2( 3, 1 ), 0 ) );
	Engine::GetResMgr()->add(I ,"Creatures");

	auto txt2 = Engine::GetIO()->loadPath<sf::Image>( "Tiles_serious.tga" );
	if (txt2.size() == 1) Engine::GetResMgr()->add(txt2[0], "Tiles_serious.tga");

	std::shared_ptr<ImageSet> I2( new ImageSet( "Tiles", "Tiles_serious.tga", Geom::Point( 0, 0 ), Geom::Point(0,0), Geom::Vec2( 32,32), Geom::Vec2( 4, 1 ), 0 ) );
	Engine::GetResMgr()->add(I2 ,"Tiles");
	//Engine::GetResMgr()->saveAllObjects<ImageSet>( true );
}


void Screen::Render()
{
	// Process Hardware/SFML Events
	sf::Event sfEvent;

	while ( Engine::GetApp().pollEvent ( sfEvent ) )
	{
		// Try to consume the event, if that fails try to convert it
		Desktop->HandleEvent( sfEvent );

		SimulatorView->HandleSfmlEvent( sfEvent );

		// give it to the converter
		EvtConv->HandleEvent( sfEvent );
	}

	// don't draw if the window is closed
	if (!Engine::GetApp().isOpen()) return;

	// update desktop
	Desktop->Update( guiclock->restart().asSeconds() );

	// Clear screen
	Engine::GetApp().clear();

	SimulatorView->Render();

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
	else if (e.Is("TOGGLE_FULLSCREEN"))
	{
		if ( !Fullscreen )
		{
			auto modes = sf::VideoMode::getFullscreenModes();

			if (modes.size() > 0)
			{
				Engine::GetApp().create(modes[0], "Maximum-Fish (fullscreen)", sf::Style::Fullscreen);
				Fullscreen = true;
			}
			else
			{
				Engine::out() << "[Screen] No supported fullscreen mode found!" << std::endl;
			}
		}
		else
		{
			Engine::GetApp().create( sf::VideoMode ( 800, 600 ), "Maximum-Fish!" );
			Fullscreen = false;
		}

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

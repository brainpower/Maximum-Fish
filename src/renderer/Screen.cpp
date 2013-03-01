#include "Screen.hpp"

#include "sbe/Engine.hpp"
#include "sbe/ResourceManager.hpp"

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

#include "sbe/event/SFMLEvent.hpp"

#include "sbe/Module.hpp"
#include "sbe/Config.hpp"

#include "sbe/gfx/ImageSet.hpp"
#include "sbe/gfx/GraphPlotter.hpp"

#include "renderer/items/Control.hpp"
#include "renderer/items/InfoPanel.hpp"
//#include "renderer/items/Manipulator.hpp"
#include "renderer/items/DebugWindow.hpp"
#include "renderer/items/MainMenu.hpp"
#include "renderer/items/MiniMap.hpp"
#include "renderer/items/SimView.hpp"

#include <SFML/System/Clock.hpp>
#include <SFGUI/SFGUI.hpp>


// ############# SCREEN ####

Screen* Screen::Instance = nullptr;


Screen::Screen()
 : Fullscreen(false),
	ActiveWindows(0)
{

	Instance = this;
	KeyEventCatcher = false;

	EvtConv.reset( new SFMLEventConverter() );
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "EVT_QUIT" );
	RegisterForEvent( "SCREEN_ADD_WINDOW" );
	RegisterForEvent( "TOGGLE_FULLSCREEN" );

	RegisterForEvent( "DISPLAY_GRAPH" );

	//RegisterForSFMLEvent(sf::Event::EventType::KeyReleased);
	//RegisterForSFMLEvent(sf::Event::EventType::MouseButtonReleased);

	// convert key inputs to an event
	int i = EvtConv->LoadKeyBindingsFromConfig();
	Engine::out(Engine::INFO) << "[Screen] Loaded " << i << " key bindings." << std::endl;

	EvtConv->AddEventConversion( sf::Event::Closed ,       "EVT_QUIT", true );
	EvtConv->AddEventConversion( sf::Event::Resized ,      "WINDOW_RESIZE" );

	Init();
}

void Screen::setCameraViewPort ( const sf::FloatRect& FR) { SimulatorView->setCameraViewport( FR ); }

void Screen::Init()
{
	Engine::out(Engine::INFO) << "[Screen] Creating Window..." << std::endl;
	// create the renderwindow
	Engine::GetApp().create( sf::VideoMode ( Engine::getCfg()->get<int>("renderer.windowsize.x"),
											Engine::getCfg()->get<int>("renderer.windowsize.y") ),
											"Maximum-Fish!" );

	// must be created before using SFGUI
	SFG.reset ( new sfg::SFGUI() );

	// top-level container for all SFGUI widgets
	Desktop.reset ( new sfg::Desktop() );

    Contr.reset   ( new Control() );
    IPan.reset    ( new InfoPanel() );
    //Man.reset     ( new Manipulator() );
	DbgWin.reset  ( new DebugWindow() );
	MnMnWin.reset ( new MainMenu() );
	MiMap.reset   ( new MiniMap() );


	SimulatorView.reset ( new SimView() );


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

		if ( ActiveWindows == 0 ) SimulatorView->HandleSfmlEvent( sfEvent );

		// give it to the converter
		EvtConv->HandleEvent( sfEvent );
	}

	// don't draw if the window is closed
	if (!Engine::GetApp().isOpen()) return;

	// update desktop
	Desktop->Update( guiclock->restart().asSeconds() );

	// Clear screen
	Engine::GetApp().clear(sf::Color(180,180,180));

	SimulatorView->Render();

	// draw SFGUI
	SFG->Display( Engine::GetApp() );

	// Blit
	Engine::GetApp().display();

}

void Screen::OnWindowEntered() {  ActiveWindows++; Engine::out() << " ActiveWindows: " << ActiveWindows << std::endl; }
void Screen::OnWindowLeft() { ActiveWindows--; Engine::out() << " ActiveWindows: " << ActiveWindows << std::endl; }

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
				Engine::out(Engine::WARNING) << "[Screen] No supported fullscreen mode found!" << std::endl;
			}
		}
		else
		{
			Engine::GetApp().create( sf::VideoMode ( Engine::getCfg()->get<int>("renderer.windowsize.x"),
														Engine::getCfg()->get<int>("renderer.windowsize.y") ),
														"Maximum-Fish!" );
			Fullscreen = false;
		}
		Event ev("WINDOW_RESIZE");
		Module::Get()->QueueEvent(ev, true);
	}
	else if (e.Is("SCREEN_ADD_WINDOW"))
	{
		if (e.Data().type() == typeid( sfg::Window::Ptr ) )
		{
			sfg::Window::Ptr P = boost::any_cast<sfg::Window::Ptr>(e.Data());
			P->GetSignal( sfg::Window::OnMouseEnter ).Connect( &Screen::OnWindowEntered, this );
			P->GetSignal( sfg::Window::OnMouseLeave ).Connect( &Screen::OnWindowLeft, this );

			Engine::out() << "[Screen] Adding Window " << P->GetTitle().toAnsiString() << std::endl;
			Desktop->Add(P);
		}
		else
		{
			Engine::out(Engine::ERROR) << "[Screen] SCREEN_ADD_WIDGET Event with wrong parameters" << std::endl;
		}
	}
	else if (e.Is("DISPLAY_GRAPH"))
	{
		if (e.Data().type() == typeid( std::shared_ptr<GraphPlotter> ) )
		{

			std::shared_ptr<GraphPlotter> p = boost::any_cast<std::shared_ptr<GraphPlotter>>(e.Data());
			if (!p)
			{
				Engine::out() << "[Screen] INVALID POINTER!" << std::endl;
			}

			sf::RenderTexture tex;
			tex.create(p->getGraph().Size.x, p->getGraph().Size.y);
			p->updateVertexArrays();
			p->draw( tex );
			tex.display();

			sfg::Window::Ptr P = sfg::Window::Create();
			sfg::Image::Ptr I = sfg::Image::Create( tex.getTexture().copyToImage() );
			P->Add(I);

			Event ev( "SCREEN_ADD_WINDOW");
			e.SetData( P );
			Module::Get()->QueueEvent(ev);
		}
		else
		{
			Engine::out(Engine::ERROR) << "[Screen] DISPLAY_GRAPH Event with wrong parameters" << std::endl;
		}



	}
	else if (e.Is("EVT_QUIT"))
	{
		Module::Get()->RequestQuit();
	}
}

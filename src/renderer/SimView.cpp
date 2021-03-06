#include "SimView.hpp"

#include "sbe/event/SFMLEventUser.hpp"
#include "sbe/ResourceManager.hpp"
#include "sbe/Config.hpp"
#include "sbe/Module.hpp"

#include "sbe/gfx/Camera.hpp"
#include "sbe/gfx/Renderer.hpp"
#include "sbe/gfx/GraphPlotter.hpp"
#include "sbe/gfx/Screen.hpp"
#include "sbe/gfx/ImageSet.hpp"

#include "renderer/ui/windows/Control.hpp"
#include "renderer/ui/windows/InfoPanel.hpp"
//#include "renderer/items/Manipulator.hpp"
#include "renderer/ui/windows/MainMenu.hpp"
#include "renderer/ui/windows/GraphBook.hpp"
#include "renderer/ui/windows/NewSimulation.hpp"

#include "renderer/SimActors.hpp"

#include "sbe/sfg/DebugWindow.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector2.hpp>

#include <SFGUI/Widget.hpp>

SimView::SimView()
{
	InitRenderer();
	// has to be after InitRenderer
	LoadResources();
	InitDesktop();
	ISim.reset ( new SimActors );

	Module::Get()->QueueEvent("TOGGLE_SHOW_MAINMENU");
}

SimView::~SimView()
{
	ISim.reset();
	Contr.reset();
    IPan.reset();
	//Man;
	DbgWin.reset();
	NewWin.reset();
	MnMnWin.reset();
	GraBo.reset();
}

void SimView::InitRenderer()
{

	// add our RenderLayers with our default Cam
	for (int i = L_BACK; i < L_END; ++i)
		sbe::Screen::sRndr()->addLayer( sbe::RenderLayer( (i==L_BACK? nullptr : sbe::Screen::sCam() ) ));

	sbe::Screen::sRndr()->getLayer( L_BACK )->Cam.reset();
}

void SimView::InitDesktop()
{
	Engine::out(Engine::INFO) << "[SimView] Creating Desktop Windows..." << std::endl;

    Contr.reset   ( new Control() );
    IPan.reset    ( new InfoPanel() );
    //Man.reset     ( new Manipulator() );
	DbgWin.reset  ( new sbe::DebugWindow() );
	NewWin.reset  ( new NewSimWindow() );
	MnMnWin.reset ( new MainMenu() );
	GraBo.reset   ( new GraphBook() );

	sbe::Screen::get()->addSFMLEventHandler( this );
}

void SimView::HandleSfmlEvent ( const sf::Event& e)
{
	static const int TileSize = Engine::getCfg()->get<int>("system.ui.simView.tileSize");

	if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left)
	{
		// Translate the click position to Terrain Coordinates (float)
		sf::Vector2i ClickPos( e.mouseButton.x, e.mouseButton.y );

		sf::Vector2f RealPos = Engine::GetApp().mapPixelToCoords( ClickPos, sbe::Screen::sCam()->getView());

		RealPos /= (float)TileSize;

		Module::Get()->QueueEvent( Event("TERRAIN_CLICKED", glm::point2( RealPos.x, RealPos.y )), true);
	}
}

bool SimView::LoadResources()
{
	auto txts = Engine::GetIO()->loadPath<sf::Image>( Engine::getCfg()->get<std::string>("system.renderer.creatureTexture") );
	if (txts.size() == 1) Engine::GetResMgr()->add(txts[0], Engine::getCfg()->get<std::string>("system.renderer.creatureTexture") );
	else
	{
		Engine::out(Engine::ERROR) << "[SimView] Error loading Textures" << std::endl;
		return false;
	}


	std::shared_ptr<sbe::ImageSet> I( new sbe::ImageSet( Engine::getCfg()->get<std::string>("system.renderer.creatureImageSet"),
											Engine::getCfg()->get<std::string>("system.renderer.creatureTexture"),
											glm::ipoint2( 0, 0 ),
											glm::ipoint2(0,0),
											glm::ivec2( 16,16),
											glm::ivec2( 3, 1 ),
											0 ) );

	Engine::GetResMgr()->add(I ,Engine::getCfg()->get<std::string>("system.renderer.creatureImageSet"));
	I->updateTexture();
	//Engine::GetIO()->saveObject<ImageSet>( );

	/// Creature Shaders
	std::shared_ptr<sf::Shader> highlightShader( new sf::Shader );
	highlightShader->loadFromFile("res/shader/highlight.vert", "res/shader/highlight.frag");
	highlightShader->setParameter("creatures", *(I->getTexture()));
	Engine::GetResMgr()->add( highlightShader, "highlightShader" );
	sbe::Screen::sRndr()->getLayer( L_CREATURES )->States.shader  = &(*highlightShader);

	sbe::Screen::sRndr()->getLayer( L_CREATURES )->States.texture = &(*(I->getTexture()));

	auto txt2 = Engine::GetIO()->loadPath<sf::Image>( Engine::getCfg()->get<std::string>("system.renderer.terrainTexture" ));
	if (txt2.size() == 1) Engine::GetResMgr()->add(txt2[0], Engine::getCfg()->get<std::string>("system.renderer.terrainTexture" ));
	else
	{
		Engine::out(Engine::ERROR) << "[SimView] Error loading Textures" << std::endl;
		return false;
	}

	std::string backgroundtxt = Engine::getCfg()->get<std::string>("system.renderer.backgroundTexture", "none" );
	if ( backgroundtxt != "none" )
	{
		auto txt3 = Engine::GetIO()->loadPath<sf::Image>( backgroundtxt);
		if (txt3.size() == 1) Engine::GetResMgr()->add(txt3[0], "backgroundimg");
		else
		{
			Engine::out(Engine::ERROR) << "[SimView] Error loading Textures" << std::endl;
			return false;
		}

		std::shared_ptr<sf::Texture> backtxt( new sf::Texture );
		backtxt->loadFromImage( *(txt3[0]) );
		backtxt->setRepeated( true );
		Engine::GetResMgr()->add(backtxt, "backgroundtxt");
	}



	std::shared_ptr<sbe::ImageSet> I2( new sbe::ImageSet( Engine::getCfg()->get<std::string>("system.renderer.terrainImageSet" ),
												Engine::getCfg()->get<std::string>("system.renderer.terrainTexture" ),
												glm::ipoint2( 0, 0 ),
												glm::ipoint2(0,0),
												glm::ivec2( 32,32),
												glm::ivec2( 4, 1 ),
												0 ) );

	Engine::GetResMgr()->add(I2 ,Engine::getCfg()->get<std::string>("system.renderer.terrainImageSet" ));
	I2->updateTexture();

	if ( Engine::getCfg()->get<bool>("system.ui.simView.useShaderTileMap") )
	{
		std::shared_ptr<sf::Shader> tilemapShader( new sf::Shader );
		tilemapShader->loadFromFile("res/shader/tilemap.vert", "res/shader/tilemap.frag");
		tilemapShader->setParameter("tileGraphics", *(I2->getTexture()));

		Engine::GetResMgr()->add( tilemapShader, "tilemapShader" );

		if ( !tilemapShader ) std::cout << "Error shader" << std::endl;
		if ( !sbe::Screen::sRndr() ) std::cout << "Error render" << std::endl;

		sbe::Screen::sRndr()->getLayer( L_TERRAIN )->States.shader  = &(*tilemapShader);
	}

	else
	{
		sbe::Screen::sRndr()->getLayer( L_TERRAIN )->States.texture = &(*(I2->getTexture()));
	}

	return true;
}

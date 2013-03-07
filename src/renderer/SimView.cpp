#include "SimView.hpp"

#include "sbe/event/SFMLEventUser.hpp"
#include "sbe/ResourceManager.hpp"
#include "sbe/gfx/ImageSet.hpp"
#include "sbe/Config.hpp"

#include "sbe/gfx/Camera.hpp"
#include "sbe/gfx/Renderer.hpp"
#include "sbe/gfx/GraphPlotter.hpp"
#include "sbe/gfx/Screen.hpp"

#include "renderer/items/Control.hpp"
#include "renderer/items/InfoPanel.hpp"
//#include "renderer/items/Manipulator.hpp"
#include "renderer/items/MainMenu.hpp"
#include "renderer/items/MiniMap.hpp"

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

}

SimView::~SimView()
{
	ISim.reset();
	Contr.reset();
    IPan.reset();
	//Man;
	DbgWin.reset();
	MnMnWin.reset();
	MiMap.reset();
}

void SimView::InitRenderer()
{

	// add our RenderLayers with our default Cam
	for (int i = L_BACK; i < L_END; ++i)
		Screen::sRndr()->addLayer( RenderLayer( Screen::sCam() ) );
}

void SimView::InitDesktop()
{
	Engine::out(Engine::INFO) << "[SimView] Creating Desktop Windows..." << std::endl;

    Contr.reset   ( new Control() );
    IPan.reset    ( new InfoPanel() );
    //Man.reset     ( new Manipulator() );
	DbgWin.reset  ( new DebugWindow() );
	MnMnWin.reset ( new MainMenu() );
	MiMap.reset   ( new MiniMap() );

	Screen::get()->addSFMLEventHandler( this );
}

void SimView::HandleSfmlEvent ( const sf::Event& e)
{
	static const int TileSize = Engine::getCfg()->get<int>("system.ui.simView.tileSize");

	if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left)
	{
		// Translate the click position to Terrain Coordinates (float)
		sf::Vector2i ClickPos( e.mouseButton.x, e.mouseButton.y );

		sf::Vector2f RealPos = Engine::GetApp().mapPixelToCoords( ClickPos, Screen::sCam()->getView());

		RealPos /= (float)TileSize;

		Event ev("TERRAIN_CLICKED");
		ev.SetData( Geom::Pointf( RealPos.x, RealPos.y ) );

		Module::Get()->QueueEvent( ev, true);
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


	std::shared_ptr<ImageSet> I( new ImageSet( Engine::getCfg()->get<std::string>("system.renderer.creatureImageSet"), Engine::getCfg()->get<std::string>("system.renderer.creatureTexture"), Geom::Point( 0, 0 ), Geom::Point(0,0), Geom::Vec2( 16,16), Geom::Vec2( 3, 1 ), 0 ) );
	Engine::GetResMgr()->add(I ,Engine::getCfg()->get<std::string>("system.renderer.creatureImageSet"));
	I->updateTexture();
	//Engine::GetIO()->saveObject<ImageSet>( );

	auto txt2 = Engine::GetIO()->loadPath<sf::Image>( Engine::getCfg()->get<std::string>("system.renderer.terrainTexture" ));
	if (txt2.size() == 1) Engine::GetResMgr()->add(txt2[0], Engine::getCfg()->get<std::string>("system.renderer.terrainTexture" ));
	else
	{
		Engine::out(Engine::ERROR) << "[SimView] Error loading Textures" << std::endl;
		return false;
	}



	std::shared_ptr<ImageSet> I2( new ImageSet( Engine::getCfg()->get<std::string>("system.renderer.terrainImageSet" ), Engine::getCfg()->get<std::string>("system.renderer.terrainTexture" ), Geom::Point( 0, 0 ), Geom::Point(0,0), Geom::Vec2( 32,32), Geom::Vec2( 4, 1 ), 0 ) );
	Engine::GetResMgr()->add(I2 ,Engine::getCfg()->get<std::string>("system.renderer.terrainImageSet" ));
	I2->updateTexture();

	Engine::GetResMgr()->saveAllObjects<ImageSet>( true );

	if ( Engine::getCfg()->get<bool>("system.ui.simView.useShaderTileMap") )
	{
		std::shared_ptr<sf::Shader> tilemapShader( new sf::Shader );
		tilemapShader->loadFromFile("res/shader/tilemap.vert", "res/shader/tilemap.frag");
		tilemapShader->setParameter("tileGraphics", *(I2->getTexture()));

		Engine::GetResMgr()->add( tilemapShader, "tilemapShader" );

		if ( !tilemapShader ) std::cout << "Error shader" << std::endl;
		if ( !Screen::sRndr() ) std::cout << "Error render" << std::endl;

		Screen::sRndr()->getLayer( L_TERRAIN )->States.shader  = &(*tilemapShader);
	}

	return true;
}

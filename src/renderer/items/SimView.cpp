#include "SimView.hpp"



#include "sbe/ResourceManager.hpp"
#include "sbe/gfx/ImageSet.hpp"
#include "sbe/Config.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>

#include <cmath>

SimView::SimView()
: Name("SimView")
{
	RegisterForEvent( "WINDOW_RESIZE" );

	Cam.reset ( new Camera );
	Cam->Setup();

	// add our RenderLayers with our default Cam
	for (int i = L_BACK; i < L_END; ++i)
		Picasso.addLayer( RenderLayer( Cam ) );


	LoadResources();

	ISim.reset ( new SimActors( Picasso ) );
}

void SimView::HandleEvent(Event& e)
{
	if(e.Is("WINDOW_RESIZE")){
		Cam->setTargetSize(sf::Vector2f( Engine::GetApp().getSize().x, Engine::GetApp().getSize().y));
	}
}

void SimView::HandleSfmlEvent ( const sf::Event& e)
{
	static const int TileSize = Engine::getCfg()->get<int>("ui.simView.tileSize");
	Cam->HandleEvent( e );

	switch (e.type)
	{
		case sf::Event::MouseButtonPressed:
			if (e.mouseButton.button == sf::Mouse::Left)
			{
				// Translate the click position to Terrain Coordinates (float)
				sf::Vector2i ClickPos( e.mouseButton.x, e.mouseButton.y );

				sf::Vector2f RealPos = Engine::GetApp().mapPixelToCoords( ClickPos, Cam->getView());

				RealPos /= (float)TileSize;

				Event ev("TERRAIN_CLICKED");
				ev.SetData( Geom::Pointf( RealPos.x, RealPos.y ) );

				Module::Get()->QueueEvent( ev, true);
			}
			break;
		default:
			break;
	}
}

bool SimView::LoadResources()
{
	auto txts = Engine::GetIO()->loadPath<sf::Image>( Engine::getCfg()->get<std::string>("renderer.creatureTexture") );
	if (txts.size() == 1) Engine::GetResMgr()->add(txts[0], Engine::getCfg()->get<std::string>("renderer.creatureTexture") );
	else
	{
		Engine::out(Engine::ERROR) << "[SimView] Error loading Textures" << std::endl;
		return false;
	}


	std::shared_ptr<ImageSet> I( new ImageSet( Engine::getCfg()->get<std::string>("renderer.creatureImageSet"), Engine::getCfg()->get<std::string>("renderer.creatureTexture"), Geom::Point( 0, 0 ), Geom::Point(0,0), Geom::Vec2( 16,16), Geom::Vec2( 3, 1 ), 0 ) );
	Engine::GetResMgr()->add(I ,Engine::getCfg()->get<std::string>("renderer.creatureImageSet"));
	I->updateTexture();
	//Engine::GetIO()->saveObject<ImageSet>( );

	auto txt2 = Engine::GetIO()->loadPath<sf::Image>( Engine::getCfg()->get<std::string>("renderer.terrainTexture" ));
	if (txt2.size() == 1) Engine::GetResMgr()->add(txt2[0], Engine::getCfg()->get<std::string>("renderer.terrainTexture" ));
	else
	{
		Engine::out(Engine::ERROR) << "[SimView] Error loading Textures" << std::endl;
		return false;
	}



	std::shared_ptr<ImageSet> I2( new ImageSet( Engine::getCfg()->get<std::string>("renderer.terrainImageSet" ), Engine::getCfg()->get<std::string>("renderer.terrainTexture" ), Geom::Point( 0, 0 ), Geom::Point(0,0), Geom::Vec2( 32,32), Geom::Vec2( 4, 1 ), 0 ) );
	Engine::GetResMgr()->add(I2 ,Engine::getCfg()->get<std::string>("renderer.terrainImageSet" ));
	I2->updateTexture();

	Engine::GetResMgr()->saveAllObjects<ImageSet>( true );

	if ( Engine::getCfg()->get<bool>("ui.simView.useShaderTileMap") )
	{
		std::shared_ptr<sf::Shader> tilemapShader( new sf::Shader );
		tilemapShader->loadFromFile("res/shader/tilemap.vert", "res/shader/tilemap.frag");
		tilemapShader->setParameter("tileGraphics", *(I2->getTexture()));

		Engine::GetResMgr()->add( tilemapShader, "tilemapShader" );
		Picasso.getLayer( L_TERRAIN )->States.shader  = &(*tilemapShader);
		Picasso.getLayer( L_TERRAIN )->States.texture = &(*(I2->getTexture()));
	}

	return true;
}

void SimView::Render()
{
	Cam->Update();
	Picasso.render( Engine::GetApp() );
	PostDebugInfo();
}

void SimView::PostDebugInfo()
{
	Cam->showDebugInfo();
}

#include "SimView.hpp"

#include "simulator/Simulator.hpp"
#include "simulator/Terrain.hpp"
#include "simulator/Creature.hpp"
#include "simulator/Species.hpp"
#include "simulator/Tile.hpp"

#include "sbe/ResourceManager.hpp"
#include "sbe/gfx/ImageSet.hpp"
#include "sbe/Config.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>

#include <cmath>

SimView::SimView()
: Name("SimView")
{

	TileSize = 	Engine::getCfg()->get<int>("ui.simView.tileSize");
	TerrainSize = 	Engine::getCfg()->get<int>("sim.terragen.debug.size");
	RenderGrid = Engine::getCfg()->get<bool>("ui.simView.renderGrid");
	CreatureSize = Engine::getCfg()->get<int>("ui.simView.creatureSize");

	RegisterForEvent("UpdateCreatureRenderList");
	RegisterForEvent("UpdateTileRenderList");
	RegisterForEvent("UpdateTilemapTexture");
	RegisterForEvent("WINDOW_RESIZE");

	GridColor = sf::Color( Engine::getCfg()->get<int>("ui.simView.gridColor.r"),
						Engine::getCfg()->get<int>("ui.simView.gridColor.g"),
						Engine::getCfg()->get<int>("ui.simView.gridColor.b") );
	Cam.Setup();

	tileGraphics.loadFromFile("res/textures/Tiles_serious.tga");
	tileGraphicsTexture.loadFromImage(tileGraphics);

	tilemapShader.loadFromFile("res/shader/tilemap.vert", "res/shader/tilemap.frag");
	tilemapShader.setParameter("tileGraphics", tileGraphicsTexture);
	tilemapShader.setParameter("tilemap", tilemapTexture);

	tilemapState = sf::RenderStates(&tilemapShader);
}

void SimView::HandleEvent(Event& e)
{
	if(e.Is("UpdateCreatureRenderList"))
	{		// check event datatype

		if (e.Data().type() == typeid( std::list<std::shared_ptr<Creature>> ))
		{
			// cast into desired type
			std::list<std::shared_ptr<Creature>> r = boost::any_cast< std::list<std::shared_ptr<Creature>> >(e.Data());
			//std::list<std::shared_ptr<Creature>> CullRenderList( r );
			ReadCreatureRenderList( r );
		}
		else
		{
			Engine::out(Engine::ERROR) << "[SimView] Wrong eventdata at UpdateCreatureRenderList!" << std::endl;
		}
		//Engine::out() << "[SimView] Creatures NOT updated" << std::endl;
	}
	else if ( e.Is("UpdateTileRenderList"))
	{
		// check event datatype
		if (e.Data().type() == typeid( std::vector<std::shared_ptr<Tile>> ))
		{
			// cast into desired type
			std::vector<std::shared_ptr<Tile>> r = boost::any_cast< std::vector<std::shared_ptr<Tile>> >(e.Data());
			ReadTileRenderList( r );
		}
		else
		{
			Engine::out(Engine::ERROR) << "[SimView] Wrong eventdata at UpdateTileRenderList!" << std::endl;
		}
	} else if ( e.Is("UpdateTilemapTexture"))
	{
		// check event datatype
		if (e.Data().type() == typeid( std::shared_ptr<sf::Image> ))
		{
			// cast into desired type
			std::shared_ptr<sf::Image> i = boost::any_cast< std::shared_ptr<sf::Image> >(e.Data());
			tilemapTexture.create( i->getSize().x, i->getSize().y );
			tilemapTexture.loadFromImage( *i );
		}
		else
		{
			Engine::out(Engine::ERROR) << "[SimView] Wrong eventdata at UpdateTileRenderList!" << std::endl;
		}
	} else if(e.Is("WINDOW_RESIZE")){
		Cam.setTargetSize(sf::Vector2f( Engine::GetApp().getSize().x, Engine::GetApp().getSize().y));
	}
}

void SimView::HandleSfmlEvent ( const sf::Event& e)
{
	Cam.HandleEvent( e );

	switch (e.type)
	{
		case sf::Event::MouseButtonPressed:
			if (e.mouseButton.button == sf::Mouse::Left)
			{
				// Translate the click position to Terrain Coordinates (float)
				sf::Vector2i ClickPos( e.mouseButton.x, e.mouseButton.y );

				sf::Vector2f RealPos = Engine::GetApp().mapPixelToCoords( ClickPos, Cam.getView());

				RealPos = RealPos / (float)TileSize;

				Event ev("TERRAIN_CLICKED");
				ev.SetData( Geom::Pointf( RealPos.x, RealPos.y ) );

				Module::Get()->QueueEvent( ev, true);
			}
			break;
		default:
			break;
	}
}

void SimView::Render()
{
	std::shared_ptr<ImageSet> TileImgSet = Engine::GetResMgr()->get<ImageSet>("Tiles");
	std::shared_ptr<ImageSet> CreatureImgSet = Engine::GetResMgr()->get<ImageSet>("Creatures");


	Cam.Update();

	Engine::GetApp().setView(Cam.getView());


	//if (TileImgSet->getTexture())
	//	Engine::GetApp().draw( Tiles, TileImgSet->getTexture().get());

	tilemapSprite.setTexture(tilemapTexture);

	// has to be the size of the terrain multiplied by the size of a tile
	// may produce glitches in some circumstances one is if the Terrain is
	// to small
  	tilemapSprite.setTextureRect(sf::IntRect(0,0,256*32,256*32));

  	// has to be the center of the terrain
  	// if you want to draw it on center
  	tilemapSprite.setScale(4.0f, 4.0f);
  	tilemapSprite.setPosition(-512*28,-512*28);

	Engine::GetApp().draw( tilemapSprite, tilemapState );

	if (CreatureImgSet->getTexture())
		Engine::GetApp().draw( Creatures , CreatureImgSet->getTexture().get());

	if (RenderGrid)
		Engine::GetApp().draw( Grid );

	Engine::GetApp().setView( Engine::GetApp().getDefaultView());

	PostDebugInfo();
}

void SimView::PostDebugInfo()
{
	Cam.showDebugInfo();
}

void SimView::ReadTileRenderList(TileRenderList& r)
{

	Tiles.clear();
	Tiles.resize( 4 * r.size() );
	Tiles.setPrimitiveType( sf::PrimitiveType::Quads );

	TerrainSize = std::sqrt( r.size() );

	std::shared_ptr<ImageSet> ImgSet = Engine::GetResMgr()->get<ImageSet>("Tiles");
	ImgSet->updateTexture();

	if (!ImgSet->getTexture())
	{
		Engine::out(Engine::WARNING) << "[SimView] Unable to get texture for tile rendering!" << std::endl;
		return;
	}

	int i = 0;

	for ( std::shared_ptr<Tile> T : r)
	{
		ImgSet->CreateQuad( T->getTileSpriteIndex() , Tiles, DetermineTilePos( T ) , (i++ * 4));
	}

	Engine::out() << "[SimView] Recreated tiles vertexarray!" << std::endl;

	// and create the corresponding grid
	CreateGrid();
}


void SimView::ReadCreatureRenderList(CreatureRenderList& r)
{
	Creatures.clear();
	Creatures.resize( 4 * r.size() );
	Creatures.setPrimitiveType( sf::PrimitiveType::Quads );

	std::shared_ptr<ImageSet> ImgSet = Engine::GetResMgr()->get<ImageSet>("Creatures");
	ImgSet->updateTexture();

	if (!ImgSet->getTexture())
	{
		Engine::out(Engine::WARNING) << "[SimView] Unable to get texture for creature rendering!" << std::endl;
		return;
	}

	int i = 0;

	for ( std::shared_ptr<Creature> T : r)
	{
		ImgSet->CreateQuad( DetermineCreatureSpriteIndex( T ), Creatures, DetermineCreaturePos(T), (i++ * 4) );
	}

	//Engine::out() << "[SimView] Recreated creature vertexarray!" << std::endl;
}

void SimView::CreateGrid()
{
	Engine::out() << "[SimView] Updated Grid" << std::endl;

	Grid.setPrimitiveType ( sf::PrimitiveType::Lines );
	Grid.clear();
	Grid.resize( 2* ((TerrainSize+1) * 2 ) );

	for ( int i = 0; i <= TerrainSize; ++i)
	{
		int pos = 2*i;
		Grid[ pos ].position = sf::Vector2f( 0, i * TileSize );
		Grid[ pos + 1 ].position = sf::Vector2f( TerrainSize * TileSize, i * TileSize );

		Grid[ pos ].color = GridColor;
		Grid[ pos + 1 ].color = GridColor;
	}

	for ( int i = 0; i <= TerrainSize; ++i)
	{
		int pos = (TerrainSize+1)*2 + 2*i;
		Grid[ pos  ].position = sf::Vector2f( i * TileSize, 0 );
		Grid[ pos + 1 ].position = sf::Vector2f( i * TileSize, TerrainSize * TileSize );

		Grid[ pos ].color = GridColor;
		Grid[ pos + 1 ].color = GridColor;
	}


}




// ### TERRAIN/TILE STUFF ##

sf::FloatRect SimView::DetermineTilePos( std::shared_ptr<Tile>& t)
{
	sf::FloatRect re;

	re.left 	= TileSize * t->getPosition().x;
	re.top 		= TileSize * t->getPosition().y;
	re.width 	= TileSize;
	re.height 	= TileSize;

	return re;
}

sf::FloatRect SimView::DetermineCreaturePos( std::shared_ptr<Creature>& c)
{
	sf::FloatRect re;

	re.left 	= TileSize * c->getPosition().x - CreatureSize/2;
	re.top 		= TileSize * c->getPosition().y - CreatureSize/2;
	re.width 	= CreatureSize;
	re.height 	= CreatureSize;

	return re;
}

int SimView::DetermineCreatureSpriteIndex ( std::shared_ptr<Creature>& t)
{
	/// FIXME: hard coded
	return t->getSpecies()->getType();
}

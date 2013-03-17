#include "SimActors.hpp"

#include "sbe/gfx/Actor.hpp"
#include "sbe/gfx/Renderer.hpp"
#include "sbe/gfx/Camera.hpp"
#include "sbe/Config.hpp"
#include "sbe/ResourceManager.hpp"

#include "sbe/gfx/GraphPlotter.hpp"
#include "sbe/gfx/MapPlotter.hpp"
#include "sbe/gfx/ImageUtils.hpp"
#include "sbe/gfx/Screen.hpp"

#include "simulator/world/Creature.hpp"
#include "simulator/world/Tile.hpp"

#include <SFGUI/Window.hpp>
#include <SFGUI/Image.hpp>

SimActors::SimActors()
{
	TileSize =         Engine::getCfg()->get<int>("system.ui.simView.tileSize");
	TerrainSize =      Engine::getCfg()->get<int>("sim.terragen.debug.size");
	CreatureSize =     Engine::getCfg()->get<int>("system.ui.simView.creatureSize");
	RenderGrid =       Engine::getCfg()->get<bool>("system.ui.simView.renderGrid");
	useShaderTileMap = Engine::getCfg()->get<bool>("system.ui.simView.useShaderTileMap");
	cullThreshold =    Engine::getCfg()->get<int>("system.ui.simView.cullThreshold");

	MessageHandler_.reset( new MessageHandler() );

	RegisterForEvent("UpdateCreatureRenderList");
	RegisterForEvent("UpdateTileRenderList");
	RegisterForEvent("UpdateTilemapTexture");

	GridColor = sf::Color( Engine::getCfg()->get<int>("system.ui.simView.gridColor.r"),
						Engine::getCfg()->get<int>("system.ui.simView.gridColor.g"),
						Engine::getCfg()->get<int>("system.ui.simView.gridColor.b") );

}

SimActors::~SimActors()
{
}

void SimActors::HandleEvent(Event& e)
{
	if(e.Is("UpdateCreatureRenderList", typeid( std::list<std::shared_ptr<Creature>> )))
	{
		std::list<std::shared_ptr<Creature>> r = boost::any_cast< std::list<std::shared_ptr<Creature>> >(e.Data());
		//std::list<std::shared_ptr<Creature>> CullRenderList( r );
		ReadCreatureRenderList( r );
	}
	else if ( e.Is("UpdateTileRenderList", typeid( std::vector<std::shared_ptr<Tile>> )))
	{
		std::vector<std::shared_ptr<Tile>> r = boost::any_cast< std::vector<std::shared_ptr<Tile>> >(e.Data());
		ReadTileRenderList( r );
	} else if ( e.Is("UpdateTilemapTexture", typeid( std::shared_ptr<sf::Image> )))
	{
		if ( useShaderTileMap )
		{
			std::shared_ptr<sf::Image> i = boost::any_cast< std::shared_ptr<sf::Image> >(e.Data());
			tilemapTexture.create( i->getSize().x, i->getSize().y );
			tilemapTexture.loadFromImage( *i );
			CreateTerrainShaderMap();
		}
	}
}

void SimActors::ReadCreatureRenderList(CreatureRenderList& r)
{
	bool newActor = false;
	if ( !CreaturesActor )
	{
		CreaturesActor.reset( new VertexActor() );
		newActor = true;
	}

	sf::VertexArray& Creatures = (std::dynamic_pointer_cast<VertexActor>(CreaturesActor))->arr;
	Creatures.clear();
	Creatures.setPrimitiveType( sf::PrimitiveType::Quads );

	bool cull = r.size() > cullThreshold;
	auto imgs = Engine::GetResMgr()->get<ImageSet>("Creatures");

	for ( std::shared_ptr<Creature> C : r)
	{
		auto Pos = DetermineCreaturePos( C );
		if ( !cull || Screen::sCam()->getDrawnArea().intersects(Pos) ) imgs->CreateQuad( DetermineCreatureSpriteIndex( C ) , Creatures, Pos );
	}

	//Engine::out() << "[SimActors] Recreated creature vertexarray!" << r.size() << std::endl;

	if ( newActor )
	{
		Screen::get()->getRenderer()->addActor ( CreaturesActor, L_CREATURES );
	}

}

void SimActors::ReadTileRenderList(TileRenderList& r)
{
	TerrainSize = std::sqrt( r.size() );

	if ( !useShaderTileMap )
	{
		CreateTerrainVertexArray( r );
		Engine::out() << "[SimActors] Recreated tiles vertexarray!" << std::endl;
	}
	// and create the corresponding grid
	CreateGrid();
}

void SimActors::CreateTerrainVertexArray(TileRenderList& r)
{
	bool newActor = false;
	if ( !TileActor )
	{
		TileActor.reset( new VertexActor() );
		newActor = true;
	}

	sf::VertexArray& Tiles = (dynamic_pointer_cast<VertexActor>(TileActor))->arr;

	Tiles.clear();
	Tiles.resize( 4 * r.size() );
	Tiles.setPrimitiveType( sf::PrimitiveType::Quads );



	int i = 0;
	for ( std::shared_ptr<Tile> T : r)
		Engine::GetResMgr()->get<ImageSet>("Tiles")->CreateQuad( T->getTileSpriteIndex(), Tiles, DetermineTilePos(T), (i++ * 4) );



	if ( newActor )
		Screen::get()->getRenderer()->addActor ( TileActor, L_TERRAIN );
}

void SimActors::CreateTerrainShaderMap()
{
	bool newActor = false;
	if ( !TileActor )
	{
		TileActor.reset( new SpriteActor() );
		newActor = true;
	}

	TerrainSize = tilemapTexture.getSize().x;

	std::shared_ptr<sf::Shader> tilemapShader = Engine::GetResMgr()->get<sf::Shader>( "tilemapShader" );
	std::shared_ptr<ImageSet> TileImgSet = Engine::GetResMgr()->get<ImageSet>("Tiles");

	tilemapShader->setParameter("tilemap", tilemapTexture);
	// amount of tiles
	tilemapShader->setParameter("TerrainSize", TerrainSize);
	Engine::out() << "Shaderparam TerrainSize " << TerrainSize << std::endl;
	// amount of sprites in the imageset
	tilemapShader->setParameter("AtlasSize", sf::Vector2f(TileImgSet->FrameCount.x, TileImgSet->FrameCount.y) );
	Engine::out() << "Shaderparam AtlasSize " << TileImgSet->FrameCount << std::endl;
	// how big should each tile be rendered
	tilemapShader->setParameter("TileRenderSize", TileSize);
	Engine::out() << "Shaderparam TileRenderSize " << TileSize << std::endl;

	Screen::get()->getRenderer()->getLayer( L_TERRAIN )->States.shader = &(*tilemapShader);


	sf::Sprite& sprite = (dynamic_pointer_cast<SpriteActor>(TileActor))->sprite;

	sprite.setTexture( tilemapTexture );

	// has to be the size of the terrain multiplied by the size of a tile
	// may produce glitches in some circumstances one is if the Terrain is
	// to small
  	sprite.setTextureRect(sf::IntRect(0,0,TileSize*TerrainSize,TileSize*TerrainSize));
  	sprite.setScale(1.0f, 1.0f);
  	sprite.setPosition(0,0);

	if ( newActor )
		Screen::get()->getRenderer()->addActor ( TileActor, L_TERRAIN );

	Engine::out(Engine::INFO) << "[SimView] Updated Tilemap Shader!" << std::endl;
}


void SimActors::CreateGrid()
{
	bool newActor = false;
	if ( !GridActor )
	{
		GridActor.reset( new VertexActor() );
		newActor = true;
	}

	Engine::out() << "[SimActors] Updated Grid" << std::endl;

	sf::VertexArray& Grid = (dynamic_pointer_cast<VertexActor>(GridActor))->arr;

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

	if ( newActor )
		Screen::get()->getRenderer()->addActor ( GridActor, L_OVERLAY );
}




// ### TERRAIN/TILE STUFF ##

sf::FloatRect SimActors::DetermineTilePos( std::shared_ptr<Tile>& t)
{
	sf::FloatRect re;

	re.left 	= TileSize * t->getPosition().x;
	re.top 		= TileSize * t->getPosition().y;
	re.width 	= TileSize;
	re.height 	= TileSize;

	return re;
}

sf::FloatRect SimActors::DetermineCreaturePos( std::shared_ptr<Creature>& c)
{
	sf::FloatRect re;

	re.left 	= TileSize * c->getPosition().x - CreatureSize/2;
	re.top 		= TileSize * c->getPosition().y - CreatureSize/2;
	re.width 	= CreatureSize;
	re.height 	= CreatureSize;

	return re;
}

int SimActors::DetermineCreatureSpriteIndex ( std::shared_ptr<Creature>& t)
{
	/// FIXME: hard coded
	return t->getSpecies()->getType();
}

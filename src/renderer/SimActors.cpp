#include "SimActors.hpp"

#include "sbe/gfx/Actor.hpp"
#include "sbe/gfx/Renderer.hpp"
#include "sbe/Config.hpp"
#include "sbe/ResourceManager.hpp"

#include "simulator/Creature.hpp"
#include "simulator/Tile.hpp"

SimActors::SimActors(Renderer& R)
 : Picasso( R)
{
	TileSize = 	Engine::getCfg()->get<int>("system.ui.simView.tileSize");
	TerrainSize = 	Engine::getCfg()->get<int>("system.sim.terragen.debug.size");
	CreatureSize = Engine::getCfg()->get<int>("system.ui.simView.creatureSize");
	RenderGrid = Engine::getCfg()->get<bool>("system.ui.simView.renderGrid");
	useShaderTileMap = Engine::getCfg()->get<bool>("system.ui.simView.useShaderTileMap");

	RegisterForEvent("UpdateCreatureRenderList");
	RegisterForEvent("UpdateTileRenderList");
	RegisterForEvent("UpdateTilemapTexture");

	GridColor = sf::Color( Engine::getCfg()->get<int>("system.ui.simView.gridColor.r"),
						Engine::getCfg()->get<int>("system.ui.simView.gridColor.g"),
						Engine::getCfg()->get<int>("system.ui.simView.gridColor.b") );

}


void SimActors::HandleEvent(Event& e)
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
			CreateTerrainShaderMap();
		}
		else
		{
			Engine::out(Engine::ERROR) << "[SimView] Wrong eventdata at UpdateTileRenderList!" << std::endl;
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
	Creatures.resize( 4 * r.size() );
	Creatures.setPrimitiveType( sf::PrimitiveType::Quads );

	int i = 0;
	for ( std::shared_ptr<Creature> C : r)
		Engine::GetResMgr()->get<ImageSet>("Creatures")->CreateQuad( DetermineCreatureSpriteIndex( C ) , Creatures, DetermineCreaturePos( C ) , (i++ * 4));

	//Engine::out() << "[SimActors] Recreated creature vertexarray!" << std::endl;

	if ( newActor )
	{

		Picasso.getLayer( L_CREATURES )->States.texture = &(*(Engine::GetResMgr()->get<ImageSet>("Creatures")->getTexture()));
		Picasso.addActor ( CreaturesActor, L_CREATURES );
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
		Engine::GetResMgr()->get<ImageSet>("Creatures")->CreateQuad( T->getTileSpriteIndex(), Tiles, DetermineTilePos(T), (i++ * 4) );

	if ( newActor )
		Picasso.addActor ( TileActor, L_TERRAIN );
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

	tilemapShader->setParameter("tilemap", tilemapTexture);
	Picasso.getLayer( L_TERRAIN )->States.shader = &(*tilemapShader);

	std::shared_ptr<ImageSet> TileImgSet = Engine::GetResMgr()->get<ImageSet>("Tiles");
	sf::Sprite& sprite = (dynamic_pointer_cast<SpriteActor>(TileActor))->sprite;

	sprite.setTexture( *(TileImgSet->getTexture()) );
	TileImgSet->getTexture()->setRepeated(true);
	// has to be the size of the terrain multiplied by the size of a tile
	// may produce glitches in some circumstances one is if the Terrain is
	// to small
  	sprite.setTextureRect(sf::IntRect(0,0,TileSize*TerrainSize,TileSize*TerrainSize));
  	sprite.setScale(1.0f, 1.0f);
  	sprite.setPosition(0,0);

	if ( newActor )
		Picasso.addActor ( TileActor, L_TERRAIN );

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
		Picasso.addActor ( GridActor, L_OVERLAY );
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

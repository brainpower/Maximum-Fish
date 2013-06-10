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

#include "sbe/sfg/Message.hpp"
#include "sbe/sfg/MessageHandler.hpp"

#include "simulator/world/Creature.hpp"
#include "simulator/world/Species.hpp"
#include "simulator/world/Tile.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

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

	MessageHandler_.reset( new sbe::MessageHandler() );
	MessageHandler_->setPauseEvent( "PAUSELOCK_UP" );

	RegisterForEvent("UpdateCreatureRenderList");
	RegisterForEvent("UpdateTileRenderList");
	RegisterForEvent("UpdateTilemapTexture");
	RegisterForEvent("CREATURE_CLICKED");

	RegisterForEvent("WINDOW_RESIZED");

	RegisterForEvent("RESET_SIMULATION");

	RegisterForEvent("EVT_SAVE_GOOD");
	RegisterForEvent("EVT_SAVE_BAD");
	RegisterForEvent("EVT_LOAD_BAD");
	RegisterForEvent("EVT_LOAD_GOOD");

	GridColor = sf::Color( Engine::getCfg()->get<int>("system.ui.simView.gridColor.r"),
						Engine::getCfg()->get<int>("system.ui.simView.gridColor.g"),
						Engine::getCfg()->get<int>("system.ui.simView.gridColor.b") );


	SetCamLimits();
	CreateBackground();
}

SimActors::~SimActors()
{
}

void SimActors::HandleEvent(Event& e)
{
	//if(e.Is("UpdateCreatureRenderList", typeid( std::vector<std::tuple<Geom::Pointf,int,Species*>> )))
	if(e.Is("UpdateCreatureRenderList", typeid( CreatureRenderList )))
	{
		CreatureRenderList& r = boost::any_cast< CreatureRenderList& >(e.Data());
		//CreatureRenderList CullRenderList( r );
		ReadCreatureRenderList( r );
	}
	else if ( e.Is("UpdateTileRenderList", typeid( std::vector<std::shared_ptr<Tile>> )))
	{
		std::vector<std::shared_ptr<Tile>> r = boost::any_cast< std::vector<std::shared_ptr<Tile>> >(e.Data());
		ReadTileRenderList( r );
	}
	else if ( e.Is("UpdateTilemapTexture", typeid( std::shared_ptr<sf::Image> )))
	{
		if ( useShaderTileMap )
		{
			std::shared_ptr<sf::Image> i = boost::any_cast< std::shared_ptr<sf::Image> >(e.Data());
			tilemapTexture.create( i->getSize().x, i->getSize().y );
			tilemapTexture.loadFromImage( *i );
			CreateTerrainShaderMap();
		}
	}
	else if ( (e.Is("EVT_SAVE_BAD", typeid(std::string)) || e.Is("EVT_LOAD_BAD", typeid(std::string))) )
	{
		std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::OK , "SAVE / LOAD ERROR!", boost::any_cast< std::string >(e.Data())) );
		Module::Get()->QueueEvent( Event("NEW_MESSAGE", M) );
	}
	else if ( (e.Is("EVT_SAVE_GOOD") || e.Is("EVT_LOAD_GOOD")) )
	{
		std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::OK , "SAVE / LOAD OK!", "Saving / Loading successfull!") );
		Module::Get()->QueueEvent( Event("NEW_MESSAGE", M) );
	}
	else if ( e.Is( "CREATURE_CLICKED", typeid( std::shared_ptr<Creature> )))
    {
		m_highlight = boost::any_cast<std::shared_ptr<Creature>>( e.Data() );
    }
    	else if ( e.Is( "WINDOW_RESIZED" ))
    {
		CreateBackground();
    }
	else if ( e.Is( "RESET_SIMULATION" ))
    {
		SetCamLimits();
    }
}

void SimActors::ReadCreatureRenderList(CreatureRenderList& r)
{
	bool newActor = false;
	if ( !CreaturesActor )
	{
		CreaturesActor.reset( new sbe::VertexActor() );
		newActor = true;
	}

	sf::VertexArray& Creatures = (std::dynamic_pointer_cast<sbe::VertexActor>(CreaturesActor))->arr;
	Creatures.clear();
	Creatures.setPrimitiveType( sf::PrimitiveType::Quads );

	bool cull = r.size() > cullThreshold;
	auto imgs = Engine::GetResMgr()->get<sbe::ImageSet>("Creatures");

	if ( m_highlight && m_highlight->getCurrentHealth() <= 0 ) m_highlight.reset();

	for ( CreatureRenderInfo& R : r)
	{
		auto Pos = DetermineCreaturePos(std::get<0>(R));
		if ( cull && !sbe::Screen::sCam()->getDrawnArea().intersects(Pos) ) continue;

		sf::Color Col = sf::Color::Black;
		// Determine Highlighting Color, if any
		if ( m_highlight )
		{
			// currently selected Creature
			if (Pos == DetermineCreaturePos(m_highlight)) {Col = sf::Color(255,255,0,0 );}
			// currently selected Species
			else if ( std::get<2>(R) == m_highlight->getSpecies().get()) {Col = sf::Color(255,0,0,0 );}
			// not selected, fade
			else {Col = sf::Color(0,0,0,0);}
		}

		imgs->CreateQuad( std::get<1>(R) , Creatures, Pos, -1, Col);
	}

	//Engine::out() << "[SimActors] Recreated creature vertexarray!" << r.size() << std::endl;

	if ( newActor )
		sbe::Screen::get()->getRenderer()->addActor ( CreaturesActor, L_CREATURES );

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
	SetCamLimits();
}

void SimActors::SetCamLimits()
{
	TileSize =         Engine::getCfg()->get<int>("system.ui.simView.tileSize");
	TerrainSize =      Engine::getCfg()->get<int>("sim.terragen.debug.size");

	Engine::out() << "[SimActors] caminfo " << TileSize << " - " << TerrainSize << std::endl;

	sbe::Screen::sCam()->setZoomLimits( sf::Vector2f( TileSize, TileSize*2 ), sf::Vector2f( TileSize*TerrainSize*3, TileSize*TerrainSize*2 ) );
	sbe::Screen::sCam()->setCamLimits( sf::FloatRect( -1000, -1000, TileSize*TerrainSize+2000, TileSize*TerrainSize+2000 ) );

	sbe::Screen::sCam()->setTargetCenter(sf::Vector2f((TerrainSize*TileSize)/2,(TerrainSize*TileSize)/2));
	sbe::Screen::sCam()->zoom ((TerrainSize*TileSize) / sbe::Screen::sCam()->getTargetSize().y );
}

void SimActors::CreateTerrainVertexArray(TileRenderList& r)
{
	bool newActor = false;
	if ( !TileActor )
	{
		TileActor.reset( new sbe::VertexActor() );
		newActor = true;
	}

	sf::VertexArray& Tiles = (std::dynamic_pointer_cast<sbe::VertexActor>(TileActor))->arr;

	Tiles.clear();
	Tiles.resize( 4 * r.size() );
	Tiles.setPrimitiveType( sf::PrimitiveType::Quads );



	int i = 0;
	for ( std::shared_ptr<Tile> T : r)
		Engine::GetResMgr()->get<sbe::ImageSet>("Tiles")->CreateQuad( T->getTileSpriteIndex(), Tiles, DetermineTilePos(T), (i++ * 4) );



	if ( newActor )
		sbe::Screen::get()->getRenderer()->addActor ( TileActor, L_TERRAIN );
}

void SimActors::CreateTerrainShaderMap()
{
	bool newActor = false;
	if ( !TileActor )
	{
		TileActor.reset( new sbe::SpriteActor() );
		newActor = true;
	}

	TerrainSize = tilemapTexture.getSize().x;

	std::shared_ptr<sf::Shader> tilemapShader = Engine::GetResMgr()->get<sf::Shader>( "tilemapShader" );
	std::shared_ptr<sbe::ImageSet> TileImgSet = Engine::GetResMgr()->get<sbe::ImageSet>("Tiles");

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

	sbe::Screen::get()->getRenderer()->getLayer( L_TERRAIN )->States.shader = &(*tilemapShader);


	sf::Sprite& sprite = (std::dynamic_pointer_cast<sbe::SpriteActor>(TileActor))->sprite;

	sprite.setTexture( tilemapTexture );

	// has to be the size of the terrain multiplied by the size of a tile
	// may produce glitches in some circumstances one is if the Terrain is
	// to small
  	sprite.setTextureRect(sf::IntRect(0,0,TileSize*TerrainSize,TileSize*TerrainSize));
  	sprite.setScale(1.0f, 1.0f);
  	sprite.setPosition(0,0);

	if ( newActor )
		sbe::Screen::get()->getRenderer()->addActor ( TileActor, L_TERRAIN );

	Engine::out(Engine::INFO) << "[SimView] Updated Tilemap Shader!" << std::endl;
}

void SimActors::CreateBackground()
{
	if ( !BackgroundActor )
	{
		BackgroundActor.reset( new sbe::SpriteActor() );
		sbe::Screen::get()->getRenderer()->addActor ( BackgroundActor, L_BACK );
	}

	sf::Sprite& sprite = (std::dynamic_pointer_cast<sbe::SpriteActor>(BackgroundActor))->sprite;
	std::shared_ptr<sf::Texture> txt = Engine::GetResMgr()->get<sf::Texture>( "backgroundtxt" );
	sprite.setTexture( *txt );
  	sprite.setTextureRect(sf::IntRect(0,0,Engine::GetApp().getSize().x,Engine::GetApp().getSize().y));
  	sprite.setScale(1.0f, 1.0f);
  	sprite.setPosition(0,0);
}

void SimActors::CreateGrid()
{
	bool newActor = false;
	if ( !GridActor )
	{
		GridActor.reset( new sbe::VertexActor() );
		newActor = true;
	}

	Engine::out() << "[SimActors] Updated Grid" << std::endl;

	sf::VertexArray& Grid = (std::dynamic_pointer_cast<sbe::VertexActor>(GridActor))->arr;

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
		sbe::Screen::get()->getRenderer()->addActor ( GridActor, L_OVERLAY );
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

sf::FloatRect SimActors::DetermineCreaturePos( Geom::Pointf& Pos)
{
	sf::FloatRect re;

	re.left 	= TileSize * Pos.x - CreatureSize/2;
	re.top 		= TileSize * Pos.y - CreatureSize/2;
	re.width 	= CreatureSize;
	re.height 	= CreatureSize;

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

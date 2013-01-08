#include "SimView.hpp"

#include "simulator/Simulator.hpp"
#include "simulator/Terrain.hpp"
#include "simulator/Creature.hpp"
#include "simulator/Tile.hpp"

#include "sbe/ResourceManager.hpp"
#include "sbe/ImageSet.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <cmath>

SimView::SimView()
: Name("SimView"),
 TileSize( 128 ),
 RenderGrid ( true )
{
	RegisterForEvent("UpdateCreatureRenderList");
	RegisterForEvent("UpdateTileRenderList");

	GridColor = sf::Color( 42, 42, 42 );
}

void SimView::HandleEvent(Event& e)
{
	if(e.Is("UpdateCreatureRenderList"))
	{		// check event datatype
		
		if (e.Data().type() == typeid( std::list<std::shared_ptr<Creature>> ))
		{
			// cast into desired type
			std::list<std::shared_ptr<Creature>> r = boost::any_cast< std::list<std::shared_ptr<Creature>> >(e.Data());
			ReadCreatureRenderList( r );
		}
		else
		{
			Engine::out() << "[SimView] Wrong eventdata at UpdateCreatureRenderList!" << std::endl;
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
			Engine::out() << "[SimView] Wrong eventdata at UpdateTileRenderList!" << std::endl;
		}
	}
}

void SimView::HandleSfmlEvent ( const sf::Event& e)
{

	if (e.type == sf::Event::EventType::KeyReleased)
	{

		int delta = 10;

		if (e.key.shift) delta *= 10;

		switch (e.key.code)
		{
			case sf::Keyboard::Key::Up:
				Camera.move(0, -delta);
				break;

			case sf::Keyboard::Key::Down:
				Camera.move(0, delta);
				break;

			case sf::Keyboard::Key::Left:
				Camera.move(-delta, 0);
				break;

			case sf::Keyboard::Key::Right:
				Camera.move(delta, 0);
				break;



			case sf::Keyboard::Key::PageUp:
				Camera.zoom(1.1);
				break;
			case sf::Keyboard::Key::PageDown:
				Camera.zoom(0.9);
				break;
			default:
				break;
		}
	}
	else if (e.type == sf::Event::EventType::Resized)
	{
		Camera.setSize(e.size.width, e.size.height);
	}
}

void SimView::SetupCamera()
{
	Camera.setSize(800, 600);
	Camera.setCenter(400,300);

	//Camera.move(0,500);
}

void SimView::Render()
{
	std::shared_ptr<ImageSet> TileImgSet = Engine::GetResMgr()->get<ImageSet>("Tiles");
	std::shared_ptr<ImageSet> CreatureImgSet = Engine::GetResMgr()->get<ImageSet>("Creatures");


	Engine::GetApp().setView(Camera);


	if (TileImgSet->getTexture())
		Engine::GetApp().draw( Tiles, TileImgSet->getTexture().get());
	
	if (RenderGrid)
		Engine::GetApp().draw( Grid );

	if (CreatureImgSet->getTexture())
		Engine::GetApp().draw( Creatures , CreatureImgSet->getTexture().get());

	Engine::GetApp().setView( Engine::GetApp().getDefaultView());
}

void SimView::ReadTileRenderList(TileRenderList& r)
{

	Tiles.clear();
	Tiles.resize( 4 * r.size() );
	Tiles.setPrimitiveType( sf::PrimitiveType::Quads );

	std::shared_ptr<ImageSet> ImgSet = Engine::GetResMgr()->get<ImageSet>("Tiles");
	ImgSet->updateTexture();

	if (!ImgSet->getTexture())
	{
		Engine::out() << "[SimView] Unable to get texture for tile rendering!" << std::endl;
		return;
	}

	int i = 0;

	for ( std::shared_ptr<Tile> T : r)
	{
		ImgSet->CreateQuad( DetermineTileSpriteIndex( T ) , Tiles, DetermineTilePos( T ) , (i++ * 4));
	}

	Engine::out() << "[SimView] Recreated tiles vertexarray!" << std::endl;

	// and create the corresponding grid
	CreateGrid( std::sqrt( r.size() ) );
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
		Engine::out() << "[SimView] Unable to get texture for creature rendering!" << std::endl;
		return;
	}

	int i = 0;

	for ( std::shared_ptr<Creature> T : r)
	{
		ImgSet->CreateQuad( DetermineCreatureSpriteIndex( T ), Creatures, DetermineCreaturePos(T), (i++ * 4) );
	}

	//Engine::out() << "[SimView] Recreated creature vertexarray!" << std::endl;
}

void SimView::CreateGrid( int TerrainSize )
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

sf::Vector2f SimView::CalculateRequisition()
{
}


// ### TERRAIN/TILE STUFF ##

sf::FloatRect SimView::DetermineTilePos( std::shared_ptr<Tile>& t)
{
	sf::FloatRect re;

	re.left 	= TileSize * t->getPosition().x();
	re.top 		= TileSize * t->getPosition().y();
	re.width 	= TileSize;
	re.height 	= TileSize;

	return re;
}

sf::FloatRect SimView::DetermineCreaturePos( std::shared_ptr<Creature>& c)
{
	sf::FloatRect re;
	
	const int CreatureSize = 16;

	re.left 	= TileSize * c->getPosition().x() - CreatureSize/2;
	re.top 		= TileSize * c->getPosition().y() - CreatureSize/2;
	re.width 	= CreatureSize;
	re.height 	= CreatureSize;

	return re;
}

int SimView::DetermineTileSpriteIndex ( std::shared_ptr<Tile>& t)
{
	/// FIXME: hard coded max height only fits for debug terrain!
	float heightpercentage = t->getHeight() / 1500;

	if ( heightpercentage < .01) return 0;
	if ( heightpercentage < .05) return 1;
	if ( heightpercentage < .9) return 2;
	return 3;
}

int SimView::DetermineCreatureSpriteIndex ( std::shared_ptr<Creature>& t)
{
	/// FIXME: hard coded
	return 2;
}

#include "SimView.hpp"

#include "simulator/Simulator.hpp"
#include "simulator/Terrain.hpp"
#include "simulator/Creature.hpp"
#include "simulator/Tile.hpp"

#include "sbe/ResourceManager.hpp"
#include "sbe/ImageSet.hpp"

#include <SFML/Window/Keyboard.hpp>

SimView::SimView()
: Name("SimView"),
 TileSize( 32 )
{
	RegisterForEvent("UpdateCreatureRenderList");
	RegisterForEvent("UpdateTileRenderList");
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
	std::shared_ptr<ImageSet> ImgSet = Engine::GetResMgr()->get<ImageSet>("Tiles");


	Engine::GetApp().setView(Camera);

	if (!ImgSet->getTexture()) return;

	Engine::GetApp().draw( Tiles, ImgSet->getTexture().get());
	Engine::GetApp().draw( Creatures , ImgSet->getTexture().get());

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
		//Engine::out() << "[SimView] Unable to get texture for creature rendering!" << std::endl;
		return;
	}

	int i = 0;

	for ( std::shared_ptr<Creature> T : r)
	{
		ImgSet->CreateQuad( DetermineCreatureSpriteIndex( T ), Creatures, DetermineCreaturePos(T), (i++ * 4) );
	}

	//Engine::out() << "[SimView] Recreated creature vertexarray!" << std::endl;
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
	
	const inst CreatureSize = 16;

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

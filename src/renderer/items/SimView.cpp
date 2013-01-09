#include "SimView.hpp"

#include "simulator/Simulator.hpp"
#include "simulator/Terrain.hpp"
#include "simulator/Creature.hpp"
#include "simulator/Tile.hpp"

#include "sbe/ResourceManager.hpp"
#include "sbe/ImageSet.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>

#include <cmath>

SimView::SimView()
: Name("SimView"),
 TileSize( 128 ),
 RenderGrid ( true ),
 Scrolling( false )
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
	if (e.type == sf::Event::EventType::KeyPressed)
	{

		int delta = 10;

		if (e.key.shift) delta *= 10;

		switch (e.key.code)
		{
			case sf::Keyboard::Key::Up:
				TargetCenter.y += -delta;
				//Camera.move(0, -delta);
				break;

			case sf::Keyboard::Key::Down:
				TargetCenter.y += delta;
				//Camera.move(0, delta);
				break;

			case sf::Keyboard::Key::Left:
				TargetCenter.x += -delta;
				//Camera.move(-delta, 0);
				break;

			case sf::Keyboard::Key::Right:
				TargetCenter.x += delta;
				//Camera.move(delta, 0);
				break;



			case sf::Keyboard::Key::PageUp:
				TargetSize *= 1.1f;
				//Camera.zoom(1.1);
				break;
			case sf::Keyboard::Key::PageDown:
				TargetSize *= 0.9f;
				//Camera.zoom(0.9);
				break;
			default:
				break;
		}
	}
	else if (e.type == sf::Event::EventType::MouseWheelMoved)
	{
		const float WheelZoomFactor = .2f;
		
		for (int i = 0; i < std::abs(e.mouseWheel.delta); ++i)
		{
			TargetSize *= (e.mouseWheel.delta < 0) ? 1 + WheelZoomFactor : 1 - WheelZoomFactor;
		}
	}
	else if (e.type == sf::Event::EventType::MouseMoved)
	{
		const float ScrollFactor = 2.0f;

		if (Scrolling)
		{
			//Camera.move( (e.mouseMove.x - lastMousePos.x)*ScrollFactor , (e.mouseMove.y - lastMousePos.y)*ScrollFactor ); 
			//TargetCenter = Camera.getCenter(); 
			
			TargetCenter.x += (lastMousePos.x - e.mouseMove.x )*ScrollFactor;
			TargetCenter.y += (lastMousePos.y - e.mouseMove.y )*ScrollFactor; 
		}

		lastMousePos.x = e.mouseMove.x;
		lastMousePos.y = e.mouseMove.y;
	}
	else if (e.type == sf::Event::EventType::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Middle )
	{
		Scrolling = true;
	}
	else if (e.type == sf::Event::EventType::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Middle )
	{
		Scrolling = false;
	}
	else if (e.type == sf::Event::EventType::MouseLeft)
	{
		Scrolling = false;
	}
	else if (e.type == sf::Event::EventType::Resized)
	{
		TargetSize = sf::Vector2f( e.size.width, e.size.height );
		//Camera.setSize(e.size.width, e.size.height);
	}
}

void SimView::SetupCamera()
{
	Camera.setSize(800, 600);
	Camera.setCenter(400,300);

	TargetSize = sf::Vector2f(800,600);
	TargetCenter = sf::Vector2f(400,300);

	//Camera.move(0,500);
}

void SimView::UpdateCamera()
{
	const float ZoomFactor = 0.1f;
	// minimum difference between Target and current position to be smoothed
	const int minDiff = 2;
	sf::Vector2f CurrentSize = Camera.getSize();
	sf::Vector2f CurrentCenter = Camera.getCenter();

	float tmp;
	sf::Vector2f Target;

	if ( TargetSize != CurrentSize )
	{
		//Engine::out() << "Size: " << CurrentSize.x << "/" << CurrentSize.y << std::endl;
		//Engine::out() << "Target: " << TargetSize.x << "/" << TargetSize.y << std::endl;
		
		Target.x = CurrentSize.x + (TargetSize.x - CurrentSize.x)*ZoomFactor;
		Target.y = CurrentSize.y + (TargetSize.y - CurrentSize.y)*ZoomFactor;
		if ( std::abs(CurrentSize.x - TargetSize.x) < minDiff ) Target.x = TargetSize.x;
		if ( std::abs(CurrentSize.y - TargetSize.y) < minDiff ) Target.y = TargetSize.y;
		
		Camera.setSize( Target );
	}

	if ( TargetCenter != CurrentCenter )
	{
		Target.x = CurrentCenter.x + (TargetCenter.x - CurrentCenter.x)*ZoomFactor;
		Target.y = CurrentCenter.y + (TargetCenter.y - CurrentCenter.y)*ZoomFactor;
		if ( std::abs(CurrentCenter.x - TargetCenter.x) < minDiff ) Target.x = TargetCenter.x;
		if ( std::abs(CurrentCenter.y - TargetCenter.y) < minDiff ) Target.y = TargetCenter.y;

		Camera.setCenter( Target );
	}

}

void SimView::Render()
{
	std::shared_ptr<ImageSet> TileImgSet = Engine::GetResMgr()->get<ImageSet>("Tiles");
	std::shared_ptr<ImageSet> CreatureImgSet = Engine::GetResMgr()->get<ImageSet>("Creatures");


	UpdateCamera();

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

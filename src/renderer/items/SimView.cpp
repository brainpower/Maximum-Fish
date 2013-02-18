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

#include <boost/format.hpp>

#include <cmath>

using boost::format;
using boost::io::group;

SimView::SimView()
:
 Scrolling( false ),
 Name("SimView")
{

	TileSize = 	Engine::getCfg()->get<int>("ui.simView.tileSize");
	TerrainSize = 	Engine::getCfg()->get<int>("sim.terragen.debug.size");
	RenderGrid = Engine::getCfg()->get<bool>("ui.simView.renderGrid");
	CreatureSize = Engine::getCfg()->get<int>("ui.simView.creatureSize");
	ZoomFactor = Engine::getCfg()->get<float>("ui.simView.zoomFactor");

	ScrollFactor = Engine::getCfg()->get<float>("ui.simView.scrollFactor");
	delta = Engine::getCfg()->get<float>("ui.simView.delta");
	WheelZoomFactor = Engine::getCfg()->get<float>("ui.simView.wheelZoomFactor");

	RegisterForEvent("UpdateCreatureRenderList");
	RegisterForEvent("UpdateTileRenderList");
	RegisterForEvent("WINDOW_RESIZE");

	GridColor = sf::Color( Engine::getCfg()->get<int>("ui.simView.gridColor.r"),
						Engine::getCfg()->get<int>("ui.simView.gridColor.g"),
						Engine::getCfg()->get<int>("ui.simView.gridColor.b") );
	SetupCamera();
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
	} else if(e.Is("WINDOW_RESIZE")){
		TargetSize = sf::Vector2f( Engine::GetApp().getSize().x, Engine::GetApp().getSize().y);
	}
}

void SimView::HandleSfmlEvent ( const sf::Event& e)
{

	switch (e.type)
	{
		case sf::Event::KeyPressed:



			if (e.key.shift) delta *= 10;

			switch (e.key.code)
			{
				case sf::Keyboard::Key::Up:
					TargetCenter.y += -delta;
					break;

				case sf::Keyboard::Key::Down:
					TargetCenter.y += delta;
					break;

				case sf::Keyboard::Key::Left:
					TargetCenter.x += -delta;
					break;

				case sf::Keyboard::Key::Right:
					TargetCenter.x += delta;
					break;

				case sf::Keyboard::Key::PageUp:
					TargetSize *= 1.1f;
					break;

				case sf::Keyboard::Key::PageDown:
					TargetSize *= 0.9f;
					break;

				default:
					break;
			}
			break;

		case sf::Event::MouseWheelMoved:


			for (int i = 0; i < std::abs(e.mouseWheel.delta); ++i)
			{
				TargetSize *= (e.mouseWheel.delta < 0) ? 1 + WheelZoomFactor : 1 - WheelZoomFactor;
			}
			break;

		case sf::Event::MouseMoved:
		{
			sf::Vector2i mouseMove(e.mouseMove.x, e.mouseMove.y);

			sf::Vector2f glLastMousePos = Engine::GetApp().mapPixelToCoords( lastMousePos, Camera);
			sf::Vector2f glMouseMove = Engine::GetApp().mapPixelToCoords( mouseMove, Camera);

			if (Scrolling)
			{
				TargetCenter += (glLastMousePos - glMouseMove )*ScrollFactor;
			}

			lastMousePos.x = e.mouseMove.x;
			lastMousePos.y = e.mouseMove.y;
			}
			break;

		case sf::Event::MouseButtonPressed:
			if (e.mouseButton.button == sf::Mouse::Middle) Scrolling = true;

			if (e.mouseButton.button == sf::Mouse::Left)
			{
				// Translate the click position to Terrain Coordinates (float)
				sf::Vector2i ClickPos( e.mouseButton.x, e.mouseButton.y );

				sf::Vector2f RealPos = Engine::GetApp().mapPixelToCoords( ClickPos, Camera);

				RealPos = RealPos / (float)TileSize;

				Event e("TERRAIN_CLICKED");
				e.SetData( Geom::Pointf( RealPos.x, RealPos.y ) );

				Module::Get()->QueueEvent( e, true);
			}

			break;

		case sf::Event::MouseButtonReleased:
			if (e.mouseButton.button == sf::Mouse::Middle) Scrolling = false;
			break;

		case sf::Event::Resized:

			//Camera.setSize(e.size.width, e.size.height);
			break;

		default:

			break;
	}


}

void SimView::SetupCamera()
{
	Camera.setSize(Engine::getCfg()->get<int>("renderer.windowsize.x"),
					Engine::getCfg()->get<int>("renderer.windowsize.y"));

	Camera.setCenter(Engine::getCfg()->get<int>("renderer.windowsize.x")/2,
					Engine::getCfg()->get<int>("renderer.windowsize.y")/2);

	TargetSize = sf::Vector2f(Engine::getCfg()->get<int>("renderer.windowsize.x"),
							Engine::getCfg()->get<int>("renderer.windowsize.y"));

	TargetCenter = sf::Vector2f(Engine::getCfg()->get<int>("renderer.windowsize.x")/2,
								Engine::getCfg()->get<int>("renderer.windowsize.y")/2);


	//Camera.move(0,500);
}

void SimView::UpdateCamera()
{
	// minimum difference between Target and current position to be smoothed
	const int minDiff = 2;
	sf::Vector2f CurrentSize = Camera.getSize();
	sf::Vector2f CurrentCenter = Camera.getCenter();

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

	PostDebugInfo();
}

void SimView::PostDebugInfo()
{
	Module::Get()->DebugString("View Size",  str(format("%.0f x %.0f") % TargetSize.x % TargetSize.y));
	Module::Get()->DebugString("View Pos", str(format("%.0f x %.0f") % TargetCenter.x % TargetCenter.y));
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

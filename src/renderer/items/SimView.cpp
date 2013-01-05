#include "SimView.hpp"


#include "simulator/Creature.hpp"
#include "simulator/Tile.hpp"

void SimView::HandleEvent(Event& e)
{
}

void SimView::Render()
{

}

void SimView::ReadTileRenderList(TileRenderList& r)
{
	Tiles.resize( 4 * r.size() );

	auto ImgSet = Engine::GetResMgr()->Get<ImageSet>("Tiles.png");

	int i = 0;

	for ( std::shared_ptr<Tile> T : r)
	{
		// defines which frame in the imageset should be used to render the creature
		// we don't have a tileset to render stuff yet, so the actual indices are yet to be defined
		int frame = 0;

		ImgSet->CreateQuad( frame , Tiles, , i++);
	}

}


void SimView::ReadCreatureRenderList(CreatureRenderList& r)
{
}

sf::Vector2f SimView::CalculateRequisition()
{
}

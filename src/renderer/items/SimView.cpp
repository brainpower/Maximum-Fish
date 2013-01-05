#include "SimView.hpp"


#include "simulator/Creature.hpp"
#include "simulator/Tile.hpp"

void SimView::HandleEvent(Event& e)
{
	if(e.Is("UpdateCreatureRenderList"))
	{
		Engine::out() << "Creatures updatet" << std::endl;
	}
}

void SimView::Render()
{

}

void SimView::ReadTileRenderList(TileRenderList& r)
{

}

void SimView::ReadCreatureRenderList(CreatureRenderList& r)
{
}

sf::Vector2f SimView::CalculateRequisition()
{
}

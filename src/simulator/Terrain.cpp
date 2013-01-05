#include "Terrain.hpp"
#include "sbe/event/Event.hpp"
#include "sbe/Module.hpp"

Tile* Terrain::getTile( Geom::Vec2f pos )
{
	return nullptr//&(Tiles[ (int)(pos.x()) ][ (int)(pos.y()) ])
	;
}

int Terrain::getTileElevation(Geom::Vec2f pos)
{
	return 0//Tiles[(int)(pos.x())][(int)(pos.y())].getElevation()
	;
}

int Terrain::getMaxElevation()
{
	return max_elevation;
}

float Terrain::getGlobalTemp()
{
	return global_temp;
}

void Terrain::update_terrain()
{
	//make a freakin Event, man
	Event e("UpdateTileRenderList");
	e.SetData( m_tiles );
	Module::Get()->QueueEvent(e, true);
}

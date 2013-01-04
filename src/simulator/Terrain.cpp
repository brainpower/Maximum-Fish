#include "Terrain.hpp"

Tile* Terrain::getTile( Geom::Vec2f pos )
{
	return &(Tiles[ (int)(pos.x()) ][ (int)(pos.y()) ]);
}

int Terrain::getTileElevation(Geom::Vec2f pos)
{
	return Tiles[(int)(pos.x())][(int)(pos.y())].getElevation();
}

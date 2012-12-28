#include "Terrain.hpp"

Tile* Terrain::getTile( Geom::Vec2f pos )
{
	return &(Tiles[ (int)(pos.x()) ][ (int)(pos.y()) ]);
}

int Terrain::getTileAltitude(Geom::Vec2f pos)
{
	return Tiles[(int)(pos.x())][(int)(pos.y())].getAltitude();
}

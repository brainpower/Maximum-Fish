#include "Terrain.hpp"

int Terrain::getTileAltitude(Geom::Vec2f pos)
{
	return Tiles[(int)(pos.x())][(int)(pos.y())].getAltitude();
}
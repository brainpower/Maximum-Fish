#ifndef TERRAINIOPLUGIN_HPP
#define TERRAINIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Terrain.hpp"

class TerrainIOPlugin : iTreeIOPlugin<Terrain> {

	TerrainIOPlugin(){}

	virtual ~TerrainIOPlugin(){}

	ObjectList loadObjects(boost::ptree::node &root) override;
	bool saveObject( const Terrain &o, boost::ptree::node &root) override;
};

#endif // TERRAINIOPLUGIN_HPP


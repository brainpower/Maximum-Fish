#ifndef TERRAINIOPLUGIN_HPP
#define TERRAINIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Terrain.hpp"

class TerrainIOPlugin : iTreeIOPlugin<Terrain> {

	TerrainIOPlugin(){}

	virtual ~TerrainIOPlugin(){}

	ObjectList loadObjects(boost::property_tree::ptree &root) override;
	bool saveObject( const Terrain &o, boost::property_tree::ptree &root) override;
};

#endif // TERRAINIOPLUGIN_HPP


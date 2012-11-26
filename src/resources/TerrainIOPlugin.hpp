#ifndef TERRAINIOPLUGIN_HPP
#define TERRAINIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Terrain.hpp"

class TerrainIOPlugin : iTreeIOPlugin<Terrain> {

	TerrainIOPlugin(){}

	virtual ~TerrainIOPlugin(){}

#ifndef __GCC_4_6__ // gcc < 4.7 doesn't support override
	ObjectList loadObjects(boost::property_tree::ptree &root) override;
	bool saveObject( const Terrain &o, boost::property_tree::ptree &root) override;
#else
	ObjectList loadObjects(boost::property_tree::ptree &root);
	bool saveObject( const Terrain &o, boost::property_tree::ptree &root);
#endif
};

#endif // TERRAINIOPLUGIN_HPP


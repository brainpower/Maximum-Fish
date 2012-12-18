#ifndef TERRAINIOPLUGIN_HPP
#define TERRAINIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Terrain.hpp"

class TerrainIOPlugin : iTreeIOPlugin<Terrain> {

	TerrainIOPlugin()
	 : iTreeIOPlugin( "Terrain.info" )
	{}

	virtual ~TerrainIOPlugin(){}

	virtual ObjPtr loadObjects(boost::property_tree::ptree &root);
	virtual bool saveObject( const Terrain &o, boost::property_tree::ptree &root);

};

#endif // TERRAINIOPLUGIN_HPP

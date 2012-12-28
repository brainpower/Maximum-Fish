#ifndef TERRAINIOPLUGIN_HPP
#define TERRAINIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Terrain.hpp"

class TerrainIOPlugin : public iTreeIOPlugin<Terrain> {

public:
	TerrainIOPlugin()
	 : iTreeIOPlugin( "Terrain.info" )
	{}

	virtual ~TerrainIOPlugin() = default;

	virtual ObjPtr loadObject(const boost::property_tree::ptree::value_type &node);
	virtual bool saveObject( const Terrain &o, boost::property_tree::ptree &root);

};

#endif // TERRAINIOPLUGIN_HPP

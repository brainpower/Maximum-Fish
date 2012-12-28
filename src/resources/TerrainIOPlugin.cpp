#include "TerrainIOPlugin.hpp"

TerrainIOPlugin::ObjPtr TerrainIOPlugin::loadObjects(const boost::property_tree::ptree::value_type &node){
	ObjPtr re;
	if (node.first != "Terrain") return re;

	try
	{
		const ptree& pt = node.second;

		re.reset( new Terrain() );

		re->setHumidityFactor( pt.get<float>("humidityFactor") );

		// for y in tiles.x:
		//   for t in tiles.x.y:
		//		 tile->set(t.altitude)
		//		 tile->set(t.nutrition)
		//		 tile->set(t.baseHumidity)
		//		 tile->set(t.Humidity)
		//		 tile->set(t.Biomass)
		//     re->addTile(x,y, tile);


	} catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[TerrainIOPlugin] Error loading Terrain from ptree!";
		re.reset();
		return re;
	}

	return re;
}

bool TerrainIOPlugin::saveObject(const Terrain &t, boost::property_tree::ptree &r){
	try
	{
		ptree pt;

		//pt.put<float>("humidityFactor", t.getHumidityFactor());

		// for vec<tile> in tiles:
		//   for tile in vec<tile>:
		//     pt.put<int>("tiles.x.y.altitude", tile.altitude);
		//     pt.put<int>("tiles.x.y.nutrition", tile.altitude);
		//     pt.put<int>("tiles.x.y.baseHumidity", tile.baseHumidity);
		//     pt.put<int>("tiles.x.y.Humidity", tile.Humidity);
		//     pt.put<int>("tiles.x.y.Biomass", tile.Biomass);

		r.add_child( "Species", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[SpeciesIOPlugin] Error saving Species to ptree!";
		return false;
	}
}

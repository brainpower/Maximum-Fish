#include "TerrainIOPlugin.hpp"
#include "sbe/Engine.hpp"

using boost::property_tree::ptree;

TerrainIOPlugin::ObjPtr TerrainIOPlugin::loadObject(const boost::property_tree::ptree::value_type &node)
{
    ObjPtr re;
    if (node.first != "Terrain") return re;

    try
    {
        const ptree& pt = node.second;

        re.reset( new Terrain() );

        re->setHumidityFactor( pt.get<float>("humidityFactor") );

        // for y in tiles.x:
        //   for t in tiles.x.y:
        //		 tile->set(t.elevation)
        //		 tile->set(t.nutrition)
        //		 tile->set(t.baseHumidity)
        //		 tile->set(t.Humidity)
        //		 tile->set(t.Biomass)
        //     re->addTile(x,y, tile);


    }
    catch ( boost::property_tree::ptree_error )
    {
        Engine::out() << "[TerrainIOPlugin] Error loading Terrain from ptree!";
        re.reset();
        return re;
    }

    return re;
}

bool TerrainIOPlugin::saveObject( const std::string& name,const Terrain &t, boost::property_tree::ptree &r)
{
    try
    {
        ptree pt;

        pt.put<std::string>("Name", name);

        pt.put<float>("humidityFactor", t.getHumidityFactor());

        pt.put<int>("size.x", t.Size.x());
        pt.put<int>("size.y", t.Size.y());

        for (auto tile : t.Tiles)
        {
            pt.put<int>("tiles.x.y.pos.x", tile->Position.x());
            pt.put<int>("tiles.x.y.pos.y", tile->Position.y());
            pt.put<int>("tiles.x.y.heigth", tile->height);
            pt.put<int>("tiles.x.y.nutrition", tile->nutrition);
            pt.put<int>("tiles.x.y.baseHumidity", tile->baseHumidity);
            pt.put<int>("tiles.x.y.Humidity", tile->humidity);
            pt.put<int>("tiles.x.y.Biomass", tile->biomass);
        }


        r.add_child( "Terrain", pt);
        return true;
    }
    catch ( boost::property_tree::ptree_error )
    {
        Engine::out() << "[SpeciesIOPlugin] Error saving Terrain to ptree!";
        return false;
    }
}

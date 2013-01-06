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
        re->Size.SetX( pt.get<int>("size.x") );
        re->Size.SetY( pt.get<int>("size.y") );

				for ( const boost::property_tree::ptree::value_type& e : pt){
					if(e.first == "Tile"){
						auto pt_t = e.second;

						std::shared_ptr<Tile> t_ptr(new Tile(
						    Geom::Vec2(pt_t.get<int>("pos.x"), pt_t.get<int>("pos.y")),
						    pt_t.get<float>("height"),
						    pt_t.get<float>("nutrition"),
						    pt_t.get<float>("baseHumidity")));

						t_ptr->humidity     = pt_t.get<float>("humidity");
						t_ptr->biomass      = pt_t.get<int>  ("biomass");

						re->Tiles.push_back(t_ptr);
					}
				}

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
        	ptree pt_t;

            pt_t.put<int>("pos.x", tile->Position.x());
            pt_t.put<int>("pos.y", tile->Position.y());
            pt_t.put<float>("height", tile->height);
            pt_t.put<float>("nutrition", tile->nutrition);
            pt_t.put<float>("baseHumidity", tile->baseHumidity);
            pt_t.put<float>("humidity", tile->humidity);
            pt_t.put<int>("biomass", tile->biomass);

            pt.add_child( "Tile", pt_t);
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

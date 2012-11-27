#include "CreatureIOPlugin.hpp"
#include <sstream>

using boost::property_tree::ptree;

CreatureIOPlugin::ObjectList CreatureIOPlugin::loadObjects(ptree &r){
	int i = r.get<int>("creatures.count");
	stringstream ss;
	ObjectList ol;

	while(i){
		// better use get_child here instead of a counter,
		// see: http://www.boost.org/doc/libs/1_52_0/doc/html/boost/property_tree/basic_ptree.html#id1249113-bb
		// and: http://www.boost.org/doc/libs/1_52_0/doc/html/boost_propertytree/tutorial.html
		auto c = std::shared_ptr<Creature>(new Creature());
		c->setCurrentHealth(get<int>(r, i, "currentHealth"));
		c->setAge(get<int>(r, i, "age"));
		c->setPosition(get<float>(r, i, "posX"), get<float>(r, i, "posY"));
		//c->setSpecies(get<string>(r, i, "species")); // how to save? or restore?
		//c->setCurrentTile(get<well, ...>(r, i, "tile")); // or is position enough?
		ol.push_back(c);
		--i;
	}
	return ol;
}

template<typename T>
T CreatureIOPlugin::get(ptree &r, const int cnum, const string &key){
	stringstream ss;
	ss << "creatures." << cnum << "." << key;
	return r.get<T>(ss.str());
}

template<typename T>
void CreatureIOPlugin::put(ptree &r, const int cnum, const string &key, const T &value){
	stringstream ss;
	ss << "creatures." << cnum << "." << key;
	r.put<T>(ss.str(), value);
}

bool CreatureIOPlugin::saveObject(const Creature &c, boost::property_tree::ptree &r){
	int i = r.get<int>("creatures.count") +1;

	put<int>(r, i, "currentHealth", c.getCurrentHealth());
	put<int>(r, i, "age", c.getAge());
	put<float>(r, i, "posX", c.getPosition().x());
	put<float>(r, i, "posY", c.getPosition().y());
	//save species
	//save tile?

	r.put<int>("creatures.count", i);
	return true;
}

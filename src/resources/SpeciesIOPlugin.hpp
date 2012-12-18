#ifndef SPECIESIOPLUGIN_HPP
#define SPECIESIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Species.hpp"


class SpeciesIOPlugin : public iTreeIOPlugin<Species> {
	SpeciesIOPlugin()
	 : iTreeIOPlugin( "Species.info" )
	{}

	virtual ~SpeciesIOPlugin(){}

	virtual ObjPtr loadObjects(boost::property_tree::ptree &root);
	virtual bool saveObject( const Species &o, boost::property_tree::ptree &root);

};


#endif // SPECIESIOPLUGIN_HPP

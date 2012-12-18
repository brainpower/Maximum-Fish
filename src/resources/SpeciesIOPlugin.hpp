#ifndef SPECIESIOPLUGIN_HPP
#define SPECIESIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Species.hpp"


class SpeciesIOPlugin : public iTreeIOPlugin<Species> {
	SpeciesIOPlugin(){}

	virtual ~SpeciesIOPlugin(){}

#ifndef __GCC_4_6__ // gcc < 4.7 doesn't support override
	ObjPtr loadObjects(boost::property_tree::ptree &root) override;
	bool saveObject( const Species &o, boost::property_tree::ptree &root) override;
#else
	ObjPtr loadObjects(boost::property_tree::ptree &root);
	bool saveObject( const Species &o, boost::property_tree::ptree &root);
#endif
};


#endif // SPECIESIOPLUGIN_HPP

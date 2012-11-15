#ifndef SPECIESIOPLUGIN_HPP
#define SPECIESIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Species.hpp"


class SpeciesIOPlugin : public iTreeIOPlugin<Species> {
	SpeciesIOPlugin(){}

	virtual ~SpeciesIOPlugin(){}

	ObjectList loadObjects(boost::ptree::node &root) override;
	bool saveObject( const Species &o, boost::ptree::node &root) override;
};


#endif // SPECIESIOPLUGIN_HPP

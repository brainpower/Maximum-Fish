#ifndef SPECIESIOPLUGIN_HPP
#define SPECIESIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "simulator/world/Species.hpp"

/// iTreeIOPlugin for Species
class SpeciesIOPlugin : public sbe::iTreeIOPlugin<Species> {
public:
	SpeciesIOPlugin()
	 : iTreeIOPlugin( "Species.info" )
	{}

	virtual ~SpeciesIOPlugin();
	virtual ObjPtr loadObject(const boost::property_tree::ptree::value_type &node);
	virtual bool saveObject( const std::string& name, const Species &o, boost::property_tree::ptree &root);

};


#endif // SPECIESIOPLUGIN_HPP

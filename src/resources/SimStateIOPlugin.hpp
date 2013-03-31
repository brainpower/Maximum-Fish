#ifndef SIMSTATEIOPLUGIN_HPP
#define SIMSTATEIOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "simulator/SimState.hpp"

/// iTreeIOPlugin for SimState
class SimStateIOPlugin : public sbe::iTreeIOPlugin<SimState> {

public:
	SimStateIOPlugin()
	 : iTreeIOPlugin( "SimState.info" )
	{}

	virtual ~SimStateIOPlugin();

	virtual ObjPtr loadObject(const boost::property_tree::ptree::value_type &node);
	virtual bool saveObject( const std::string& name, const SimState &s, boost::property_tree::ptree &root);

};

#endif // SIMSTATEIOPLUGIN_HPP

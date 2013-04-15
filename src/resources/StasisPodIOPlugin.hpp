#ifndef STASISPODIOPLUGIN_HPP
#define STASISPODOPLUGIN_HPP

#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "simulator/StasisPod.hpp"

/// iTreeIOPlugin for StasisPod
class StasisPodIOPlugin : public sbe::iTreeIOPlugin<StasisPod> {

public:
	StasisPodIOPlugin()
	 : iTreeIOPlugin( "StasisPod.info" )
	{}

	virtual ~StasisPodIOPlugin();

	virtual ObjPtr loadObject(const boost::property_tree::ptree::value_type &node);
	virtual bool saveObject( const std::string& name, const StasisPod &s, boost::property_tree::ptree &root);

};

#endif // STASISPODIOPLUGIN_HPP

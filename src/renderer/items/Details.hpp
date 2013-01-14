#ifndef DETAILS_H
#define DETAILS_H

#include <SFGUI/SFGUI.hpp>

class Details
{
	public:

		virtual ~Details() {}
		virtual sfg::Box::Ptr Get() = 0;
	protected:
		sfg::Box::Ptr DetailsBox;
};

#endif // DETAILS_H


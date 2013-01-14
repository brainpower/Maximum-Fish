#ifndef DETAILS_EMPTY_H
#define DETAILS_EMPTY_H

#include "renderer/items/Details.hpp"

#include <SFGUI/SFGUI.hpp>

class DetailsEmpty : public Details
{
	public:

		DetailsEmpty();
		virtual ~DetailsEmpty() {}
		virtual void Update() {}
        virtual sfg::Box::Ptr Get();
    protected:
        sfg::Box::Ptr DetailsBox;
};

#endif // DETAILS_EMPTY_H


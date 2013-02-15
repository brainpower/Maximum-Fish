#ifndef DETAILS_EMPTY_H
#define DETAILS_EMPTY_H

#include "renderer/items/Details.hpp"

#include <SFGUI/SFGUI.hpp>

class DetailsEmpty : public Details
{
	public:

		DetailsEmpty();
		virtual ~DetailsEmpty() {}
		virtual void UpdateLabel() {}
        virtual sfg::Widget::Ptr Get();
};

#endif // DETAILS_EMPTY_H


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
		virtual void UpdateBox() {}
        virtual sfg::Widget::Ptr GetLabel();
        virtual sfg::Widget::Ptr GetBox();
};

#endif // DETAILS_EMPTY_H


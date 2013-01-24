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
        virtual sfg::Widget::Ptr Get();
    protected:
		sfg::Label::Ptr DetailsLabel;
};

#endif // DETAILS_EMPTY_H


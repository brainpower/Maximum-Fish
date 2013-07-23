#ifndef DETAILS_EMPTY_H
#define DETAILS_EMPTY_H

#include "renderer/ui/items/Details.hpp"


/// Empty Details window
class DetailsEmpty : public Details
{
	public:

		DetailsEmpty();
		virtual ~DetailsEmpty() {}
		virtual void UpdateLabel() {}
		virtual void UpdateBox() {}
        virtual sfg::SharedPtr<sfg::Widget> GetLabel();
        virtual sfg::SharedPtr<sfg::Widget> GetBox();
};

#endif // DETAILS_EMPTY_H


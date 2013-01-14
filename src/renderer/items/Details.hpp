#ifndef DETAILS_H
#define DETAILS_H

#include <SFGUI/SFGUI.hpp>

class Details
{
	public:

		virtual ~Details() {}
		virtual sfg::Box::Ptr Get() = 0;

		/// update the contents of the box returned by Get
		virtual void Update() = 0;

		void AddLabel( sfg::Box::Ptr Box, const std::string& key, const std::string& value )
		{
			sfg::Label::Ptr L = sfg::Label::Create( key +": "+ value );
			L->SetAlignment( sf::Vector2f(0.f, 0.f) );
			Box->Pack( L );
		}

	protected:
		sfg::Box::Ptr DetailsBox;
};

#endif // DETAILS_H


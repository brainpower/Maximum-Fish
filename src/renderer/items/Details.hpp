#ifndef DETAILS_H
#define DETAILS_H

#include "sbe/event/EventUser.hpp"

#include <SFGUI/SFGUI.hpp>

/// Base class shows specific Detail for one Simulation element
class Details : public EventUser
{
	public:
		Details();
		virtual ~Details() {}
		virtual sfg::Widget::Ptr GetLabel() = 0;
		virtual sfg::Widget::Ptr GetBox() = 0;

		/// update the contents of the box returned by Get
		virtual void UpdateLabel() = 0;
		virtual void UpdateBox() = 0;

		virtual void HandleEvent(Event& e);
		void AddLabel( sfg::Box::Ptr Box, const std::string& key, const std::string& value );
		void AddToLabel( sfg::Label::Ptr Label, const std::string& key, const std::string& value );

    protected:
		sfg::Box::Ptr DetailsBox;
		sfg::Label::Ptr DetailsLabel;

	private:
		sf::Clock UpdateTimer;
};

#endif // DETAILS_H


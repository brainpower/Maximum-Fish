#ifndef DETAILS_H
#define DETAILS_H

#include "sbe/event/EventUser.hpp"

#include <SFGUI/SharedPtr.hpp>
namespace sfg
{
	class Widget;
	class Box;
	class Label;
}

/// Base class shows specific Detail for one Simulation element
class Details : public sbe::EventUser
{
	public:
		Details();
		virtual ~Details() {}
		virtual sfg::SharedPtr<sfg::Widget> GetLabel() = 0;
		virtual sfg::SharedPtr<sfg::Widget> GetBox() = 0;

		/// update the contents of the box returned by Get
		virtual void UpdateLabel() = 0;
		virtual void UpdateBox() = 0;

		virtual void HandleEvent(Event& e);
		void AddLabel( sfg::SharedPtr<sfg::Box> Box, const std::string& key, const std::string& value );
		void AddToLabel( sfg::SharedPtr<sfg::Label> Label, const std::string& key, const std::string& value );

    protected:
		sfg::SharedPtr<sfg::Box> DetailsBox;
		sfg::SharedPtr<sfg::Label> DetailsLabel;

	private:
		sf::Clock UpdateTimer;
};

#endif // DETAILS_H


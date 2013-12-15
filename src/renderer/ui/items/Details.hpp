#ifndef DETAILS_H
#define DETAILS_H

#include "sbe/event/EventUser.hpp"


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
		virtual std::shared_ptr<sfg::Widget> GetLabel() = 0;
		virtual std::shared_ptr<sfg::Widget> GetBox() = 0;

		/// update the contents of the box returned by Get
		virtual void UpdateLabel() = 0;
		virtual void UpdateBox() = 0;

		virtual void HandleEvent(Event& e);
		void AddLabel( std::shared_ptr<sfg::Box> Box, const std::string& key, const std::string& value );
		void AddToLabel( std::shared_ptr<sfg::Label> Label, const std::string& key, const std::string& value );

    protected:
		std::shared_ptr<sfg::Box> DetailsBox;
		std::shared_ptr<sfg::Label> DetailsLabel;

	private:
		sf::Clock UpdateTimer;
};

#endif // DETAILS_H


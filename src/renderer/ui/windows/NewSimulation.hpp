#ifndef NEWSIM_HPP
#define NEWSIM_HPP

#include <SFGUI/SharedPtr.hpp>

#include "sbe/event/EventUser.hpp"
#include <SFML/System/Vector2.hpp>

namespace sfg
{
	class Widget;
	class Box;
	class Notebook;
	class Window;
	class Entry;
	class Label;
}

class NewSimWindow : public sbe::EventUser
{
	public:
		NewSimWindow();
		void HandleEvent(Event& e);
		void CreateWindow();
		sfg::SharedPtr<sfg::Widget> CreateGenerationPage();
		sfg::SharedPtr<sfg::Widget> CreateTerrainPage();

	private:

		void okClick();
		void abortClick();

		sfg::SharedPtr<sfg::Entry> entry( const std::string& cfg, sf::Vector2f req = { 60, 5 } );
		sfg::SharedPtr<sfg::Label> lbl( const std::string& text, sf::Vector2f Align = {0.5f, 0.5f} );

		sfg::SharedPtr<sfg::Window> Win;
		sfg::SharedPtr<sfg::Notebook> Tabs;
		// Generation Page
		sfg::SharedPtr<sfg::Entry> Seed;

		sfg::SharedPtr<sfg::Entry> CountMult;
		sfg::SharedPtr<sfg::Entry> CarnivoreSpeciesCount;
		sfg::SharedPtr<sfg::Entry> CarnivoreCount;
		sfg::SharedPtr<sfg::Entry> HerbivoreSpeciesCount;
		sfg::SharedPtr<sfg::Entry> HerbivoreCount;
		sfg::SharedPtr<sfg::Entry> PlantSpeciesCount;
		sfg::SharedPtr<sfg::Entry> PlantCount;

		// Terrain Page
		sfg::SharedPtr<sfg::Entry> MaxHeight;
		sfg::SharedPtr<sfg::Entry> Size;
		sfg::SharedPtr<sfg::Entry> WaterLimit;
		sfg::SharedPtr<sfg::Entry> NutritionMin;
		sfg::SharedPtr<sfg::Entry> NutritionMax;
		sfg::SharedPtr<sfg::Entry> HumidityMin;
		sfg::SharedPtr<sfg::Entry> HumidityMax;
};

#endif // NEWSIM_HPP


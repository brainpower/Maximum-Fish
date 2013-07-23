#ifndef NEWSIM_HPP
#define NEWSIM_HPP

#include <SFGUI/SharedPtr.hpp>

#include "sbe/event/EventUser.hpp"
#include "sbe/sfg/List.hpp"
#include "../../../simulator/Generator.hpp"
#include <SFML/System/Vector2.hpp>

#include "../../../simulator/world/Species.hpp"
#include <boost/lexical_cast.hpp>

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
		sfg::SharedPtr<sfg::Widget> CreateSpeciesPage();

	private:

		void okClick();
		void newSpeciesClick();
		void delSpeciesClick();
		void abortClick();
		void createSpecies();
		void modifySpecies(std::shared_ptr<Species> _spec);
		std::shared_ptr<Species> createSpecies( Species::SPECIES_TYPE type );

		sfg::SharedPtr<sfg::Entry> entry( const std::string& cfg, sf::Vector2f req = { 60, 5 } );
		sfg::SharedPtr<sfg::Entry> entry( std::string _str,int _t, sf::Vector2f req = { 60, 5 } );
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
		sfg::SharedPtr<sfg::Entry> RainAmount;
		sfg::SharedPtr<sfg::Entry> RainFall;

		// Species Page
		sfg::SharedPtr<sfg::Entry> MaxAge;
		sfg::SharedPtr<sfg::Entry> MaxHealth;
		sfg::SharedPtr<sfg::Entry> MaxSpeed;
		sfg::SharedPtr<sfg::Entry> Reach;
		sfg::SharedPtr<sfg::Entry> Resistance;
		sfg::SharedPtr<sfg::Entry> BreedingSpeed;
		sfg::SharedPtr<sfg::Entry> MaxRegen;
		sfg::SharedPtr<sfg::Entry> FoodReq;
		sfg::SharedPtr<sfg::Entry> WaterReq;
		sfg::SharedPtr<sfg::Entry> OptimalTemperature;
		sfg::SharedPtr<sfg::Entry> NewSpecies;
		sfg::SharedPtr<sfg::Entry> SpeciesType;
		sfg::SharedPtr<sfg::Entry> SpeciesCount;

		sbe::sfgList s_list;
		std::vector<std::shared_ptr<Species>> t_species;
		std::vector<int> t_species_count;
};

#endif // NEWSIM_HPP


#ifndef NEWSIM_HPP
#define NEWSIM_HPP



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
	class ComboBox;
}

class NewSimWindow : public sbe::EventUser
{
	public:
		NewSimWindow();
		void HandleEvent(Event& e);
		void CreateWindow();
		std::shared_ptr<sfg::Widget> CreateGenerationPage();
		std::shared_ptr<sfg::Widget> CreateTerrainPage();
		std::shared_ptr<sfg::Widget> CreateSpeciesPage();

	private:

		void okClick();
		void newSpeciesClick();
		void newRandomSpeciesClick();
		void delSpeciesClick();
		void abortClick();
		void createSpecies();
		void modifySpecies(std::shared_ptr<Species> _spec);

		Species::SPECIES_TYPE getCurrentSpeciesType();

		std::shared_ptr<sfg::Entry> entry( const std::string& cfg, sf::Vector2f req = { 60, 5 } );
		std::shared_ptr<sfg::Entry> entry( std::string _str,int _t, sf::Vector2f req = { 60, 5 } );
		std::shared_ptr<sfg::Label> lbl( const std::string& text, sf::Vector2f Align = {0.5f, 0.5f} );

		std::shared_ptr<sfg::Window> Win;
		std::shared_ptr<sfg::Notebook> Tabs;
		// Generation Page
		std::shared_ptr<sfg::Entry> Seed;

		std::shared_ptr<sfg::Entry> CountMult;
		std::shared_ptr<sfg::Entry> CarnivoreSpeciesCount;
		std::shared_ptr<sfg::Entry> CarnivoreCount;
		std::shared_ptr<sfg::Entry> HerbivoreSpeciesCount;
		std::shared_ptr<sfg::Entry> HerbivoreCount;
		std::shared_ptr<sfg::Entry> PlantSpeciesCount;
		std::shared_ptr<sfg::Entry> PlantCount;

		// Terrain Page
		std::shared_ptr<sfg::Entry> MaxHeight;
		std::shared_ptr<sfg::Entry> Size;
		std::shared_ptr<sfg::Entry> WaterLimit;
		std::shared_ptr<sfg::Entry> NutritionMin;
		std::shared_ptr<sfg::Entry> NutritionMax;
		std::shared_ptr<sfg::Entry> HumidityMin;
		std::shared_ptr<sfg::Entry> HumidityMax;
		std::shared_ptr<sfg::Entry> RainAmount;
		std::shared_ptr<sfg::Entry> RainFall;

		// Species Page
		std::shared_ptr<sfg::Entry> MaxAge;
		std::shared_ptr<sfg::Entry> MaxHealth;
		std::shared_ptr<sfg::Entry> MaxSpeed;
		std::shared_ptr<sfg::Entry> Reach;
		std::shared_ptr<sfg::Entry> Resistance;
		std::shared_ptr<sfg::Entry> BreedingSpeed;
		std::shared_ptr<sfg::Entry> MaxRegen;
		std::shared_ptr<sfg::Entry> FoodReq;
		std::shared_ptr<sfg::Entry> WaterReq;
		std::shared_ptr<sfg::Entry> OptimalTemperature;
		std::shared_ptr<sfg::Entry> NewSpecies;
		std::shared_ptr<sfg::Entry> SpeciesType;
		std::shared_ptr<sfg::Entry> CreatureCount;

		std::shared_ptr<sfg::ComboBox>  cmbBox;

		sbe::sfgList s_list;
		std::vector<std::shared_ptr<Species>> t_species;
		std::vector<int> t_species_count;
		int last_selected = -1;

		std::mt19937 rnd;
};

#endif // NEWSIM_HPP


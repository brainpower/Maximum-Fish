#include "NewSimulation.hpp"

#include "sbe/Config.hpp"

#include <SFGUI/Window.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Notebook.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Table.hpp>

using namespace sfg;

NewSimWindow::NewSimWindow()
{
	RegisterForEvent("KEY_SHOW_NEWSIM");
	RegisterForEvent("SPECIES_GEN_CLICKED");
}

void NewSimWindow::HandleEvent(Event& e)
{
	if ( e.Is("KEY_SHOW_NEWSIM") )
	{
		CreateWindow();
		Module::Get()->QueueEvent( Event("SCREEN_ADD_WINDOW", Win) );
	}
	if( e.Is("SPECIES_GEN_CLICKED", typeid( std::string )))
	{
		std::string s =  boost::any_cast<std::string>(e.Data());

		for(int i = 0; i < t_species.size(); i++)
		{
			if(s.compare(t_species[i]->getName()) == 0)
			{
				NewSpecies->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getName()))));
				MaxAge->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getMaxAge()))));
				MaxHealth->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getMaxHealth()))));
				MaxSpeed->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getMaxSpeed()))));
				Reach->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getReach()))));
				Resistance->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getResistance()))));
				BreedingSpeed->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getBreedingSpeed()))));
				MaxRegen->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getMaxRegeneration()))));
				FoodReq->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getFoodRequirement()))));
				WaterReq->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getWaterRequirement()))));
				OptimalTemperature->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species[i]->getOptimalTemperature()))));
				if(t_species[i]->getType() ==  Species::SPECIES_TYPE::HERBA)
				{
					SpeciesType->SetText(*(new sf::String("HERBA")));
				} else if(t_species[i]->getType() ==  Species::SPECIES_TYPE::HERBIVORE) {
					SpeciesType->SetText(*(new sf::String("HERBIVORE")));
				} else {
					SpeciesType->SetText(*(new sf::String("CARNIVORE")));
				}
				SpeciesCount->SetText(*(new sf::String(boost::lexical_cast<std::string>(t_species_count[i]))));
			}
		}

	}
}

void NewSimWindow::CreateWindow()
{
	Win = Window::Create( );
	Box::Ptr mainBox = Box::Create( Box::VERTICAL );
		Notebook::Ptr nB = Notebook::Create();
		nB->AppendPage( CreateGenerationPage(), lbl("Generation") );
		nB->AppendPage( CreateTerrainPage(), lbl("Terrain") );
		nB->AppendPage( CreateSpeciesPage(), lbl("Species") );
	mainBox->Pack( nB, true,true);
		Box::Ptr hBox;
		hBox = Box::Create( Box::HORIZONTAL, 3.0f);
		// add Spacer
		hBox->Pack( lbl("-- MF --"), true, true);
			Button::Ptr abortBtn = Button::Create( "Abort" );
			abortBtn->GetSignal( Button::OnLeftClick ).Connect( &NewSimWindow::abortClick , this );
		hBox->Pack( abortBtn, false, false);
			Button::Ptr okBtn = Button::Create( "Ok/New" );
			okBtn->GetSignal( Button::OnLeftClick ).Connect( &NewSimWindow::okClick , this );
		hBox->Pack( okBtn, false, false );
	mainBox->Pack( hBox, false, false );

	Win->SetRequisition( sf::Vector2f( 300, 500 ) );
	Win->Add(mainBox);

	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width )/2 , ( winSize.y - Allocation.height )/2   ) );
}

SharedPtr<Widget> NewSimWindow::CreateGenerationPage()
{
	CountMult = entry("sim.terragen.count");
	Seed =  entry("sim.defaultSeed");

	CarnivoreSpeciesCount =  entry("sim.terragen.species.carnivore");
	HerbivoreSpeciesCount =  entry("sim.terragen.species.herbivore");
	PlantSpeciesCount =  entry("sim.terragen.species.plant");

	CarnivoreCount =  entry("sim.terragen.count.carnivore");
	HerbivoreCount =  entry("sim.terragen.count.herbivore");
	PlantCount =  entry("sim.terragen.count.plant");

	Table::Ptr main = Table::Create();

	main->Attach( lbl( "Seed" ), 				{{0,0},{1,1}}, Table::EXPAND, 0 );
	main->Attach( Seed, 						{{3,0},{1,1}}, Table::FILL, 0 );
	main->Attach( lbl( "Count Multiplicator" ), {{0,1},{1,1}}, Table::EXPAND, 0 );
	main->Attach( CountMult, 					{{3,1},{1,1}}, Table::FILL, 0 );
	main->Attach( lbl( "Carnivore Species" ), 	{{0,2},{1,1}}, Table::EXPAND, 0 );
	main->Attach( CarnivoreSpeciesCount, 		{{3,2},{1,1}}, Table::FILL, 0 );
	main->Attach( lbl( "Carnivore Count" ), 	{{0,3},{1,1}}, Table::EXPAND, 0 );
	main->Attach( CarnivoreCount, 				{{3,3},{1,1}}, Table::FILL, 0 );
	main->Attach( lbl( "Herbivore Species" ), 	{{0,4},{1,1}}, Table::EXPAND, 0 );
	main->Attach( HerbivoreSpeciesCount, 		{{3,4},{1,1}}, Table::FILL, 0 );
	main->Attach( lbl( "Herbivore Count" ), 	{{0,5},{1,1}}, Table::EXPAND, 0 );
	main->Attach( HerbivoreCount, 				{{3,5},{1,1}}, Table::FILL, 0 );
	main->Attach( lbl( "Plant Species" ), 		{{0,6},{1,1}}, Table::EXPAND, 0 );
	main->Attach( PlantSpeciesCount, 			{{3,6},{1,1}}, Table::FILL, 0 );
	main->Attach( lbl( "Plant Count" ), 		{{0,7},{1,1}}, Table::EXPAND, 0 );
	main->Attach( PlantCount, 					{{3,7},{1,1}}, Table::FILL, 0 );

	main->Attach( Box::Create(), 				{{1,0},{1,8}});
	main->Attach( Box::Create(), 				{{0,8},{3,1}});

	main->SetColumnSpacings( 2.0f );
	main->SetRowSpacings( 2.0f );

	return main;
}

SharedPtr<Widget> NewSimWindow::CreateTerrainPage()
{
	MaxHeight = entry("sim.terragen.debug.maxheight");
	Size = entry("sim.terragen.debug.size");
	WaterLimit = entry("sim.terragen.debug.waterlimit");
	NutritionMin = entry("sim.terragen.debug.nutrition.min");
	NutritionMax = entry("sim.terragen.debug.nutrition.max");
	HumidityMin = entry("sim.terragen.debug.humidity.min");
	HumidityMax = entry("sim.terragen.debug.humidity.max");
	RainAmount = entry("sim.terragen.debug.humidity.rainamount");
	RainFall = entry("sim.terragen.debug.humidity.rainfall");

	Table::Ptr main = Table::Create();

		main->Attach( lbl( "Maximum Height" ),			{{0,0},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxHeight, 						{{3,0},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Island Size (square)" ), 	{{0,1},{1,1}}, Table::EXPAND, 0 );
		main->Attach( Size, 							{{3,1},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Sea Level\n(%maxheight)" ), {{0,2},{1,1}}, Table::EXPAND, 0 );
		main->Attach( WaterLimit, 						{{3,2},{1,1}}, Table::FILL, 0 );

		main->Attach( lbl( "Nutrition ( min - max )" ), {{0,3},{1,1}}, Table::EXPAND, 0 );
			Box::Ptr nBox = Box::Create();
					nBox->Pack( NutritionMin );
					nBox->Pack( lbl( " - " ), false, false);
					nBox->Pack( NutritionMax );
		main->Attach( nBox, 							{{3,3},{1,1}}, Table::FILL, 0 );

		main->Attach( lbl( "Humidity ( min - max )" ), 	{{0,4},{1,1}}, Table::EXPAND, 0 );
			Box::Ptr hBox = Box::Create();
					hBox->Pack( HumidityMin );
					hBox->Pack( lbl( " - " ), false, false);
					hBox->Pack( HumidityMax );
		main->Attach( hBox, 							{{3,4},{1,1}}, Table::FILL, 0 );

		main->Attach( lbl( "Rain Amount" ), {{0,5},{1,1}}, Table::EXPAND, 0 );
		main->Attach( RainAmount, 						{{3,5},{1,1}}, Table::FILL, 0 );

		main->Attach( lbl( "Rainfall %" ), {{0,6},{1,1}}, Table::EXPAND, 0 );
		main->Attach( RainFall, 						{{3,6},{1,1}}, Table::FILL, 0 );

		main->Attach( Box::Create(), 					{{1,0},{1,5}});
	main->Attach( Box::Create(), 						{{0,5},{3,1}});

	main->SetColumnSpacings( 2.0f );
	main->SetRowSpacings( 2.0f );

	return main;
}

SharedPtr<Widget> NewSimWindow::CreateSpeciesPage()
{
	t_species.clear();
	t_species_count.clear();
	MaxAge = entry("sim.species.defaults.maxAge");
	MaxHealth = entry("sim.species.defaults.maxHealth");
	MaxSpeed = entry("sim.species.defaults.maxSpeed.carnivore");
	Reach = entry("sim.species.defaults.reach.carnivore");
	Resistance = entry("sim.species.defaults.resistance");
	BreedingSpeed = entry("sim.species.defaults.breedingSpeed.carnivore");
	MaxRegen = entry("sim.species.defaults.maxRegeneration.carnivore");
	FoodReq = entry("sim.species.defaults.foodRequirement.carnivore");
	WaterReq = entry("sim.species.defaults.waterRequirement.carnivore");
	OptimalTemperature = entry("sim.species.defaults.optimalTemperature");
	NewSpecies = entry("");
	SpeciesType = entry("HERBA", 0);
	SpeciesCount = entry("0", 0);

	Table::Ptr main = Table::Create();
	Button::Ptr okBtn = Button::Create( "Create" );
	okBtn->GetSignal( Button::OnLeftClick ).Connect( &NewSimWindow::newSpeciesClick , this );
	s_list = *(new sbe::sfgList("SPECIES_GEN_CLICKED"));
	main->Attach(s_list.getList(), {{0,0},{1,1}}, Table::EXPAND, 0);

		main->Attach( lbl("New Species" ),			{{1,0},{1,1}}, Table::EXPAND, 0 );
		main->Attach( NewSpecies, 					{{2,0},{1,1}}, Table::EXPAND, 0 );
		main->Attach( okBtn, 						{{3,0},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl("Species Count" ),		{{0,1},{1,1}}, Table::EXPAND, 0 );
		main->Attach( SpeciesCount, 				{{3,1},{1,1}}, Table::EXPAND, 0 );
		main->Attach( lbl( "Species Type" ),		{{0,2},{1,1}}, Table::EXPAND, 0 );
		main->Attach( SpeciesType, 					{{3,2},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Age" ),			{{0,3},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxAge, 						{{3,3},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Health" ), 		{{0,4},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxHealth, 					{{3,4},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Speed" ), 		{{0,5},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxSpeed, 					{{3,5},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Reach" ), 		{{0,6},{1,1}}, Table::EXPAND, 0 );
		main->Attach( Reach, 						{{3,6},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Resistance" ), 			{{0,7},{1,1}}, Table::EXPAND, 0 );
		main->Attach( Resistance, 					{{3,7},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "BreedingSpeed" ), 		{{0,8},{1,1}}, Table::EXPAND, 0 );
		main->Attach( BreedingSpeed, 				{{3,8},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Regeneration" ),{{0,9},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxRegen, 					{{3,9},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Food Requirement" ), 	{{0,10},{1,1}}, Table::EXPAND, 0 );
		main->Attach( FoodReq,			 			{{3,10},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Water Requirement" ), 	{{0,11},{1,1}}, Table::EXPAND, 0 );
		main->Attach( WaterReq,			 			{{3,11},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Optimal Temperature" ), {{0,12},{1,1}}, Table::EXPAND, 0 );
		main->Attach( OptimalTemperature, 			{{3,12},{1,1}}, Table::FILL, 0 );

		main->Attach( Box::Create(), 					{{1,0},{1,5}});
	main->Attach( Box::Create(), 						{{0,5},{3,1}});

	main->SetColumnSpacings( 2.0f );
	main->SetRowSpacings( 2.0f );
	return main;
}

void NewSimWindow::okClick()
{

	Engine::getCfg()->set("sim.terragen.count", CountMult->GetText().toAnsiString());
	Engine::getCfg()->set("sim.defaultSeed", Seed->GetText().toAnsiString());

	Engine::getCfg()->set("sim.terragen.species.carnivore", CarnivoreSpeciesCount->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.species.herbivore", HerbivoreSpeciesCount->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.species.plant", PlantSpeciesCount->GetText().toAnsiString());

	Engine::getCfg()->set("sim.terragen.count.carnivore", CarnivoreCount->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.count.herbivore", HerbivoreCount->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.count.plant", PlantCount->GetText().toAnsiString());


	Engine::getCfg()->set("sim.terragen.debug.maxheight", MaxHeight->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.debug.size", Size->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.debug.waterlimit", WaterLimit->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.debug.nutrition.min", NutritionMin->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.debug.nutrition.max", NutritionMax->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.debug.humidity.min", HumidityMin->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.debug.humidity.max", HumidityMax->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.debug.humidity.rainfall", RainFall->GetText().toAnsiString());
	Engine::getCfg()->set("sim.terragen.debug.humidity.rainamount", RainAmount->GetText().toAnsiString());

	Engine::getCfg()->set("sim.species.defaults.maxAge", MaxAge->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.maxHealth", MaxHealth->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.maxSpeed.carnivore", MaxSpeed->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.reach.carnivore", Reach->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.resistance", Resistance->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.breedingSpeed.carnivore", BreedingSpeed->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.maxRegeneration.carnivore", MaxRegen->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.foodRequirement.carnivore", FoodReq->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.waterRequirement.carnivore", WaterReq->GetText().toAnsiString());
	Engine::getCfg()->set("sim.species.defaults.optimalTemperature", OptimalTemperature->GetText().toAnsiString());

	Module::Get()->QueueEvent("CLEAR_OVERLAYS", true);
	Module::Get()->QueueEvent("RESET_SIMULATION", true);
	Win->Show(false);

	if(!t_species.empty())
	{
		auto s_tmp = std::make_shared<std::vector<std::shared_ptr<Species>>>(t_species);
		auto n_tmp = std::make_shared<std::vector<int>>(t_species_count);
		Module::Get()->QueueEvent(Event("NEW_NONRANDOM_SIM", std::make_pair(s_tmp, n_tmp)), true);
	}
}

void NewSimWindow::abortClick()
{
	Win->Show(false);
}

void NewSimWindow::newSpeciesClick()
{
	s_list.addItem(NewSpecies->GetText().toAnsiString());
	t_species_count.push_back(boost::lexical_cast<int>(SpeciesCount->GetText().toAnsiString()));
	createSpecies();
}

SharedPtr<Entry> NewSimWindow::entry( const std::string& cfg, sf::Vector2f req )
{
	SharedPtr<Entry> tmp = Entry::Create( Engine::getCfg()->get<std::string>(cfg) );
	tmp->SetRequisition( req );
	return tmp;
}

SharedPtr<Entry> NewSimWindow::entry( std::string _str, int _t, sf::Vector2f req )
{
	SharedPtr<Entry> tmp = Entry::Create( _str );
	tmp->SetRequisition( req );
	return tmp;
}

SharedPtr<Label> NewSimWindow::lbl( const std::string& text, sf::Vector2f Align )
{
	SharedPtr<Label> tmp = Label::Create(text);
	tmp->SetAlignment( Align );
	return tmp;
}

void NewSimWindow::createSpecies()
{
	Species::SPECIES_TYPE type;
	if(SpeciesType->GetText().toAnsiString().compare("HERBA") == 0)
	{
		type = Species::SPECIES_TYPE::HERBA;
	} else if(SpeciesType->GetText().toAnsiString().compare("HERBIVORE") == 0) {
		type = Species::SPECIES_TYPE::HERBIVORE;
	} else {
		type = Species::SPECIES_TYPE::CARNIVORE;
	}
	std::shared_ptr<Species> S ( new Species( NewSpecies->GetText().toAnsiString(), type) );


	S->setMaxAge(boost::lexical_cast<int>(MaxAge->GetText().toAnsiString()));
	S->setMaxHealth(boost::lexical_cast<float>(MaxHealth->GetText().toAnsiString()));
	S->setMaxSpeed(boost::lexical_cast<float>(MaxSpeed->GetText().toAnsiString()));
	S->setReach(boost::lexical_cast<float>(Reach->GetText().toAnsiString()));
	S->setResistance(boost::lexical_cast<float>(Resistance->GetText().toAnsiString()));
	S->setBreedingSpeed(boost::lexical_cast<int>(BreedingSpeed->GetText().toAnsiString()));
	S->setMaxRegeneration(boost::lexical_cast<float>(MaxRegen->GetText().toAnsiString()));
	S->setFoodRequirement(boost::lexical_cast<float>(FoodReq->GetText().toAnsiString()));
	S->setWaterRequirement(boost::lexical_cast<float>(WaterReq->GetText().toAnsiString()));
	S->setType( type );
	S->setOptimalTemperature(boost::lexical_cast<int>(OptimalTemperature->GetText().toAnsiString()));

	t_species.push_back(S);
}

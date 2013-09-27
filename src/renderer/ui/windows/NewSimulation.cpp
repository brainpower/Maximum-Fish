#include "NewSimulation.hpp"

#include "sbe/sfg/Message.hpp"

#include "sbe/Config.hpp"

#include "simulator/Generator.hpp"

#include <SFGUI/Window.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Notebook.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Table.hpp>
#include <SFGUI/Separator.hpp>
#include <SFGUI/ComboBox.hpp>

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
		if ( !Win )
        {
            CreateWindow();
            Module::Get()->QueueEvent( Event("SCREEN_ADD_WINDOW", Win) );
        }
        else Win->Show(true);

	}

	if( e.Is("SPECIES_GEN_CLICKED", typeid( std::string )))
	{
		std::string s =  boost::any_cast<std::string>(e.Data());

		if(last_selected != -1)  {
			modifySpecies(t_species[last_selected]);
		}

		for(int i = 0; i < t_species.size(); i++)
		{
			if(s.compare(t_species[i]->getName()) == 0)
			{
				NewSpecies->SetText(t_species[i]->getName());
				MaxAge->SetText(boost::lexical_cast<std::string>(t_species[i]->getMaxAge()));
				MaxHealth->SetText(boost::lexical_cast<std::string>(t_species[i]->getMaxHealth()));
				MaxSpeed->SetText(boost::lexical_cast<std::string>(t_species[i]->getMaxSpeed()));
				Reach->SetText(boost::lexical_cast<std::string>(t_species[i]->getReach()));
				Resistance->SetText(boost::lexical_cast<std::string>(t_species[i]->getResistance()));
				BreedingSpeed->SetText(boost::lexical_cast<std::string>(t_species[i]->getBreedingSpeed()));
				MaxRegen->SetText(boost::lexical_cast<std::string>(t_species[i]->getMaxRegeneration()));
				FoodReq->SetText(boost::lexical_cast<std::string>(t_species[i]->getFoodRequirement()));
				WaterReq->SetText(boost::lexical_cast<std::string>(t_species[i]->getWaterRequirement()));
				OptimalTemperature->SetText(boost::lexical_cast<std::string>(t_species[i]->getOptimalTemperature()));

				SpeciesType->SetText(Species::type2String( t_species[i]->getType() ));

				CreatureCount->SetText(boost::lexical_cast<std::string>(t_species_count[i]));
				last_selected = s_list.getIndex(s);
			}
		}
	}
}

void NewSimWindow::CreateWindow()
{
	Win = Window::Create( );
	Win->SetTitle("Create new simulation");

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
	SpeciesType = entry("plant", 0);
	SpeciesType->SetState(sfg::Widget::INSENSITIVE);
	CreatureCount = entry("0", 0);

	Table::Ptr main = Table::Create();
	Button::Ptr newBtn = Button::Create( "Add Default Species" );
	Button::Ptr rndBtn = Button::Create( "Add Random Species" );
	Button::Ptr delBtn = Button::Create( "Delete" );
	newBtn->GetSignal( Button::OnLeftClick ).Connect( &NewSimWindow::newSpeciesClick , this );
	rndBtn->GetSignal( Button::OnLeftClick ).Connect( &NewSimWindow::newRandomSpeciesClick , this );
	delBtn->GetSignal( Button::OnLeftClick ).Connect( &NewSimWindow::delSpeciesClick , this );

	Separator::Ptr spacer = Separator::Create(Separator::Orientation::HORIZONTAL);
	cmbBox = ComboBox::Create();
	cmbBox->AppendItem("plant");
	cmbBox->AppendItem("herbivore");
	cmbBox->AppendItem("carnivore");
	cmbBox->SelectItem(0);

	s_list.setClickEventName("SPECIES_GEN_CLICKED");

	unsigned r = 0;
	main->Attach(s_list.getList(), {{0,r},{1,6}}, Table::FILL, 0);

		main->Attach( lbl("New Species" ),			{{1,r++},{1,1}}, Table::EXPAND, 0 );
		main->Attach( NewSpecies, 					{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( cmbBox,						{{1,r++},{1,1}}, Table::FILL, 0);
		main->Attach( newBtn, 						{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( rndBtn, 						{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( delBtn, 						{{1,r++},{1,1}}, Table::FILL, 0);
		main->Attach( spacer, 				 		{{0,r++},{2,1}}, Table::FILL, 0);
		main->Attach( lbl("# of Creatures" ),		{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( CreatureCount, 				{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Species Type" ),		{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( SpeciesType, 					{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Age" ),			{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxAge, 						{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Health" ), 		{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxHealth, 					{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Speed" ), 		{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxSpeed, 					{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Reach" ), 		{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( Reach, 						{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Resistance" ), 			{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( Resistance, 					{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "BreedingSpeed" ), 		{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( BreedingSpeed, 				{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Maximum Regeneration" ),{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( MaxRegen, 					{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Food Requirement" ), 	{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( FoodReq,			 			{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Water Requirement" ), 	{{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( WaterReq,			 			{{1,r++},{1,1}}, Table::FILL, 0 );
		main->Attach( lbl( "Optimal Temperature" ), {{0,r},{1,1}}, Table::EXPAND, 0 );
		main->Attach( OptimalTemperature, 			{{1,r++},{1,1}}, Table::FILL, 0 );

		main->Attach( Box::Create(), 					{{1,0},{1,5}});
	main->Attach( Box::Create(), 						{{0,5},{3,1}});

	main->SetColumnSpacings( 5.0f );
	main->SetRowSpacings( 2.0f );

	for ( int i = 0; i < Engine::getCfg()->get<int>( "sim.terragen.species.plant" ); ++i )
		t_species.push_back(Generator::createSpecies(Species::SPECIES_TYPE::HERBA, "plant_" + boost::lexical_cast<std::string>(t_species.size() ) ));
	for ( int i = 0; i < Engine::getCfg()->get<int>( "sim.terragen.species.herbivore" ); ++i )
		t_species.push_back(Generator::createSpecies(Species::SPECIES_TYPE::HERBIVORE, "herbivore_" + boost::lexical_cast<std::string>(t_species.size() ) ));
	for ( int i = 0; i < Engine::getCfg()->get<int>( "sim.terragen.species.carnivore" ); ++i )
		t_species.push_back(Generator::createSpecies(Species::SPECIES_TYPE::CARNIVORE, "carnivore_" + boost::lexical_cast<std::string>(t_species.size() ) ));

	for(std::shared_ptr<Species> spe : t_species)
	{
		s_list.addItem(spe->getName());
		t_species_count.push_back(Engine::getCfg()->get<int>("sim.terragen.count." + Species::type2String(spe->getType())));
	}
	s_list.select(0);
	return main;
}

void NewSimWindow::okClick()
{

	if(last_selected != -1)  {
		modifySpecies(t_species[last_selected]);
	}

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

	Win->Show(false);

	if(!t_species.empty())
	{
		auto s_tmp = std::make_shared<std::vector<std::shared_ptr<Species>>>(t_species);
		auto n_tmp = std::make_shared<std::vector<int>>(t_species_count);
		Module::Get()->QueueEvent(Event("NEW_NONRANDOM_SIM", std::make_pair(s_tmp, n_tmp)), true);
	}
}

void NewSimWindow::modifySpecies(std::shared_ptr<Species> S)
{
	S->setMaxAge(boost::lexical_cast<int>(MaxAge->GetText().toAnsiString()));
	S->setMaxHealth(boost::lexical_cast<float>(MaxHealth->GetText().toAnsiString()));
	S->setMaxSpeed(boost::lexical_cast<float>(MaxSpeed->GetText().toAnsiString()));
	S->setReach(boost::lexical_cast<float>(Reach->GetText().toAnsiString()));
	S->setResistance(boost::lexical_cast<float>(Resistance->GetText().toAnsiString()));
	S->setBreedingSpeed(boost::lexical_cast<int>(BreedingSpeed->GetText().toAnsiString()));
	S->setMaxRegeneration(boost::lexical_cast<float>(MaxRegen->GetText().toAnsiString()));
	S->setFoodRequirement(boost::lexical_cast<float>(FoodReq->GetText().toAnsiString()));
	S->setWaterRequirement(boost::lexical_cast<float>(WaterReq->GetText().toAnsiString()));

	S->setOptimalTemperature(boost::lexical_cast<int>(OptimalTemperature->GetText().toAnsiString()));
	t_species_count[last_selected] = boost::lexical_cast<int>(CreatureCount->GetText().toAnsiString());
}

void NewSimWindow::abortClick()
{
	Win->Show(false);
}

void NewSimWindow::newSpeciesClick()
{
	std::string name = NewSpecies->GetText().toAnsiString();
	if(s_list.getIndex(name) < 0) {
		s_list.addItem(name);
		t_species_count.push_back(boost::lexical_cast<int>(CreatureCount->GetText().toAnsiString()));
		createSpecies();
	} else {
		std::string text = "The Species '" + name + "' is already present!";
		std::shared_ptr<sbe::Message> m( new sbe::Message(sbe::Message::OK, "Species already exists!", text ));
		Module::Get()->QueueEvent( Event("NEW_MESSAGE", m), true );
	}
}

void NewSimWindow::newRandomSpeciesClick()
{
	std::string name = NewSpecies->GetText().toAnsiString();
	if(s_list.getIndex(name) < 0) {
		s_list.addItem(name);
		t_species_count.push_back(boost::lexical_cast<int>(CreatureCount->GetText().toAnsiString()));
		std::shared_ptr<Species> S = Generator::createRandomSpecies(getCurrentSpeciesType(), name, rnd);
		t_species.push_back(S);
	} else {
		std::string text = "The Species '" + name + "' is already present!";
		std::shared_ptr<sbe::Message> m( new sbe::Message(sbe::Message::OK, "Species already exists!", text ));
		Module::Get()->QueueEvent( Event("NEW_MESSAGE", m), true );
	}
}

void NewSimWindow::delSpeciesClick()
{
	if ( last_selected == s_list.getIndex(s_list.getSelectedItem()) ) last_selected = -1;
	int index = s_list.getIndex(s_list.getSelectedItem());
	if ( index == -1 ) return;
	t_species.erase(t_species.begin()+index);
	t_species_count.erase(t_species_count.begin()+index);
	s_list.removeItem(s_list.getSelectedItem());
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
	std::shared_ptr<Species> S ( new Species( NewSpecies->GetText().toAnsiString(), getCurrentSpeciesType()) );
	t_species.push_back(S);
}

Species::SPECIES_TYPE NewSimWindow::getCurrentSpeciesType()
{
	return Species::string2Type(cmbBox->GetSelectedText().toAnsiString());
}

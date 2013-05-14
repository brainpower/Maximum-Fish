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
}

void NewSimWindow::HandleEvent(Event& e)
{
	if ( e.Is("KEY_SHOW_NEWSIM") )
	{
		CreateWindow();
		Module::Get()->QueueEvent( Event("SCREEN_ADD_WINDOW", Win) );
	}
}

void NewSimWindow::CreateWindow()
{
	Win = Window::Create( );
	Box::Ptr mainBox = Box::Create( Box::VERTICAL );
		Notebook::Ptr nB = Notebook::Create();
		nB->AppendPage( CreateGenerationPage(), lbl("Generation") );
		nB->AppendPage( CreateTerrainPage(), lbl("Terrain") );
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

	Module::Get()->QueueEvent("RESET_SIMULATION", true);
	Win->Show(false);
}

void NewSimWindow::abortClick()
{
	Win->Show(false);
}

SharedPtr<Entry> NewSimWindow::entry( const std::string& cfg, sf::Vector2f req )
{
	SharedPtr<Entry> tmp = Entry::Create( Engine::getCfg()->get<std::string>(cfg) );
	tmp->SetRequisition( req );
	return tmp;
}

SharedPtr<Label> NewSimWindow::lbl( const std::string& text, sf::Vector2f Align )
{
	SharedPtr<Label> tmp = Label::Create(text);
	tmp->SetAlignment( Align );
	return tmp;
}

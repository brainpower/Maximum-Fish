#include "CreatureList.hpp"
#include "renderer/Screen.hpp"

CreatureList::CreatureList( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    RegisterForEvent( "TOGGLE_SHOW_CREATURELIST" );
    RegisterForEvent( "WINDOW_RESIZE" );
    RegisterForEvent( "CREATURE_CLICKED" );
    RegisterForEvent( "TILE_CLICKED" );

	CreateWindow(RelativePosition, Size);
	Win->Show(false);
}

void CreatureList::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
    //make and configure window
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::SHADOW);
    Win->SetRequisition( sf::Vector2f( 10 , 10 ) );
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
    // set Allocation (instead of calling updatePosition, because size is unknown)
    Win->SetAllocation(sf::FloatRect( ( appSize.x - 300 ) , 0 , 300 , appSize.y ));


        //create wholebox
        sfg::Box::Ptr wholeBox( sfg::Box::Create( sfg::Box::VERTICAL, 3.0f ));

            //create notebook
            sfg::Notebook::Ptr tabs( sfg::Notebook::Create() );
                //create box for notebook
                sfg::Box::Ptr creatures( sfg::Box::Create() );
                sfg::Box::Ptr species(   sfg::Box::Create() );
                sfg::Box::Ptr tile(      sfg::Box::Create() );
                //create label for box
                sfg::Label::Ptr creaturesLabel( sfg::Label::Create("Creatures") );
                sfg::Label::Ptr speciesLabel(   sfg::Label::Create("Species") );
                sfg::Label::Ptr tileLabel(      sfg::Label::Create("Environment") );
            //add box to notebook
            tabs->AppendPage(creatures,creaturesLabel);
            tabs->AppendPage(species,speciesLabel);
            tabs->AppendPage(tile,tileLabel);

            //create details
            CreatureFrame = sfg::Frame::Create("Creature");
                CurrentDetailsCreature.reset(new DetailsEmpty());
            CreatureFrame->Add(CurrentDetailsCreature->Get());
            //create details
            SpeciesFrame = sfg::Frame::Create("Species");
                CurrentDetailsSpecies.reset(new DetailsEmpty());
            SpeciesFrame->Add(CurrentDetailsSpecies->Get());
            //create details
            TileFrame = sfg::Frame::Create("Tile");
                CurrentDetailsTile.reset(new DetailsEmpty());
            TileFrame->Add(CurrentDetailsTile->Get());

        //pack all into wholebox
        wholeBox->Pack(tabs);
        wholeBox->Pack(CreatureFrame, false, true);
        wholeBox->Pack(SpeciesFrame, false, true);
        wholeBox->Pack(TileFrame, false, true);

    //add wholebox to window
    Win->Add(wholeBox);


	Event e( "SCREEN_ADD_WINDOW" );
	e.SetData( Win );
	Module::Get()->QueueEvent( e );
}

void CreatureList::HandleEvent( Event& e )
{
	if (e.Is("WINDOW_RESIZE"))
    {
        updatePosition();
    }
	else if (e.Is("TOGGLE_SHOW_CREATURELIST"))
    {
        if (Win->IsGloballyVisible())
		{
			Win->Show(false);
            Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,1,1 ) );
		}
        else
		{
			updatePosition();
			Win->Show(true);
			Win->GrabFocus();
		}
    }
    else if (e.Is("CREATURE_CLICKED"))
    {
        if (e.Data().type() == typeid(std::shared_ptr<Creature>))
        {
            std::shared_ptr<Creature> c = boost::any_cast<std::shared_ptr<Creature>>(e.Data());
            SetDetail(c);
            Engine::out() << "[CreatureList]: DetailsCreature updated." << std::endl;
        }
    }
    else if (e.Is("TILE_CLICKED"))
    {
        if (e.Data().type() == typeid(std::shared_ptr<Tile>))
        {
            std::shared_ptr<Tile> t = boost::any_cast<std::shared_ptr<Tile>>(e.Data());
            SetDetail(t);
            Engine::out() << "[CreatureList]: DetailsTile updated." << std::endl;
        }
    }
}

void CreatureList::SetDetail(const std::shared_ptr<Creature>& _creature)
{
    CreatureFrame->RemoveAll();

    if (_creature)
    {
        CurrentDetailsCreature.reset(new DetailsCreature(_creature));
        CreatureFrame->Add(CurrentDetailsCreature->Get());
    }
    else
    {
        CreatureFrame->Add(sfg::Label::Create("Something went wrong!\nThis Creature is not available."));
    }
}

void CreatureList::SetDetail( const std::shared_ptr<Species>& _species)
{
    SpeciesFrame->RemoveAll();

    if (_species)
    {
        CurrentDetailsSpecies.reset(new DetailsSpecies(_species));
        SpeciesFrame->Add(CurrentDetailsSpecies->Get());
    }
    else
    {
        SpeciesFrame->Add(sfg::Label::Create("Something went wrong!\nThis Species is not available."));
    }
}

void CreatureList::SetDetail( const std::shared_ptr<Tile>& _tile)
{
    TileFrame->RemoveAll();

    if (_tile)
    {
        CurrentDetailsTile.reset(new DetailsTile(_tile));
        TileFrame->Add(CurrentDetailsTile->Get());
    }
    else
    {
        TileFrame->Add(sfg::Label::Create("Something went wrong!\nThis Tile is not available."));
    }
}

void CreatureList::updatePosition()
{
    //get widgetAllocation:
	sf::FloatRect widgetAllocation = Win->GetAllocation();
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
	//set new widgetAllocation:
	Win->SetAllocation(	sf::FloatRect( ( appSize.x - (widgetAllocation.width) ) , 0 , widgetAllocation.width , appSize.y ) );
    //set new ViewPort
    if (Win->IsGloballyVisible())
    {
        Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,(1-widgetAllocation.width/appSize.x),1 ) );
    }
    else
    {
        Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,1,1 ) );
    }
}

#include "CreatureList.hpp"


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
            DetailsWin = sfg::Window::Create();
            DetailsWin->SetStyle(0);
                CurrentDetails.reset(new DetailsEmpty());
            DetailsWin->Add(CurrentDetails->Get());

        //pack all into wholebox
        wholeBox->Pack(tabs);
        wholeBox->Pack(DetailsWin, false, true);

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
        }
    }
    else if (e.Is("TILE_CLICKED"))
    {
        if (e.Data().type() == typeid(std::shared_ptr<Tile>))
        {
            std::shared_ptr<Tile> t = boost::any_cast<std::shared_ptr<Tile>>(e.Data());
            SetDetail(t);
        }
    }
}

void CreatureList::SetDetail(const std::shared_ptr<Creature>& _creature)
{
    DetailsWin->RemoveAll();

    if (!_creature)
    {
        CurrentDetails.reset(new DetailsCreature(_creature));
        DetailsWin->Add(CurrentDetails->Get());
    }
    else
    {
        DetailsWin->Add(sfg::Label::Create("Something went wrong!\nThis Creature is not available."));
    }
}

void CreatureList::SetDetail( const std::shared_ptr<Species>& _species)
{
    DetailsWin->RemoveAll();

    if (!_species)
    {
        CurrentDetails.reset(new DetailsSpecies(_species));
        DetailsWin->Add(CurrentDetails->Get());
    }
    else
    {
        DetailsWin->Add(sfg::Label::Create("Something went wrong!\nThis Species is not available."));
    }
}

void CreatureList::SetDetail( const std::shared_ptr<Tile>& _tile)
{
    DetailsWin->RemoveAll();

    if (!_tile)
    {
        CurrentDetails.reset(new DetailsTile(_tile));
        DetailsWin->Add(CurrentDetails->Get());
    }
    else
    {
        DetailsWin->Add(sfg::Label::Create("Something went wrong!\nThis Tile is not available."));
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
    //std::shared_ptr<Screen> screen = Screen::GetScreenObj();
    if (Win->IsGloballyVisible())
    {
        //screen->setCameraViewPort( sf::FloatRect( 0,0,(1-widgetAllocation.width/appSize.x),1 ) );
    }
    else
    {
        //screen->setCameraViewPort( sf::FloatRect( 0,0,1,1 ) );
    }
}

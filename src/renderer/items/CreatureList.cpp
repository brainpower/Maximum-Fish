#include "CreatureList.hpp"


CreatureList::CreatureList( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    RegisterForEvent( "TOGGLE_SHOW_CREATURELIST" );
    RegisterForEvent( "WINDOW_RESIZE" );

    CreatureDetails* Details = nullptr;

	CreateWindow(RelativePosition, Size);
	Win->Show(false);
}

void CreatureList::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
    //make and configure window
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::SHADOW /* | sfg::Window::Style::RESIZE */);
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
                sfg::Box::Ptr creatures(   sfg::Box::Create() );
                sfg::Box::Ptr species(     sfg::Box::Create() );
                sfg::Box::Ptr environment( sfg::Box::Create() );
                //create label for box
                sfg::Label::Ptr creaturesLabel(   sfg::Label::Create("Creatures") );
                sfg::Label::Ptr speciesLabel(     sfg::Label::Create("Species") );
                sfg::Label::Ptr environmentLabel( sfg::Label::Create("Environment") );
            //add box to notebook
            tabs->AppendPage(creatures,creaturesLabel);
            tabs->AppendPage(species,speciesLabel);
            tabs->AppendPage(environment,environmentLabel);

            //create details
            Details =  new CreatureDetails(new Creature());


        //pack all into wholebox
        wholeBox->Pack(tabs);
        wholeBox->Pack(Details->Get(),false, true);

    //add wholebox to window
    Win->Add(wholeBox);



	Event e("SCREEN_ADD_WINDOW");
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
}

void CreatureList::updatePosition()
{
    //get widgetAllocation:
	sf::FloatRect widgetAllocation = Win->GetAllocation();
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
	//set new widgetAllocation:
	Win->SetAllocation(	sf::FloatRect( ( appSize.x - (widgetAllocation.width) ) , 0 , widgetAllocation.width , appSize.y ) );
	}

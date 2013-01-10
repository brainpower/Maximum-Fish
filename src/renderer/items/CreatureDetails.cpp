#include "CreatureDetails.hpp"



CreatureDetails::CreatureDetails( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    RegisterForEvent( "TOGGLE_SHOW_CREATUREDETAILS" );
    RegisterForEvent( "WINDOW_RESIZE" );

	currentlabeltext = 0;
	CreateWindow(RelativePosition, Size);
	Win->Show(false);
}

void CreatureDetails::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
	Win = sfg::Window::Create();
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
    sfg::Box::Ptr creatures( sfg::Box::Create() );
    sfg::Box::Ptr species(   sfg::Box::Create() );
    //create label for box
    sfg::Label::Ptr creaturesLabel( sfg::Label::Create("Creatures") );
    sfg::Label::Ptr speciesLabel(   sfg::Label::Create("Species") );
    //add box to notebook
    tabs->AppendPage(creatures,creaturesLabel);
    tabs->AppendPage(species,speciesLabel);

    //create details
    sfg::Box::Ptr details( sfg::Box::Create( sfg::Box::VERTICAL, 3.0f ));
    sfg::Label::Ptr x( sfg::Label::Create("Details") );
    details->Pack(x);

    //pack all into wholebox
    wholeBox->Pack(tabs);
    wholeBox->Pack(details);

    //add wholebox to window
    Win->Add(wholeBox);

	Event e("SCREEN_ADD_WINDOW");
	e.SetData( Win );
	Module::Get()->QueueEvent( e );
}

void CreatureDetails::HandleEvent( Event& e )
{
	if (e.Is("WINDOW_RESIZE"))
    {
        updatePosition();
    }
	else if (e.Is("TOGGLE_SHOW_CREATUREDETAILS"))
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

void CreatureDetails::updatePosition()
{
    //get widgetAllocation:
	sf::FloatRect widgetAllocation = Win->GetAllocation();
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
	//set new widgetAllocation:
	Win->SetAllocation(	sf::FloatRect( ( appSize.x - (widgetAllocation.width) ) , 0 , widgetAllocation.width , appSize.y ) );
	}

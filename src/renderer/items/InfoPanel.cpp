#include "InfoPanel.hpp"
#include "renderer/Screen.hpp"

InfoPanel::InfoPanel( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    RegisterForEvent( "TOGGLE_SHOW_INFOPANEL" );
    RegisterForEvent( "TOGGLE_SELECT_MAN" );
    RegisterForEvent( "WINDOW_RESIZE" );
    RegisterForEvent( "TOGGLE_FULLSCREEN" );

	CreateWindow( RelativePosition, Size );
}

void InfoPanel::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
    //make and configure window
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::SHADOW );
    Win->SetRequisition( sf::Vector2f( 10, 10 ) );
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
    // set Allocation (instead of calling updatePosition, because size is unknown)
    Win->SetAllocation( sf::FloatRect( ( appSize.x - 300 ), 0, 300, appSize.y ) );


        //create toplevel notebook
        Topleveltabs = sfg::Notebook::Create();
        Topleveltabs->SetState( sfg::Widget::State::INSENSITIVE );

            //create wholeseletionBox
            MySelector.reset( new Selector );
            SelectionBox = MySelector->Get();

            //create wholemanipulationBox
            MyManipulator.reset( new Manipulator );
            ManipulationBox = MyManipulator->Get();

        //pack all into toplevelnotebook
        Topleveltabs->AppendPage( SelectionBox, sfg::Label::Create( "Selection" ) );
        Topleveltabs->AppendPage( ManipulationBox, sfg::Label::Create( "Manipulation" ) );

    //add wholeselectionbox to window
    Win->Add( Topleveltabs );
    updatePosition();


	Event e( "SCREEN_ADD_WINDOW" );
	e.SetData( Win );
	Module::Get()->QueueEvent( e );
}

void InfoPanel::HandleEvent( Event& e )
{
	if ( e.Is( "WINDOW_RESIZE" ) || e.Is( "TOGGLE_FULLSCREEN" ) )
    {
        updatePosition();
    }
	else if ( e.Is( "TOGGLE_SHOW_INFOPANEL" ) )
    {
        if ( Win->IsGloballyVisible() )
		{
			Win->Show( false );
            //Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,1,1 ) );
		}
        else
		{
			updatePosition();
			Win->Show( true );
			Win->GrabFocus();
		}
    }
    else if ( e.Is( "TOGGLE_SELECT_MAN" ) )
    {
       if ( Topleveltabs->GetCurrentPage() == 1 )
       {
           Topleveltabs->SetCurrentPage( 0 );
       }
       else if ( Topleveltabs->GetCurrentPage() == 0 )
       {
           Topleveltabs->SetCurrentPage( 1 );
       }
    }
}

void InfoPanel::updatePosition()
{
    //get widgetAllocation:
	sf::FloatRect widgetAllocation = Win->GetAllocation();
	//get current Application size:
	sf::Vector2u appSize =  Engine::GetApp().getSize();
	//set new widgetAllocation:
	Win->SetAllocation(	sf::FloatRect( ( appSize.x - ( widgetAllocation.width ) ) , 0 , widgetAllocation.width , appSize.y ) );
    //set new ViewPort
    if ( Win->IsGloballyVisible() )
    {
        //Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,(1-widgetAllocation.width/appSize.x),1 ) );
    }
    else
    {
        //Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,1,1 ) );
    }
}

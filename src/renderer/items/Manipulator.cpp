#include "Manipulator.hpp"
#include "renderer/Screen.hpp"

Manipulator::Manipulator( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
    RegisterForEvent( "TOGGLE_SHOW_INFOPANEL" );
    RegisterForEvent( "TOGGLE_IPAN_MAN" );
    RegisterForEvent( "WINDOW_RESIZE" );
	CreateWindow(RelativePosition, Size);
	Win->Show(false);
	ThisNotInfoPanel=false;
}

void Manipulator::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
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

            //create InfoFrame
            sfg::Frame::Ptr infoFrame( sfg::Frame::Create("Information") );
            infoFrame->SetRequisition( sf::Vector2f(200,200) );
            Information = sfg::Label::Create();
            ResetInformation();
            infoFrame->Add(Information);

            //create manipulation1
            sfg::Frame::Ptr m1( sfg::Frame::Create("m1") );
            unsigned int serial1on  = m1->GetSignal( sfg::Frame::OnMouseEnter).Connect( &Manipulator::SetInformation_1, this);
            unsigned int serial1off = m1->GetSignal( sfg::Frame::OnMouseLeave).Connect( &Manipulator::ResetInformation, this);
            //create manipulation2
            sfg::Frame::Ptr m2( sfg::Frame::Create("m2") );
            unsigned int serial2on  = m2->GetSignal( sfg::Frame::OnMouseEnter).Connect( &Manipulator::SetInformation_2, this);
            unsigned int serial2off = m2->GetSignal( sfg::Frame::OnMouseLeave).Connect( &Manipulator::ResetInformation, this);

        //pack all into wholebox
        wholeBox->Pack(infoFrame, false, true);
        wholeBox->Pack(m1, false, true);
        wholeBox->Pack(m2, false, true);

    //set events
    Win->GetSignal(   sfg::Window::OnRightClick ).Connect( &Manipulator::SwitchToInfoPanel, this );

    //add wholebox to window
    Win->Add(wholeBox);

    //update Position
    updatePosition();

	Event e( "SCREEN_ADD_WINDOW" );
	e.SetData( Win );
	Module::Get()->QueueEvent( e );
}

void Manipulator::SwitchToInfoPanel()
{
    Module::Get()->QueueEvent( Event( "TOGGLE_IPAN_MAN" ) );
    Module::Get()->QueueEvent( Event( "LOCK_SIM_ON_PAUSE" ), true );
}

void Manipulator::ResetInformation()
{
    Information->RequestResize();
    Information->SetText("Hover over an option below\nto show more information\nabout it.");
}

void Manipulator::SetInformation_1()
{
    Information->RequestResize();
    Information->SetText("This is some Information to m1.\nThere is no minimal or maximal Value,\nnor is anything displayed,\nso this is useless for now");
}

void Manipulator::SetInformation_2()
{
    Information->RequestResize();
    Information->SetText("Information for m2");
}

void Manipulator::HandleEvent( Event& e )
{
	if (e.Is("WINDOW_RESIZE") && Win->IsGloballyVisible())
    {
        updatePosition();
    }
	else if (e.Is("TOGGLE_SHOW_INFOPANEL") && ThisNotInfoPanel == true)
    {
        if (Win->IsGloballyVisible())
		{
			Win->Show(false);
            //Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,1,1 ) );
		}
        else
		{
			updatePosition();
			Win->Show(true);
			ResetInformation();
			Win->GrabFocus();
		}
    }
    else if (e.Is("TOGGLE_IPAN_MAN"))
    {
        Win->Show(!Win->IsGloballyVisible());
        ResetInformation();
        ThisNotInfoPanel = !ThisNotInfoPanel;
        if (Win->IsGloballyVisible()) Win->GrabFocus();
    }
}

void Manipulator::updatePosition()
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
        //Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,(1-widgetAllocation.width/appSize.x),1 ) );
    }
    else
    {
        //Screen::GetScreenObj()->setCameraViewPort( sf::FloatRect( 0,0,1,1 ) );
    }
}

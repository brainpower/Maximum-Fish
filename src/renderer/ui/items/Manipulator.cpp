#include "Manipulator.hpp"
#include "sbe/gfx/Screen.hpp"
#include "DetailsEmpty.hpp"

#include <SFGUI/Label.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Button.hpp>

Manipulator::Manipulator()
{
    RegisterForEvent( "TOGGLE_SELECT_MAN" );
	CreateBox();
	ThisNotSelector = false;
}

void Manipulator::CreateBox()
{

        //create wholebox
        WholeBox = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 3.0f );

       /*     //create InfoFrame
            sfg::Frame::Ptr infoFrame( sfg::Frame::Create( "Information" ) );
            infoFrame->SetRequisition( sf::Vector2f( 200, 200 ) );
            Information = sfg::Label::Create();
            ResetInformation();
            infoFrame->Add( Information );

            //create manipulation1
            sfg::Frame::Ptr m1( sfg::Frame::Create( "m1" ) );
            unsigned int serial1on  = m1->GetSignal( sfg::Frame::OnMouseEnter ).Connect( std::bind( &Manipulator::SetInformation_1, this ));
            unsigned int serial1off = m1->GetSignal( sfg::Frame::OnMouseLeave ).Connect( std::bind( &Manipulator::ResetInformation, this );
            //create manipulation2
            sfg::Frame::Ptr m2( sfg::Frame::Create("m2") );
            unsigned int serial2on  = m2->GetSignal( sfg::Frame::OnMouseEnter ).Connect( std::bind( &Manipulator::SetInformation_2, this ));
            unsigned int serial2off = m2->GetSignal( sfg::Frame::OnMouseLeave ).Connect( std::bind( &Manipulator::ResetInformation, this ));

        //pack all into wholebox
        WholeBox->Pack( infoFrame, false, true );
        WholeBox->Pack( m1, false, true );
        WholeBox->Pack( m2, false, true );
*/
        CurrentDetailsBox.reset( new DetailsEmpty() );
        WholeBox->Pack( CurrentDetailsBox->GetBox() );
    //set events
    WholeBox->GetSignal( sfg::Box::OnRightClick ).Connect( std::bind( &Manipulator::SwitchToSelector, this) );

}

void Manipulator::SwitchToSelector()
{
    Module::Get()->QueueEvent( Event( "TOGGLE_SELECT_MAN" ) );
    Module::Get()->QueueEvent( Event( "PAUSELOCK_DOWN" ) );
}

void Manipulator::ResetInformation()
{
    Information->RequestResize();
    Information->SetText( "Hover over an option below\nto show more information\nabout it." );
}

void Manipulator::SetInformation_1()
{
    Information->RequestResize();
    Information->SetText( "This is some Information to m1.\nThere is no minimal or maximal Value,\nnor is anything displayed,\nso this is useless for now" );
}

void Manipulator::SetInformation_2()
{
    Information->RequestResize();
    Information->SetText( "Information for m2" );
}

void Manipulator::HandleEvent( Event& e )
{

    if ( e.Is( "TOGGLE_SELECT_MAN" ) )
    {
       /********/// ResetInformation();
        ThisNotSelector = !ThisNotSelector;

		if ( e.Data().type() == typeid( std::shared_ptr<Details> ) )
		{
			CurrentDetailsBox = boost::any_cast<std::shared_ptr<Details>>( e.Data() );
			WholeBox->RemoveAll();
			WholeBox->Pack( CurrentDetailsBox->GetBox() );
		}
    }
}

sfg::Box::Ptr Manipulator::Get()
{
    return WholeBox;
}

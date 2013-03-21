#include "Selector.hpp"

Selector::Selector()
{
    RegisterForEvent( "TOGGLE_SECECT_MAN" );
    RegisterForEvent( "CREATURE_CLICKED" );
    RegisterForEvent( "TILE_CLICKED" );
	CreateBox();
	ThisNotManipulator = true;
	CreatureEmpty = true;
	SpeciesEmpty = true;
	TileEmpty = true;
}

void Selector::CreateBox()
{
        //create wholebox
        WholeBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );

            //create notebook
            sfg::Notebook::Ptr selectiontabs( sfg::Notebook::Create() );
                //create box for notebook
                sfg::Box::Ptr creatures( sfg::Box::Create() );
                sfg::Box::Ptr species(   sfg::Box::Create() );
                sfg::Box::Ptr tile(      sfg::Box::Create() );
            //add box to notebook
            selectiontabs->AppendPage( creatures, sfg::Label::Create( "Creatures" ) );
            selectiontabs->AppendPage( species,   sfg::Label::Create( "Species" ) );
            selectiontabs->AppendPage( tile,      sfg::Label::Create( "Environment" ) );

            //create details
            CreatureFrame = sfg::Frame::Create( "Creature" );
                CurrentDetailsCreature.reset( new DetailsEmpty() );
            CreatureFrame->Add( CurrentDetailsCreature->GetLabel() );
            //create details
            SpeciesFrame = sfg::Frame::Create( "Species" );
                CurrentDetailsSpecies.reset( new DetailsEmpty() );
            SpeciesFrame->Add( CurrentDetailsSpecies->GetLabel() );
            //create details
            TileFrame = sfg::Frame::Create( "Tile" );
                CurrentDetailsTile.reset( new DetailsEmpty() );
            TileFrame->Add( CurrentDetailsTile->GetLabel() );

        //pack all into wholeselectionbox
        WholeBox->Pack( selectiontabs );
        WholeBox->Pack( CreatureFrame, false, true );
        WholeBox->Pack( SpeciesFrame,  false, true );
        WholeBox->Pack( TileFrame,     false, true );


    //set Events
    CreatureFrame->GetSignal( sfg::Frame::OnRightClick ).Connect( &Selector::SwitchToCreatureManipulator, this );
    CreatureFrame->GetSignal( sfg::Frame::OnMouseEnter ).Connect( &Selector::ExtendCreatureLabel, this );
    CreatureFrame->GetSignal( sfg::Frame::OnMouseLeave ).Connect( &Selector::ReduceCreatureLabel, this );
    CreatureFrame->GetSignal( sfg::Frame::OnRightClick ).Connect( &Selector::ReduceCreatureLabel, this );

    SpeciesFrame->GetSignal(  sfg::Frame::OnRightClick ).Connect( &Selector::SwitchToSpeciesManipulator, this );
    SpeciesFrame->GetSignal(  sfg::Frame::OnMouseEnter ).Connect( &Selector::ExtendSpeciesLabel, this );
    SpeciesFrame->GetSignal(  sfg::Frame::OnMouseLeave ).Connect( &Selector::ReduceSpeciesLabel, this );
    SpeciesFrame->GetSignal(  sfg::Frame::OnRightClick ).Connect( &Selector::ReduceSpeciesLabel, this );

    TileFrame->GetSignal(     sfg::Frame::OnRightClick ).Connect( &Selector::SwitchToTileManipulator, this );
    TileFrame->GetSignal(     sfg::Frame::OnMouseEnter ).Connect( &Selector::ExtendTileLabel, this );
    TileFrame->GetSignal(     sfg::Frame::OnMouseLeave ).Connect( &Selector::ReduceTileLabel, this );
    TileFrame->GetSignal(     sfg::Frame::OnRightClick ).Connect( &Selector::ReduceTileLabel, this );

}

void Selector::SwitchToCreatureManipulator()
{
    if ( !CreatureEmpty )
    {
        Module::Get()->QueueEvent( Event( "TOGGLE_SELECT_MAN", CurrentDetailsCreature ) );
        Module::Get()->QueueEvent( "PAUSELOCK_DOWN" );
    }
}

void Selector::SwitchToSpeciesManipulator()
{
    if ( !SpeciesEmpty )
    {
        Module::Get()->QueueEvent( Event( "TOGGLE_SELECT_MAN", CurrentDetailsSpecies ) );
        Module::Get()->QueueEvent( "PAUSELOCK_DOWN" );
    }
}

void Selector::SwitchToTileManipulator()
{
    if ( !TileEmpty )
    {
        Module::Get()->QueueEvent( Event( "TOGGLE_SELECT_MAN", CurrentDetailsTile ) );
        Module::Get()->QueueEvent( "PAUSELOCK_DOWN" );
    }
}

void Selector::ExtendCreatureLabel()
{
    if ( !CreatureEmpty )
    {
        CreatureFrame->SetLabel( "Creature -> Right-Click to manipulate." );
    }
}

void Selector::ReduceCreatureLabel()
{
    if ( !CreatureEmpty )
    {
        CreatureFrame->SetLabel( "Creature" );
    }
}

void Selector::ExtendSpeciesLabel()
{
    if ( !SpeciesEmpty )
    {
        SpeciesFrame->SetLabel( "Species -> Right-Click to manipulate." );
    }
}

void Selector::ReduceSpeciesLabel()
{
    if ( !SpeciesEmpty )
    {
        SpeciesFrame->SetLabel( "Species" );
    }
}

void Selector::ExtendTileLabel()
{
    if ( !TileEmpty )
    {
        TileFrame->SetLabel( "Tile -> Right-Click to manipulate." );
    }
}

void Selector::ReduceTileLabel()
{
    if ( !TileEmpty )
    {
        TileFrame->SetLabel( "Tile" );
    }
}

void Selector::HandleEvent( Event& e )
{
	if ( e.Is( "TOGGLE_SELECT_MAN" ) )
    {
        ThisNotManipulator = !ThisNotManipulator;
    }
    else if ( e.Is( "CREATURE_CLICKED", typeid( std::shared_ptr<Creature> ) ) )
    {
		std::shared_ptr<Creature> c = boost::any_cast<std::shared_ptr<Creature>>( e.Data() );

		if ( c )
		{
			SetDetail( c );
			SetDetail( c->getSpecies() );
		}
		else
		{
			CreatureFrame->Show( false );
			SpeciesFrame->Show( false );
		}
		//Engine::out() << "[Selector]: DetailsCreature updated." << std::endl;
    }
    else if ( e.Is( "TILE_CLICKED", typeid( std::shared_ptr<Tile> ) ) )
    {
		std::shared_ptr<Tile> t = boost::any_cast<std::shared_ptr<Tile>>( e.Data() );

		if ( t )
		{
			SetDetail( t );
		}
		else
		{
			CreatureFrame->Show( false );
			SpeciesFrame->Show( false );
			TileFrame->Show( false );
		}
		//Engine::out() << "[Selector]: DetailsTile updated." << std::endl;
    }
}

void Selector::SetDetail( const std::shared_ptr<Creature>& _creature )
{
    CreatureFrame->RemoveAll();

    if ( _creature )
    {
        CurrentDetailsCreature.reset( new DetailsCreature( _creature ) );
        CreatureFrame->Add( CurrentDetailsCreature->GetLabel() );
        CreatureEmpty = false;
        CreatureFrame->Show( true );
    }
    else
    {
        CreatureFrame->Add( sfg::Label::Create( "Something went wrong!\nThis Creature is not available." ) );
        CreatureEmpty = true;
    }
}

void Selector::SetDetail( const std::shared_ptr<Species>& _species )
{
    SpeciesFrame->RemoveAll();

    if ( _species )
    {
        CurrentDetailsSpecies.reset( new DetailsSpecies( _species ) );
        SpeciesFrame->Add( CurrentDetailsSpecies->GetLabel() );
        SpeciesEmpty = false;
        SpeciesFrame->Show( true );
    }
    else
    {
        SpeciesFrame->Add( sfg::Label::Create( "Something went wrong!\nThis Species is not available." ) );
        SpeciesEmpty = true;
    }
}

void Selector::SetDetail( const std::shared_ptr<Tile>& _tile )
{
    TileFrame->RemoveAll();

    if ( _tile )
    {
        CurrentDetailsTile.reset( new DetailsTile( _tile ) );
        TileFrame->Add( CurrentDetailsTile->GetLabel() );
        TileEmpty = false;
        TileFrame->Show( true );
    }
    else
    {
        TileFrame->Add( sfg::Label::Create( "Something went wrong!\nThis Tile is not available." ) );
        TileEmpty = true;
    }
}

sfg::Box::Ptr Selector::Get()
{
    return WholeBox;
}

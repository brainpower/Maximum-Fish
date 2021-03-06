#include "Overlays.hpp"
#include "sbe/gfx/ImageUtils.hpp"

#include <SFGUI/CheckButton.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Separator.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Image.hpp>

#include "sbe/gfx/Actor.hpp"
#include "sbe/gfx/actors/SpriteActor.hpp"
#include "sbe/gfx/Renderer.hpp"
#include "sbe/gfx/Screen.hpp"

#include "sbe/Config.hpp"
#include "sbe/Engine.hpp"
#include "sbe/ResourceManager.hpp"
#include "sbe/Module.hpp"

/// TODO: move into a header, dont copy!
enum RenderLayers {
	L_BACK = 0,
	L_TERRAIN,
	L_OVERLAY,
	L_CREATURES,
	L_END
};

Overlays::Overlays()
 : myOverlays("OVERLAY_CLICK")
 {
	RegisterForEvent( "OVERLAY_CLICK" );
	RegisterForEvent( "DISPLAY_MAP" );
	RegisterForEvent( "RESET_UI" );

	myBox = sfg::Box::Create( sfg::Box::Orientation::VERTICAL );

	myOverlays.getList()->SetRequisition( sf::Vector2f( 50, 150 ) );

	sfg::Box::Ptr Spacer = sfg::Box::Create( sfg::Box::Orientation::VERTICAL );
	Spacer->SetRequisition( sf::Vector2f( 10, 10 ) );


	sfg::Button::Ptr ClearButton = sfg::Button::Create("Clear");
	ClearButton->GetSignal( sfg::Button::OnLeftClick ).Connect( std::bind( &Overlays::ClearOverlays, this ));
	sfg::Box::Ptr cbuttonbox = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 0 );
	cbuttonbox->Pack ( Spacer, true, true );
	Spacer->SetRequisition( sf::Vector2f( 10, 10 ) );
	cbuttonbox->Pack ( ClearButton, false, false);


	sfg::Button::Ptr UpdateButton = sfg::Button::Create("Update Overlays now");
	UpdateButton->GetSignal( sfg::Button::OnLeftClick ).Connect( std::bind( &Overlays::ForceUpdate, this ));
	sfg::Box::Ptr ubuttonbox = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 0 );
	ubuttonbox->Pack ( Spacer, true, true );
	Spacer->SetRequisition( sf::Vector2f( 10, 10 ) );
	ubuttonbox->Pack ( UpdateButton, false, false);

	sfg::Button::Ptr DisableButton = sfg::Button::Create("Disable all Overlays");
	DisableButton->GetSignal( sfg::Button::OnLeftClick ).Connect( std::bind( &Overlays::HideAll, this ));
	sfg::Box::Ptr dbuttonbox = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 0 );
	dbuttonbox->Pack ( Spacer, true, true );
	Spacer->SetRequisition( sf::Vector2f( 10, 10 ) );
	dbuttonbox->Pack ( DisableButton, false, false);

	sfg::Separator::Ptr sep =  sfg::Separator::Create();
	sep->SetRequisition( sf::Vector2f( 10, 30 ) );

	sfg::CheckButton::Ptr ChkButton = sfg::CheckButton::Create("Update live");
	ChkButton->SetActive(Engine::getCfg()->get<bool>("system.ui.Overlays.live"));
	ChkButton->GetSignal( sfg::ToggleButton::OnToggle ).Connect( std::bind( &Overlays::ToggleLive, this ));
	ChkButton->SetRequisition( sf::Vector2f( 10, 30 ) );

	CurrentFrame = sfg::Frame::Create("None");
	CurrentFrame->Add( sfg::Label::Create("Empty."));
	CurrentFrame->SetRequisition( sf::Vector2f( 50, 150 ) );



	myBox->Pack( myOverlays.getList(), false, true );
	myBox->Pack( Spacer, true, true );
	myBox->Pack( cbuttonbox, false, false );
	myBox->Pack( ubuttonbox, false, false );
	myBox->Pack( dbuttonbox, false, false );
	myBox->Pack( sep, false, true );
	myBox->Pack( ChkButton, false, false );
	myBox->Pack( CurrentFrame, false, false );
 }

sfg::Widget::Ptr Overlays::Get()
{
	return myBox;
}

void Overlays::HandleEvent(Event& e)
{
	if (e.Is("DISPLAY_MAP", typeid( std::shared_ptr<sbe::MapPlotter> )))
	{
		auto p =  boost::any_cast<std::shared_ptr<sbe::MapPlotter>>(e.Data());
		AddMap( p );
	} else if (e.Is("OVERLAY_CLICK", typeid( std::string )))
	{
		auto s =  boost::any_cast<std::string>(e.Data());
		ShowMap( s );
	} else if (e.Is("RESET_UI"))
	{
		Engine::out() << "[Overlays] clearing." << std::endl;
		for ( auto& o : Maps ) RemoveActor(o.first);

		ClearOverlays();
		myOverlays.clear();
		Maps.clear();
	}
}

void Overlays::AddMap( std::shared_ptr<sbe::MapPlotter>& M )
{
	if (!M) {
		Engine::out() << "[Overlays::AddMap] INVALID POINTER!" << std::endl;
		return;
	}

	// we got an update
	bool update = Maps.count(M->getName()) == 1 ;
	// also update the actor
	bool actor = update && Maps[M->getName()].second ;

	if ( actor ) RemoveActor( M->getName() );

	Maps[M->getName()] = std::make_pair(M, std::shared_ptr<sbe::Actor>());

	if (actor) CreateActor( M->getName() );

	if (!update)
		myOverlays.addItem( M->getName() );
	// update current view if we are not live updating ( 3x per second is to much for sfgui) or if the sim is paused ( = this is no live update)
	else if ( (Engine::getCfg()->get<bool>("sim.paused") || !(Engine::getCfg()->get<bool>("system.ui.Overlays.live")) )
			  && myOverlays.getSelectedItem() == M->getName() )
		ShowMap( M->getName() );

	if ( myOverlays.getSelectedItem() == "" ) myOverlays.select(0);
}

void Overlays::ShowMap( const std::string& name )
{
	if ( Maps.count(name) == 1)
	{
		CurrentFrame->RemoveAll();
		CurrentFrame->SetLabel( name );

		sfg::Image::Ptr I = sfg::Image::Create( sbe::gfx::ScaleImage( Maps[name].first->getImage(), glm::ivec2(128,128)) );
		//sfg::Image::Ptr I = sfg::Image::Create( Maps[name].first->getImage() );
		sfg::CheckButton::Ptr CB = sfg::CheckButton::Create( "Active" );
		if (Maps[name].second) CB->SetActive ( true );
		CB->GetSignal( sfg::ToggleButton::OnToggle ).Connect( std::bind( &Overlays::ToggleRendering, this ));
		sfg::Box::Ptr B = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
		B->Pack(CB, false, false);
		B->Pack(I, false, false);
		CurrentFrame->Add(B);

	}
}

void Overlays::ClearOverlays()
{
	CurrentFrame->RemoveAll();
	CurrentFrame->SetLabel( "None" );
}

void Overlays::HideAll()
{
	for ( auto& overlay : Maps )
	{
		if (overlay.second.second)
			RemoveActor( overlay.first );
		if ( myOverlays.getSelectedItem() == overlay.first )
			ShowMap( overlay.first );
	}
}

void Overlays::ToggleLive()
{
	Engine::getCfg()->set("system.ui.Overlays.live", !(Engine::getCfg()->get<bool>("system.ui.Overlays.live")) );
}

void Overlays::ForceUpdate()
{
	Module::Get()->QueueEvent(Event("UPDATE_OVERLAYS"), true);
}

void Overlays::ToggleRendering()
{
	std::string name = CurrentFrame->GetLabel();

	if ( Maps.count(name) != 1) return;
	if (Maps[name].second)
		RemoveActor( name );
	else
		CreateActor( name );
}

void Overlays::CreateActor( const std::string& name )
{
	std::shared_ptr<sbe::Actor> A(Maps[name].second);
	// FIXME: A is reset immediately, either construct with new Actor or the reset is wrong
  A.reset( new sbe::SpriteActor() );

	sf::Image temp = Maps[name].first->getImage();
	sbe::gfx::SetImageAlpha( temp, 128 );

	std::shared_ptr<sf::Texture> tex ( new sf::Texture() );
	tex->loadFromImage( temp );

	sf::Sprite& sprite = std::dynamic_pointer_cast<sbe::SpriteActor>(A)->sprite;

	Engine::GetResMgr()->add( tex, "overlay_"+name );
	sprite.setTexture( *tex );

	int TileSize = 	Engine::getCfg()->get<int>("system.ui.simView.tileSize");
	int TerrainSize = 	Engine::getCfg()->get<int>("sim.terragen.debug.size");

	// the size of the terrain multiplied by the size of a tile
	sprite.setTextureRect(sf::IntRect(0,0, temp.getSize().x ,  temp.getSize().y));
	sprite.setScale(TerrainSize*TileSize/temp.getSize().x,
					TerrainSize*TileSize/temp.getSize().y);
	sprite.setPosition(0,0);

	sbe::Screen::sRndr()->addActor ( A, L_OVERLAY );
}

void Overlays::RemoveActor( const std::string& name )
{
	std::string tmp = "overlay_"+name;
	Engine::GetResMgr()->remove<sf::Texture>( tmp );
	std::shared_ptr<sbe::Actor> A(Maps[name].second);
	if ( !A ) return;
	sbe::Screen::sRndr()->removeActor( A->getID() );
	A.reset();
}

#include "Overlays.hpp"
#include "sbe/gfx/ImageUtils.hpp"

#include <SFGUI/CheckButton.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Separator.hpp>

#include "sbe/gfx/Actor.hpp"
#include "sbe/gfx/Renderer.hpp"
#include "sbe/gfx/Screen.hpp"

#include "sbe/Config.hpp"
#include "sbe/Engine.hpp"
#include "sbe/ResourceManager.hpp"

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

	myBox = sfg::Box::Create( sfg::Box::VERTICAL );

	myOverlays.getList()->SetRequisition( sf::Vector2f( 50, 150 ) );

	sfg::Box::Ptr Spacer = sfg::Box::Create( sfg::Box::VERTICAL );
	Spacer->SetRequisition( sf::Vector2f( 10, 10 ) );

	sfg::Button::Ptr ClearButton = sfg::Button::Create("Clear");
	ClearButton->GetSignal( sfg::Button::OnLeftClick ).Connect( &Overlays::ClearOverlays, this );

	sfg::Button::Ptr UpdateButton = sfg::Button::Create("Update Overlays now");
	UpdateButton->GetSignal( sfg::Button::OnLeftClick ).Connect( &Overlays::ForceUpdate, this );

	sfg::Separator::Ptr sep =  sfg::Separator::Create();
	sep->SetRequisition( sf::Vector2f( 10, 30 ) );

	sfg::CheckButton::Ptr ChkButton = sfg::CheckButton::Create("Update live");
	ChkButton->SetActive(Engine::getCfg()->get<bool>("system.ui.Overlays.live"));
	ChkButton->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Overlays::ToggleLive, this );
	ChkButton->SetRequisition( sf::Vector2f( 10, 30 ) );

	CurrentFrame = sfg::Frame::Create("None");
	CurrentFrame->Add( sfg::Label::Create("Empty."));
	CurrentFrame->SetRequisition( sf::Vector2f( 50, 150 ) );



	myBox->Pack( myOverlays.getList(), false, true );
	myBox->Pack( Spacer, true, true );
	myBox->Pack( ClearButton, false, false );
	myBox->Pack( UpdateButton, false, false );
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
	if (e.Is("DISPLAY_MAP", typeid( std::shared_ptr<MapPlotter> )))
	{
		auto p =  boost::any_cast<std::shared_ptr<MapPlotter>>(e.Data());
		AddMap( p );
	} else if (e.Is("OVERLAY_CLICK", typeid( std::string )))
	{
		auto s =  boost::any_cast<std::string>(e.Data());
		ShowMap( s );
	}
}

void Overlays::AddMap( std::shared_ptr<MapPlotter>& M )
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

	Maps[M->getName()] = std::make_pair(M, std::shared_ptr<Actor>());

	if (actor) CreateActor( M->getName() );

	if (!update)
		myOverlays.addItem( M->getName() );
	// update current view if we are not live updating ( 3x per second is to much for sfgui) or if the sim is paused ( = this is no live update)
	else if ( (Engine::getCfg()->get<bool>("system.sim.paused") || !(Engine::getCfg()->get<bool>("system.ui.Overlays.live")) )
			  && myOverlays.getSelectedItem() == M->getName() )
		ShowMap( M->getName() );

}

void Overlays::ShowMap( std::string& name )
{
	if ( Maps.count(name) == 1)
	{
		CurrentFrame->RemoveAll();
		CurrentFrame->SetLabel( name );

		sfg::Image::Ptr I = sfg::Image::Create( gfx::ScaleImage( Maps[name].first->getImage(), Geom::Vec2(128,128)) );
		//sfg::Image::Ptr I = sfg::Image::Create( Maps[name].first->getImage() );
		sfg::CheckButton::Ptr CB = sfg::CheckButton::Create( "Active" );
		if (Maps[name].second) CB->SetActive ( true );
		CB->GetSignal( sfg::ToggleButton::OnToggle ).Connect( &Overlays::ToggleRendering, this );
		sfg::Box::Ptr B = sfg::Box::Create(sfg::Box::VERTICAL);
		B->Pack(CB, false, false);
		B->Pack(I, false, false);
		CurrentFrame->Add(B);

	}
}

void Overlays::ClearOverlays()
{
	std::string name = CurrentFrame->GetLabel();

	CurrentFrame->RemoveAll();
	CurrentFrame->SetLabel( "None" );
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
	std::shared_ptr<Actor>& A(Maps[name].second);
	A.reset( new SpriteActor() );

	sf::Image temp = Maps[name].first->getImage();
	gfx::SetImageAlpha( temp, 128 );

	std::shared_ptr<sf::Texture> tex ( new sf::Texture() );
	tex->loadFromImage( temp );

	sf::Sprite& sprite = std::dynamic_pointer_cast<SpriteActor>(A)->sprite;

	Engine::GetResMgr()->add( tex, "overlay_"+name );
	sprite.setTexture( *tex );

	int TileSize = 	Engine::getCfg()->get<int>("system.ui.simView.tileSize");
	int TerrainSize = 	Engine::getCfg()->get<int>("system.sim.terragen.debug.size");

	// the size of the terrain multiplied by the size of a tile
	sprite.setTextureRect(sf::IntRect(0,0, temp.getSize().x ,  temp.getSize().y));
	sprite.setScale(TerrainSize*TileSize/temp.getSize().x,
					TerrainSize*TileSize/temp.getSize().y);
	sprite.setPosition(0,0);

	Screen::sRndr()->addActor ( A, L_OVERLAY );
}

void Overlays::RemoveActor( const std::string& name )
{
	std::string tmp = "overlay_"+name;
	Engine::GetResMgr()->remove<sf::Texture>( tmp );
	std::shared_ptr<Actor>& A(Maps[name].second);
	Screen::sRndr()->removeActor( A->getID() );
	A.reset();
}

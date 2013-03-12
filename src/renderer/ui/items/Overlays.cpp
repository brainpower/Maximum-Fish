#include "Overlays.hpp"
#include "sbe/gfx/ImageUtils.hpp"

#include <SFGUI/CheckButton.hpp>
#include <SFGUI/Button.hpp>

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
	sfg::Button::Ptr ClearButton = sfg::Button::Create("Clear");
	ClearButton->GetSignal( sfg::Button::OnLeftClick ).Connect( &Overlays::ClearOverlays, this );
	CurrentFrame = sfg::Frame::Create("None");
	CurrentFrame->Add( sfg::Label::Create("Empty."));

	myBox->Pack( myOverlays.getList(), true, true );
	myBox->Pack( ClearButton, true, false );
	myBox->Pack( CurrentFrame, true, true  );
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

	Maps[M->getName()] = std::make_pair(M, std::shared_ptr<Actor>());
	myOverlays.removeItem( M->getName() );
	myOverlays.addItem( M->getName() );
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
		B->Pack(I), true, true;
		CurrentFrame->Add(B);

	}
}

void Overlays::ClearOverlays()
{
	std::string name = CurrentFrame->GetLabel();

	CurrentFrame->RemoveAll();
	CurrentFrame->SetLabel( "None" );
}

void Overlays::ToggleRendering()
{
	std::string name = CurrentFrame->GetLabel();

	if ( Maps.count(name) == 1)
	{

		std::shared_ptr<Actor>& A(Maps[name].second);

		if ( !A )
		{
			Engine::out() << "Adding overlay" << std::endl;
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
		else
		{
			Engine::out() << "removing overlay" << std::endl;
			std::string tmp = "overlay_"+name;
			Engine::GetResMgr()->remove<sf::Texture>( tmp );
			Screen::sRndr()->removeActor( A->getID() );
			A.reset();
		}
	}
}

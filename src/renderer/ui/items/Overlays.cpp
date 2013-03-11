#include "Overlays.hpp"
#include "sbe/gfx/ImageUtils.hpp"

Overlays::Overlays()
 : myOverlays("OVERLAY_CLICK")
 {
	RegisterForEvent( "OVERLAY_CLICK" );
	RegisterForEvent( "DISPLAY_MAP" );

	myBox = sfg::Box::Create( sfg::Box::VERTICAL );
	ClearButton = sfg::Button::Create("Clear");
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

	Maps[M->getName()] = M;
	myOverlays.removeItem( M->getName() );
	myOverlays.addItem( M->getName() );
}

void Overlays::ShowMap( std::string& name )
{
	if ( Maps.count(name) == 1)
	{
		CurrentFrame->RemoveAll();
		CurrentFrame->SetLabel( name );

		sfg::Image::Ptr I = sfg::Image::Create( gfx::ScaleImage( Maps[name]->getImage(), Geom::Vec2(256,256)) );
		CurrentFrame->Add(I);
	}
}

void Overlays::ClearOverlays()
{
		CurrentFrame->RemoveAll();
		CurrentFrame->SetLabel( "None" );
}

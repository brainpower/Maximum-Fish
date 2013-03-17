#include "GraphBook.hpp"

#include <SFGUI/Window.hpp>
#include <SFGUI/Notebook.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Image.hpp>
#include <SFGUI/Label.hpp>

using namespace sfg;

GraphBook::GraphBook( const Geom::Vec2 Size )
{
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "TOGGLE_SHOW_GRAPHBOOK" );
	RegisterForEvent( "DISPLAY_GRAPH" );
	RegisterForEvent( "ADD_GRAPH_TO_BOOK" );

	CreateWindow( Size );
	Win->Show( false );
}

void GraphBook::CreateWindow( const Geom::Vec2 Size )
{
	Win = Window::Create( Window::Style::BACKGROUND | Window::Style::RESIZE | Window::Style::SHADOW );

	Win->SetRequisition( sf::Vector2f( Size.x, Size.y ) );
	updatePosition();

	Tabs = Notebook::Create();
	Tabs->SetTabPosition( Notebook::TabPosition::LEFT );
	Win->Add( Tabs );

	Module::Get()->QueueEvent( Event( "SCREEN_ADD_WINDOW", Win ) );
}

void GraphBook::HandleEvent( Event& e )
{
	if ( UpdateTimer.getElapsedTime() > sf::seconds( 2.0 ) && Win->IsGloballyVisible() )
	{
		UpdateGraph();
		UpdateTimer.restart();
	}
	if ( e.Is( "TOGGLE_SHOW_GRAPHBOOK" ) )
	{
		if ( Win->IsGloballyVisible() )
		{
			Win->Show( false );
		}
		else
		{
			updatePosition();
			UpdateGraph();//not realy clicked, but for updating the current tab
			Win->Show( true );
			Win->GrabFocus();
			UpdateTimer.restart();
		}
	}
	else if ( e.Is( "DISPLAY_GRAPH", typeid( std::shared_ptr<sbe::GraphPlotter> ) ) )
	{
		auto p = boost::any_cast<std::shared_ptr<sbe::GraphPlotter>>( e.Data() );
		PlotGraph( p );
		updatePosition();
	}
	else if ( e.Is( "ADD_GRAPH_TO_BOOK", typeid( std::string ) ) )
	{
		AddNewGraph( boost::any_cast<std::string>( e.Data() ) );
	}
}

void GraphBook::AddNewGraph( std::string displayName )
{
	Box::Ptr box( Box::Create() );
	Label::Ptr label( Label::Create( displayName ) );
	label->GetSignal( Label::OnLeftClick ).Connect( &GraphBook::UpdateGraph, this );
	Tabs->AppendPage( box, label );
}

void GraphBook::UpdateGraph()
{
	//get current tab-label-string
	std::string currentTabName = DynamicPointerCast<Label>( Tabs->GetNthTabLabel( Tabs->GetCurrentPage() ) )->GetText();
	//make update
	Module::Get()->QueueEvent( Event( "PLOT_GRAPH", currentTabName ), true );
}

void GraphBook::PlotGraph ( std::shared_ptr<sbe::GraphPlotter>& G )
{
	if ( !G ) {
		Engine::out() << "[GraphBook::PlotGraph] INVALID POINTER!" << std::endl;
		return;
	}

	sf::RenderTexture tex;
		tex.create( G->getGraph().Size.x, G->getGraph().Size.y );
		G->updateVertexArrays();
		G->draw( tex );
		tex.display();
	Image::Ptr I = Image::Create( tex.getTexture().copyToImage() );
	Box::Ptr box = DynamicPointerCast<Box>( Tabs->GetNthPage( Tabs->GetCurrentPage() ) );
	box->RemoveAll();
	box->Pack( I );
}

void GraphBook::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , ( winSize.y - Allocation.height ) ) );
}

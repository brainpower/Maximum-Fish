#include "GraphBook.hpp"


GraphBook::GraphBook( const Geom::Vec2 Size)

{
    RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "TOGGLE_SHOW_GRAPHBOOK" );
	RegisterForEvent( "DISPLAY_GRAPH" );
	RegisterForEvent( "ADD_GRAPH_TO_BOOK" );

    CreateWindow(Size);
    Win->Show(false);
}

void GraphBook::CreateWindow( const Geom::Vec2 Size )
{
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::RESIZE | sfg::Window::Style::SHADOW );

    Win->SetRequisition( sf::Vector2f(Size.x, Size.y ) );
    updatePosition();

    Tabs = sfg::Notebook::Create();
    Tabs->SetTabPosition( sfg::Notebook::TabPosition::LEFT );
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
    else if ( e.Is( "DISPLAY_GRAPH", typeid( std::shared_ptr<GraphPlotter> ) ) )
    {
		auto p = boost::any_cast<std::shared_ptr<GraphPlotter>>( e.Data() );
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
	sfg::Box::Ptr box( sfg::Box::Create() );
	sfg::Label::Ptr label( sfg::Label::Create( displayName ) );
	label->GetSignal(   sfg::Label::OnLeftClick ).Connect( &GraphBook::UpdateGraph, this );
	Tabs->AppendPage( box, label );
}

void GraphBook::UpdateGraph()
{
	//request update for current tab

	//get current tab-label-string
	std::string currentTabName = sfg::DynamicPointerCast<sfg::Label>( Tabs->GetNthTabLabel( Tabs->GetCurrentPage() ) )->GetText();
   	//make update
	Module::Get()->QueueEvent( Event( "PLOT_GRAPH", currentTabName ) , true );
}

void GraphBook::PlotGraph ( std::shared_ptr<GraphPlotter>& G )
{
	if (!G) {
		Engine::out() << "[GraphBook::PlotGraph] INVALID POINTER!" << std::endl;
		return;
	}

	sf::RenderTexture tex;
		tex.create(G->getGraph().Size.x, G->getGraph().Size.y);
		G->updateVertexArrays();
		G->draw( tex );
		tex.display();
	sfg::Image::Ptr I = sfg::Image::Create( tex.getTexture().copyToImage() );
	sfg::Box::Ptr box = sfg::DynamicPointerCast<sfg::Box>( Tabs->GetNthPage( Tabs->GetCurrentPage() ) );
	box->RemoveAll();
	box->Pack(I);
}

void GraphBook::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , ( winSize.y - Allocation.height ) ) );
}

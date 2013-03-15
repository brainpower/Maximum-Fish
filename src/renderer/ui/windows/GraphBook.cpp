#include "GraphBook.hpp"


GraphBook::GraphBook( const Geom::Vec2 Size)

{
    RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "KEY_SHOW_GRAPHBOOK" );
	RegisterForEvent( "DISPLAY_COUNT_GRAPH" );

    CreateWindow(Size);
    Win->Show(false);
}

void GraphBook::CreateWindow( const Geom::Vec2 Size )
{
    Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::RESIZE | sfg::Window::Style::SHADOW );

    //@TODO: create graphs

    Win->SetRequisition( sf::Vector2f(Size.x, Size.y ) );
    updatePosition();


    sfg::Notebook::Ptr tabs( sfg::Notebook::Create() );
    tabs->SetTabPosition( sfg::Notebook::TabPosition::LEFT );
    {
		sfg::Box::Ptr box( sfg::Box::Create() );
		sfg::Label::Ptr label( sfg::Label::Create( "Population" ) );
		label->GetSignal(   sfg::Label::OnLeftClick ).Connect( &GraphBook::TabsChangeClick, this );
	tabs->AppendPage( box, label );
    }{
		sfg::Box::Ptr box( sfg::Box::Create() );
		sfg::Label::Ptr label( sfg::Label::Create( "Births" ) );
		label->GetSignal(   sfg::Label::OnLeftClick ).Connect( &GraphBook::TabsChangeClick, this );
	tabs->AppendPage( box, label );
    }{
		sfg::Box::Ptr box( sfg::Box::Create() );
		sfg::Label::Ptr label( sfg::Label::Create( "Deaths" ) );
		label->GetSignal(   sfg::Label::OnLeftClick ).Connect( &GraphBook::TabsChangeClick, this );
	tabs->AppendPage( box, label );
    }
	//Don't forget to connect the function TabsChangeClick with every label.

    Win->Add( tabs );

    Event e("SCREEN_ADD_WINDOW");
    e.SetData( Win );
    Module::Get()->QueueEvent( e );
}

void GraphBook::HandleEvent( Event& e)
{
    if (e.Is("KEY_SHOW_GRAPHBOOK"))
    {
        if (Win->IsGloballyVisible())
		{
			Win->Show(false);
		}
        else
		{
			updatePosition();
			TabsChangeClick();//not realy clicked, but for updating the current tab
			Win->Show( true );
			Win->GrabFocus();
		}
    }
    else if ( e.Is( "DISPLAY_COUNT_GRAPH" ) )
    {
		auto p = boost::any_cast<std::shared_ptr<GraphPlotter>>( e.Data() );
		PlotGraph( p );
		updatePosition();
    }
}

void GraphBook::TabsChangeClick ()
{
	//request update for current tab

	//get current tab-label-string
	sf::String currentTab = sfg::DynamicPointerCast<sfg::Label>( sfg::DynamicPointerCast<sfg::Notebook>( Win->GetChild() )->GetNthTabLabel( sfg::DynamicPointerCast<sfg::Notebook>( Win->GetChild() )->GetCurrentPage() ) )->GetText();
   	//make update
   	if ( currentTab == "Population" )
	{
		Module::Get()->QueueEvent( "PLOT_COUNTS" , true );
	}
	else if ( currentTab == "Births" )
	{
		Module::Get()->QueueEvent( "PLOT_BIRTHS", true );
	}
	else if ( currentTab == "Deaths" )
	{
		Module::Get()->QueueEvent( "PLOT_DEATHS", true );
	}
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
	sfg::Box::Ptr box = sfg::DynamicPointerCast<sfg::Box>( sfg::DynamicPointerCast<sfg::Notebook>( Win->GetChild() )->GetNthPage( sfg::DynamicPointerCast<sfg::Notebook>( Win->GetChild() )->GetCurrentPage() ) );
	box->RemoveAll();
	box->Pack(I);
}

void GraphBook::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width ) , ( winSize.y - Allocation.height ) ) );
}

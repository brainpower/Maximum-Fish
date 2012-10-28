#include "DebugWindow.hpp"



DebugWindow::DebugWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
	RegisterForEvent( "VIEW_DBG_STRING" );
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "TOGGLE_SHOW_CONSOLE" );

	currentlabeltext = 0;
	CreateWindow(RelativePosition, Size);
}

void DebugWindow::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
	Win = sfg::Window::Create();

	DbgText = sfg::Label::Create();
	DbgText->SetAlignment( sf::Vector2f(0.f, 0.f) );

	DbgLabels = sfg::Label::Create();
	DbgLabels->SetAlignment( sf::Vector2f(0.f, 0.f) );

	LogText = sfg::Label::Create();

	Win->SetPosition( sf::Vector2f(RelativePosition.x(), RelativePosition.y() ) );
	//Win->SetRequisition( sf::Vector2f(Size.x(), Size.y() ) );

	// main box, horizontal
	sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ) );
		sfg::Box::Ptr boxInfo( sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ) );
		boxInfo->Pack( DbgLabels, false, false);
		boxInfo->Pack( DbgText, false, false);
	box->Pack(boxInfo, false, false);

		LogBox = sfg::Box::Create( sfg::Box::VERTICAL );
		LogBox->Pack(LogText, true, true);

		scrolledwindow = sfg::ScrolledWindow::Create();
		scrolledwindow->AddWithViewport( LogBox );
		scrolledwindow->SetRequisition( sf::Vector2f( 400.f, 100.f ) );
		scrolledwindow->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC );
	box->Pack ( scrolledwindow );

	// Create a window and add the box layouter to it. Also set the window's title.

	Win->SetTitle( "Debug Information [F3]" );
	Win->Add( box );

	Event e("SCREEN_ADD_WINDOW");
	e.SetData( Win );
	Module::Get()->QueueEvent( e );
}

void DebugWindow::HandleEvent( Event& e)
{
	if (e.Is("VIEW_DBG_STRING"))
	{
		//Engine::out() << "[DebugWindow] DebugString Event" << std::endl;

		if (e.Data().type() == typeid( std::pair< std::string, std::string> ))
		{
			std::pair< std::string, std::string> D = boost::any_cast<  std::pair< std::string, std::string>  >(e.Data());
			DebugStrings[D.first] = D.second;

		}
		else
		{
			Engine::out() << "[DebugWindow] DebugString Event with wrong parameters" << std::endl;
		}
	}
	else if (e.Is("EVT_FRAME"))
	{
		UpdateText();
	}
	else if (e.Is("TOGGLE_SHOW_CONSOLE"))
    {
        if (Win->IsGloballyVisible())
            Win->Show(false);
        else if (!Win->IsGloballyVisible())
            Win->Show(true);
    }
}

void DebugWindow::UpdateText()
{
	std::string values;
	std::string labels;
	for ( auto dbgString : DebugStrings )
	{
		labels += dbgString.first  + "\n";
		values += dbgString.second + "\n";
	}

	//Engine::out() << " Text: \n" << text << std::endl;

	DbgLabels->SetText( labels );
	DbgText->SetText( values );

	std::string newtext =  Engine::GetLogger()->GetLog();
	if (!newtext.empty())
	{
		LogText->SetText( LogText->GetText()+ newtext);
		currentlabeltext += newtext.size();
		Engine::GetLogger()->ClearCache();

		if (currentlabeltext > 1000)
		{
			//Engine::out() << " Adding one more label.. " << std::endl;
			LogText = sfg::Label::Create();
			LogText->SetAlignment( sf::Vector2f(0.f, 0.f) );
			LogBox->Pack(LogText, true, true);
			currentlabeltext = 0;
		}

		scrolledwindow->GetVerticalAdjustment()->SetValue( scrolledwindow->GetVerticalAdjustment()->GetUpper() );
	}

}

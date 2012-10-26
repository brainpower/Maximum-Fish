#include "DebugWindow.hpp"



DebugWindow::DebugWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size)
{
	RegisterForEvent( "VIEW_DBG_STRING" );

	CreateWindow(RelativePosition, Size);
}

void DebugWindow::CreateWindow( const Geom::Point& RelativePosition, const Geom::Vec2 Size )
{
	Win = sfg::Window::Create();
	Text = sfg::Label::Create();

	Win->SetPosition( sf::Vector2f(RelativePosition.x(), RelativePosition.y() ) );
	Win->SetRequisition( sf::Vector2f(Size.x(), Size.y() ) );

	// Create a vertical box layouter with 5 pixels spacing and add the label
	// and button to it.
	sfg::Box::Ptr box( sfg::Box::Create( sfg::Box::VERTICAL, 5.0f ) );
	box->Pack( Text );

	// Create a window and add the box layouter to it. Also set the window's title.

	Win->SetTitle( "Debug Information" );
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

			UpdateText();
		}
		else
		{
			Engine::out() << "[DebugWindow] DebugString Event with wrong parameters" << std::endl;
		}
	}
}

void DebugWindow::UpdateText()
{
	std::string text = "Debug Information: \n";
	text += "------------------------------------\n";
	for ( auto dbgString : DebugStrings )
	{
		text += "\t" + dbgString.first + "  =  " + dbgString.second + "\n";
	}
	text += "------------------------------------";

	//Engine::out() << " Text: \n" << text << std::endl;

	Text->SetText( text );
}


#include "FileBrowse.hpp"

#include <sbe/Engine.hpp>
#include <sbe/Config.hpp>

#include "SFGUI/Button.hpp"
#include "SFGUI/Entry.hpp"
#include "SFGUI/Box.hpp"
#include "sbe/sfg/List.hpp"

FileBrowse::FileBrowse( const Geom::Vec2 Size, const std::string &title,
                        const std::string &okEvent, const std::string &cancelEvent )
   : okEvt(okEvent), cancelEvt(cancelEvent) {
	RegisterForEvent( "EVT_FRAME" );
	RegisterForEvent( "FB_SEL_CHANGED" );
	//RegisterForEvent();

	CreateWindow( Size, title );
	Win->Show( false );
	Module::Get()->QueueEvent( Event("SCREEN_ADD_WINDOW", Win) );
}

void FileBrowse::CreateWindow( const Geom::Vec2 Size, const std::string &title){
	Win = sfg::Window::Create( sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR | sfg::Window::Style::SHADOW );

	fldrLst = std::make_shared<sbe::sfgList>("FB_SEL_CHANGED");
	sfg::Box::Ptr    box( sfg::Box::Create(sfg::Box::VERTICAL, 3.0f) );
	sfg::Box::Ptr    btnBox( sfg::Box::Create(sfg::Box::HORIZONTAL, 3.0f) );
	sfg::Box::Ptr    emptyBox( sfg::Box::Create(sfg::Box::HORIZONTAL, 3.0f) );
	sfg::Button::Ptr btnOK( sfg::Button::Create("OK") );
	sfg::Button::Ptr btnCancel( sfg::Button::Create("Cancel") );
	etyLoc = sfg::Entry::Create();
	etyLoc->SetText(Engine::getCfg()->get<std::string>("sim.debugsavepath"));

	btnOK->GetSignal( sfg::Button::OnLeftClick ).Connect( &FileBrowse::onOkClicked, this);
	btnCancel->GetSignal( sfg::Button::OnLeftClick ).Connect( &FileBrowse::onCancelClicked, this);

	Win->SetRequisition( sf::Vector2f(Size.x,Size.y) );
	emptyBox->SetRequisition( { 20, 1} );
	fldrLst->getList()->SetRequisition( { 20, 300} );
	updatePosition();

	btnBox->Pack( emptyBox,  true, true );
	btnBox->Pack( btnCancel, false, false );
	btnBox->Pack( btnOK,     false, false );
	box->Pack( etyLoc, false, false );
	box->Pack( fldrLst->getList(),   false, true );
	box->Pack( btnBox, false, false );

	Win->SetTitle(title);
	Win->Add(box);

	updateFolderList(etyLoc->GetText());
}

void FileBrowse::updatePosition()
{
	sf::FloatRect Allocation = Win->GetAllocation();
	sf::Vector2u winSize =  Engine::GetApp().getSize();
	Win->SetPosition( sf::Vector2f( ( winSize.x - Allocation.width )/2 , ( winSize.y - Allocation.height )/2   ) );
}

void FileBrowse::onOkClicked(){
	Module::Get()->QueueEvent( Event(okEvt, std::string(etyLoc->GetText())), true );
	hide();
}

void FileBrowse::onCancelClicked(){
	Module::Get()->QueueEvent( Event(cancelEvt) );
	hide();
}

void FileBrowse::HandleEvent( Event& e )
{
	if( e.Is("FB_SEL_CHANGED", typeid(std::string)) )
		updateFolderList(boost::any_cast<std::string>(e.Data()));
}

void FileBrowse::updateFolderList(const std::string &p){
	Engine::out(Engine::SPAM) << "[fb] lets fuck up lirrec's list using »" << p << "«!" << std::endl;
	using boost::filesystem::path;
	using boost::filesystem::directory_iterator;
	// make list empty
	fldrLst->clear();

	path bp;

	if(p == ".."){
		// path = parent of lastpath
		bp = lastPath.parent_path();
	} else {
		bp = path(p);
		if(!bp.is_absolute()){
			bp = lastPath / bp;
		}
	}
	Engine::out() << "[fb] new path is »" << bp.string() << "«"<< std::endl;

	if(exists(bp) && is_directory(bp)){
		lastPath = bp;
		fldrLst->addItem(".."); // always provide a way back up
		etyLoc->SetText(bp.string());

		// read folder content
		for(auto dit=directory_iterator(bp); dit != directory_iterator(); ++dit){
			// if isDir, addItem
			if(is_directory(dit->path())){
				Engine::out(Engine::SPAM) << "[fb] lets fuck up lirrec's list by adding »" << dit->path().string() << "«!" << std::endl;
				fldrLst->addItem(dit->path().filename().string());
			}
		}

	} else {
		Engine::out() << "[fb] " << bp.string() << " exists: " << exists(bp) << ", is_dir: "<< is_directory(bp) << std::endl;
	}
}

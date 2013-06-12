#ifndef FILE_BROWSE_HPP
#define FILE_BROWSE_HPP

#include "sbe/event/EventUser.hpp"

#include "sbe/geom/Point.hpp"

#include <SFGUI/SharedPtr.hpp>
#include "SFGUI/Window.hpp"

#include <boost/filesystem.hpp>

namespace sfg{ class Entry; }
namespace sbe{ class sfgList; }
/// FileBrowse
class FileBrowse : public sbe::EventUser
{
	public:
		FileBrowse( const Geom::Vec2 Size = Geom::Vec2(600, 400), const std::string &title="",
		            const std::string &okEvent="FB_OK", const std::string &cancelEvent="FB_CANCEL");
		virtual ~FileBrowse() = default;

		virtual void HandleEvent( Event& e);

		void setTitle(const std::string &t){ Win->SetTitle(t); }
		void setOkEvt(const std::string &e){ okEvt = e; }
		void show() { updatePosition(); Win->Show(true); Win->GrabFocus(); Engine::out() << "[fb] show" << std::endl;}
		void hide() { Win->Show(false); Engine::out() << "[fb] hide" << std::endl;}

	private:
		void CreateWindow(const Geom::Vec2 Size, const std::string &title);
		void onOkClicked();
		void onCancelClicked();
		void updatePosition();
		void updateFolderList(const std::string &path);

		sfg::SharedPtr<sfg::Window> Win;
		std::string okEvt;
		std::string cancelEvt;
		sfg::SharedPtr<sfg::Entry> etyLoc;


		std::shared_ptr<sbe::sfgList> fldrLst;
		boost::filesystem::path lastPath;
};


#endif //FILE_BROWSE_HPP

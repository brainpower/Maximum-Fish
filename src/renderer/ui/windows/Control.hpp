#ifndef CONTROL_H
#define CONTROL_H

#include "sbe/event/EventUser.hpp"

#include "sbe/geom/Point.hpp"

#include <SFGUI/SharedPtr.hpp>
namespace sfg
{
	class ToggleButton;
	class Button;
	class Entry;
	class Window;
}

/// Controls for the simulator ( play,pause,reset, settps etc )
class Control : public sbe::EventUser
{
	public:

		Control( const Geom::Vec2 Size = Geom::Vec2( 250, 30 ) );
		virtual ~Control() {}

		virtual void HandleEvent( Event& e );

	private:
		void CreateWindow( const Geom::Vec2 Size );
		sfg::SharedPtr<sfg::ToggleButton> BtnDbgWin;
        sfg::SharedPtr<sfg::ToggleButton> BtnIPanWin;
        sfg::SharedPtr<sfg::ToggleButton> BtnMnMnWin;
        sfg::SharedPtr<sfg::ToggleButton> BtnGraBoWin;
        sfg::SharedPtr<sfg::ToggleButton> BtnSimPause;
        sfg::SharedPtr<sfg::Button> BtnSimReset;
        sfg::SharedPtr<sfg::Button> BtnSimSingleFrame;
        sfg::SharedPtr<sfg::Button> BtnSimFrames;
        sfg::SharedPtr<sfg::Entry> Framesdisplay;
        unsigned int Frames;
        void BtnFramesUpClick();
        void BtnFramesDownClick();

        void BtnDbgWinClick();
        void BtnIPanWinClick();
        void BtnMnMnWinClick();
        void BtnGraBoWinClick();
        void BtnSimPauseClick();
        void BtnSimResetClick();
        void BtnSimSingleFrameClick();
        void BtnSimFramesClick();

        /// This updates the position of the ControlMenu to the bottom left corner of the application.
        /// Its called whenever a change of Application-size occures.
		void updatePosition();

        void SimPause( bool _p );

		unsigned int simPauseConnectionSerial;
		bool simPauseLock;
		int simPauseLockLevel;

		sfg::SharedPtr<sfg::Window> Win;

};

#endif // CONTROL_H


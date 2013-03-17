#ifndef CONTROL_H
#define CONTROL_H

#include "sbe/event/EventUser.hpp"

#include "sbe/geom/Point.hpp"

#include <SFGUI/SFGUI.hpp>

/// Controls for the simulator ( play,pause,reset, settps etc )
class Control : public sbe::EventUser
{
	public:

		Control( const Geom::Vec2 Size = Geom::Vec2(250, 30));
		virtual ~Control() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Vec2 Size);
		sfg::ToggleButton::Ptr BtnDbgWin;
        sfg::ToggleButton::Ptr BtnIPanWin;
        sfg::ToggleButton::Ptr BtnMnMnWin;
        sfg::ToggleButton::Ptr BtnGraBoWin;
        sfg::ToggleButton::Ptr BtnSimPause;
        sfg::Button::Ptr BtnSimReset;
        sfg::Entry::Ptr Framesdisplay;
        unsigned int Frames;
        void BtnFramesUpClick();
        void BtnFramesDownClick();

        void BtnDbgWinClick();
        void BtnIPanWinClick();
        void BtnMnMnWinClick();
        void BtnGraBoWinClick();
        void BtnSimPauseClick();
        void BtnSimResetClick();

        /// This updates the position of the ControlMenu to the bottom left corner of the application.
        /// Its called whenever a change of Application-size occures.
		void updatePosition();

        void SimPause(bool _p);

		unsigned int simPauseConnectionSerial;
		bool simPauseLock;
		int simPauseLockLevel;

		sfg::Window::Ptr Win;

};

#endif // CONTROL_H


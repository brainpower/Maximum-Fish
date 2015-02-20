#ifndef CONTROL_H
#define CONTROL_H

#include "sbe/event/EventUser.hpp"

#include "sbe/geom/Point.hpp"


namespace sfg
{
	class ToggleButton;
	class Button;
	class Entry;
	class Window;
	class Scale;
}

/// Controls for the simulator ( play,pause,reset, settps etc )
class Control : public sbe::EventUser
{
	public:

		Control( const glm::ivec2 Size = glm::ivec2( 250, 30 ) );
		virtual ~Control() {}

		virtual void HandleEvent( Event& e );

	private:
		void InitWindow( const glm::ivec2 Size );
		std::shared_ptr<sfg::ToggleButton> BtnDbgWin;
        std::shared_ptr<sfg::ToggleButton> BtnIPanWin;
        std::shared_ptr<sfg::ToggleButton> BtnMnMnWin;
        std::shared_ptr<sfg::ToggleButton> BtnGraBoWin;
        std::shared_ptr<sfg::ToggleButton> BtnSimPause;
        std::shared_ptr<sfg::Button> BtnSimReset;
        std::shared_ptr<sfg::Button> BtnSimSingleFrame;
        std::shared_ptr<sfg::Button> BtnSimFrames;
        std::shared_ptr<sfg::Entry> Framesdisplay;
        std::shared_ptr<sfg::Scale> SclTickSlider;
        std::shared_ptr<sfg::Entry> TickScaleDisplay;
        unsigned int Frames;
        void BtnFramesUpClick();
		void BtnFramesDownClick();
		void EntryGainFocus();
		void EntryLostFocus();
		void EntryTextChange();

		void BtnDbgWinClick();
		void BtnIPanWinClick();
		void BtnMnMnWinClick();
		void BtnGraBoWinClick();
		void BtnSimPauseClick();
		void BtnSimResetClick();
		void BtnSimSingleFrameClick();
		void BtnSimFramesClick();
		void TickSliderReleased();
		void TickSliderLabelUpdate();

		/// This updates the position of the ControlMenu to the bottom left corner of the application.
		/// Its called whenever a change of Application-size occures.
		void updatePosition();

		void SimPause( bool up );

		unsigned int simPauseConnectionSerial;
		int simPauseLockLevel;
		bool ListenToActionKeys;
		bool textchangeavoidrecursive;
		int ChangeframesonButtons;
		void CalculateNewFrames();

		std::shared_ptr<sfg::Window> Win;

};

#endif // CONTROL_H

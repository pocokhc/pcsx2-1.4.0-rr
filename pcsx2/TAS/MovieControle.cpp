#include "PrecompiledHeader.h"

#include "App.h"	// use "CoreThread"
#include "Counters.h"	// use"g_FrameCount"

#include "MovieControle.h"


MovieControle g_MovieControle;


//-----------------------------------------------
// ゲームはCoreThreadで動いており、動いている間はGSFrame(wxFrame)の処理を受け付けない
// (キー入力の受け付けもCoreThread内とGSFrame内の2箇所が設定されてるっぽい)
// CoreThreadが停止している間はwxFrameの入力が動く仕組みっぽい
//-----------------------------------------------

bool MovieControle::isStop()
{
	return (fPauseState && CoreThread.IsOpen() && CoreThread.IsPaused());
}
//-----------------------------------------------
// Counters(CoreThread)内の停止判定用
//-----------------------------------------------
void MovieControle::StartCheck()
{
	if (fStart && CoreThread.IsOpen() && CoreThread.IsPaused()) {
		CoreThread.Resume();
		fStart = false;
		fPauseState = false;
	}
}

void MovieControle::StopCheck()
{
	if (fFrameAdvance) {
		if (stopFrameCount < g_FrameCount) {
			fFrameAdvance = false;
			fStop = true;
		}
	}
	if (fStop && CoreThread.IsOpen() && CoreThread.IsRunning()) {
		CoreThread.PauseSelf();	//selfじゃないと止まらない
		stopFrameCount = g_FrameCount;
		fPauseState = true;
	}
}



//----------------------------------
// shortcut key
//----------------------------------
void MovieControle::FrameAdvance()
{
	fFrameAdvance = true;
	fStop = false;
	fStart = true;
}
void MovieControle::TogglePause()
{
	fStop = !fStop;
	if (fStop == false) {
		fStart = true;
	}
}
void MovieControle::Pause()
{
	fStop = true;
	fFrameAdvance = false;
}
void MovieControle::UnPause()
{
	fStop = false;
	fStart = true;
	fFrameAdvance = false;
}








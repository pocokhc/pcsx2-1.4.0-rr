#include "PrecompiledHeader.h"

#include "app.h"	//Counters.hをincludeするのに必要
#include "Counters.h"	// use"g_FrameCount"
#include "SaveState.h"	// create "SaveStateBase::keymovieFreeze()"
#include "AppSaveStates.h"	// use "States_GetCurrentSlot()"

#include "TAS/MovieControle.h"
#include "KeyMovie.h"

#include <vector>


KeyMovie g_KeyMovie;

//-----------------------------------------------
// save or loadでフレーム数の保存
//-----------------------------------------------
void SaveStateBase::keymovieFreeze()
{
	FreezeTag("keymovie");
	Freeze(g_FrameCount);	//Freeze関数でなぜかframeの保存がうまくいった

	if (IsLoading()) {
		g_KeyMovieData.addUndoCount();
	}
}

//----------------------------------
// key interrupt
//----------------------------------
void KeyMovie::ControllerInterrupt(u8 &data, u8 &port, u16 & BufCount, u8 buf[])
{
	if (state == NONE)return;
	if (port < 0 || 1 < port )return;
	if (BufCount < 1 || 8 < BufCount)return;

	//==========================
	// キー入力フレームの確認
	// BufCount 1 && data == 0x42,
	// BufCount 2 && buf[2] == 0x5A
	// の時のみ入力
	// --- LilyPad.cpp(1192) ---
	//// READ_DATA_AND_VIBRATE
	//	case 0x42:
	//		query.response[2] = 0x5A;
	//==========================
	if (BufCount == 1) {
		if (data == 0x42)
		{
			fInterruptFrame = true;
		}
		else {
			fInterruptFrame = false;
		}
	}
	else if ( BufCount == 2 ){
		if (buf[BufCount] != 0x5A) {
			fInterruptFrame = false;
		}
	}
	if (!fInterruptFrame)return;

	int bufIndex = BufCount - 3;
	if (bufIndex < 0 || 6 < bufIndex)return;

	//---------------
	// read/write
	//---------------
	const u8 &nowBuf = buf[BufCount];
	if (state == RECORD)
	{
		keyMovieData.updateFrameMax(g_FrameCount);
		keyMovieData.writeKeyBuf(g_FrameCount, port, bufIndex, nowBuf);
	}
	else if (state == REPLAY)
	{
		if (keyMovieData.getMaxFrame() < g_FrameCount)
		{
			// end replay
			g_MovieControle.Pause();
			Stop();
			return;
		}
		u8 tmp = 0;
		if (keyMovieData.readKeyBuf(tmp, g_FrameCount, port, bufIndex)) {
			buf[BufCount] = tmp;
		}
	}
}


//----------------------------------
// stop
//----------------------------------
void KeyMovie::Stop() {
	state = NONE;
	if (keyMovieData.Close()) {
		Console.WriteLn(Color_StrongBlue, L"[KeyMovie]KeyMovie stop.");
	}
}

//----------------------------------
// start
//----------------------------------
void KeyMovie::Start(wxString FileName,bool fReadOnly)
{
	g_MovieControle.Pause();
	Stop();

	if (fReadOnly)
	{
		if (!keyMovieData.Open(FileName, false)) {
			return;
		}
		if (!keyMovieData.readHeaderAndCheck()) {
			Console.WriteLn(Color_StrongBlue, L"[KeyMovie]This file is not KeyMovie file.");
			keyMovieData.Close();
			return;
		}
		// cdrom
		if (!g_Conf->CurrentIso.IsEmpty())
		{
			if (Path::GetFilename(g_Conf->CurrentIso) != keyMovieData.getHeader().cdrom) {
				Console.WriteLn(Color_StrongBlue, L"[KeyMovie]Information on CD in Movie file is different.");
			}
		}
		state = REPLAY;
		Console.WriteLn(Color_StrongBlue, wxString::Format( L"[KeyMovie]Replay movie.[%s]",FileName) );
		Console.WriteLn(Color_StrongBlue, L"MaxFrame:%d", keyMovieData.getMaxFrame());
		Console.WriteLn(Color_StrongBlue, L"UndoCount:%d", keyMovieData.getUndoCount());
	}
	else
	{
		// backup
		wxString bpfile = wxString::Format(L"%s_backup", FileName);
		if (CopyFile( FileName , bpfile, false)) {
			Console.WriteLn(Color_StrongBlue, wxString::Format(L"[KeyMovie]Create backup file.[%s]", bpfile) );
		}
		// create
		if (!keyMovieData.Open(FileName, true)) {
			return;
		}
		// cdrom
		if (!g_Conf->CurrentIso.IsEmpty())
		{
			keyMovieData.getHeader().setCdrom(Path::GetFilename(g_Conf->CurrentIso));
		}
		keyMovieData.writeHeader();

		state = RECORD;
		Console.WriteLn(Color_StrongBlue, wxString::Format(L"[KeyMovie]Start new record.[%s]",FileName ));
	}
}


//----------------------------------
// shortcut key
//----------------------------------
void KeyMovie::RecordModeToggle()
{
	if (state == REPLAY) {
		state = RECORD;
		Console.WriteLn(Color_StrongBlue, L"[KeyMovie]Record mode on.");
	}
	else if (state == RECORD) {
		state = REPLAY;
		Console.WriteLn(Color_StrongBlue, L"[KeyMovie]Replay mode on.");
	}
}



#include "PrecompiledHeader.h"

#include "app.h"	//Counters.hをincludeするのに必要
#include "Counters.h"	// use"g_FrameCount"
#include "SaveState.h"	// create "SaveStateBase::TAS_movieFreeze()"

#include "TAS/MovieControle.h"
#include "KeyMovie.h"


//----------------------------------
// KeyMovieRecord
//----------------------------------
struct KeyMovieHeader{
	char version = 1;
	u32   FrameMax;
	u32   Rerecs;
	const char HEAD_TAIL = 0xFE;
};
static KeyMovieHeader key_movie_header;
enum KEY_MOVIE_MODE{
	NONE,
	RECORD,
	REPLAY,
};
static KEY_MOVIE_MODE key_movie_mode = NONE;
static FILE* key_movie_fp = NULL;
static bool fRecordFrame = false;



//-----------------------------------------------
// save or loadでフレーム数の保存
//-----------------------------------------------
void SaveStateBase::TAS_movieFreeze()
{
	FreezeTag("movie");
	Freeze(g_FrameCount);	//Freeze関数でなぜかframeの保存がうまくいった

	if (IsLoading()) {
		key_movie_header.Rerecs++;
		if (key_movie_fp != NULL) {
			rewind(key_movie_fp);
			fwrite(&key_movie_header, sizeof(key_movie_header), 1, key_movie_fp);
		}
	}
}



//----------------------------------
// key interrupt
//----------------------------------
void KeyMovie_ControllerInterrupt(u8 &data, u8 &port, u16 & BufCount, u8 buf[])
{
	if (key_movie_fp == NULL)return;

	//--------------------------
	// キー入力フレームの確認
	// BufCount 1の時
	// data == 0x42 //READ_DATA_AND_VIBRATE
	//--------------------------
	if (BufCount == 1) {
		if (data == 0x42) {
			fRecordFrame = true;
		}
		else {
			fRecordFrame = false;
		}
	}
	if (!fRecordFrame)return;

	//-----------------------------
	// seekの位置を出す
	// port range = 0 - 1 (1bit)
	// BufCount range = 0 - 20 (5bit)
	// BufCountが何者かは不明、多分1～20の値しかとらないはず…
	//-----------------------------
	long seekpoint = g_FrameCount;
	seekpoint <<= (1 + 5);
	seekpoint += port;
	seekpoint <<= 5;
	seekpoint += BufCount;
	seekpoint += sizeof(key_movie_header);	//add header

	u8 nowBuf = buf[BufCount];
	if (key_movie_mode == RECORD)
	{
		//update header
		if (key_movie_header.FrameMax < g_FrameCount) {
			key_movie_header.FrameMax = g_FrameCount;
			rewind(key_movie_fp);
			fwrite(&key_movie_header, sizeof(key_movie_header), 1, key_movie_fp);
		}
		// write
		fseek(key_movie_fp, seekpoint, SEEK_SET);
		fwrite(&nowBuf, 1, 1, key_movie_fp);
	}
	else if (key_movie_mode == REPLAY)
	{
		if (key_movie_header.FrameMax < g_FrameCount) {
			Movie_Pause();
			return;
		}

		u8 tmp = 0;
		fseek(key_movie_fp, seekpoint, SEEK_SET);
		fread(&tmp, 1, 1, key_movie_fp);
		// BufCountが1の時データが違うとうまくいかなかったので
		// そのフレームの取得をやめる。
		if (BufCount == 1) {
			if (buf[BufCount] != tmp) {
				fRecordFrame = false;
			}
		}
		else {
			buf[BufCount] = tmp;
		}

		// debug
		if (tmp != nowBuf) {
			//Console.WriteLn(Color_StrongBlue, "F=%d data=%d port=%d bufCount=%d %d -> %d 0x%x", FrameCount, data, port, BufCount, nowBuf,tmp, seekpoint);
		}
	}
}




//----------------------------------
// stop
//----------------------------------
void KeyMovie_Stop() {
	key_movie_mode = NONE;
	if (key_movie_fp != NULL) {
		fclose(key_movie_fp);
		key_movie_fp = NULL;
		Console.WriteLn(Color_StrongBlue, "KeyMovie stop.");
	}
}

//----------------------------------
// start
//----------------------------------
void KeyMovie_Start(const char* FileName,bool fReadOnly)
{
	//key_movie_header = { 0 };
	if (fReadOnly)
	{
		fopen_s(&key_movie_fp, FileName, "rb+");
		if (key_movie_fp == NULL) {
			Console.WriteLn(Color_StrongBlue, "file open fail: %s", strerror(errno));
			return;
		}
		fread(&key_movie_header, sizeof(key_movie_header), 1, key_movie_fp);
		Console.WriteLn(Color_StrongBlue, "KeyMovie start. frame:%d rerecs:%d", key_movie_header.FrameMax,key_movie_header.Rerecs);
		Console.WriteLn(Color_StrongBlue, "Replay mode. To record, press the 'R' key.");
		key_movie_mode = REPLAY;
	}
	else
	{
		fopen_s(&key_movie_fp, FileName, "wb+");
		if (key_movie_fp == NULL) {
			Console.WriteLn(Color_StrongBlue, "file open fail: %s", strerror(errno));
			return;
		}
		fwrite(&key_movie_header, sizeof(key_movie_header), 1, key_movie_fp);
		key_movie_mode = RECORD;
		Console.WriteLn(Color_StrongBlue, "Record start.");
	}
}





//----------------------------------
// shortcut key
//----------------------------------
void KeyMovie_RecordModeOn()
{
	if (key_movie_mode == REPLAY) {
		key_movie_header.FrameMax = g_FrameCount;
		key_movie_mode = RECORD;
		Console.WriteLn(Color_StrongBlue, "Record mode change.");
	}
}





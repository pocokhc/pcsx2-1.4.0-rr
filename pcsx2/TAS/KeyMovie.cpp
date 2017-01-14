#include "PrecompiledHeader.h"

#include "app.h"	//Counters.hをincludeするのに必要
#include "Counters.h"	// use"g_FrameCount"
#include "SaveState.h"	// create "SaveStateBase::TAS_movieFreeze()"

#include "TAS/MovieControle.h"
#include "KeyMovie.h"

//----------------------------------
// header
//----------------------------------
struct KeyMovieHeader {
	unsigned char version = 1;
	unsigned char ID = 0xCC;//特に意味はなし、keyファイルか判定用のID

	char emu[50] = "PCSX2-1.4.0-rr";
	char auther[50]="";		//todo
	char RecordedFrom = 0;	//todo(power-on)
	char cdrom[50] = "";	//todo

};
static KeyMovieHeader header;
static u32   FrameMax;
static u32   UndoCount;
static FILE * key_movie_fp=NULL;
static u32 FRAMEMAX_SEEKPOINT = sizeof(KeyMovieHeader);
static u32 UNDOCOUNT_SEEKPOINT = sizeof(KeyMovieHeader)+4;
long getSeekPoint(u32 frame,u8 port,u8 buf) {
	return sizeof(header) + 4 + 4 + ((frame << 1) + port) * 20 + (buf - 1);
}

//----------------------------------
// state
//----------------------------------
enum KEY_MOVIE_MODE{
	NONE,
	RECORD,
	REPLAY,
};
static KEY_MOVIE_MODE key_movie_mode = NONE;

//----------------------------------
// val
//----------------------------------
static bool fRecordFrame = false;


//-----------------------------------------------
// save or loadでフレーム数の保存
//-----------------------------------------------
void SaveStateBase::keymovieFreeze()
{
	FreezeTag("keymovie");
	Freeze(g_FrameCount);	//Freeze関数でなぜかframeの保存がうまくいった

	if (IsLoading()) {
		UndoCount++;
		if (key_movie_fp != NULL) {
			fseek(key_movie_fp, UNDOCOUNT_SEEKPOINT, SEEK_SET);
			fwrite( &UndoCount, 4 , 1, key_movie_fp);
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
	// BufCount range = 1 - 20
	// BufCountが何者かは不明、多分1～20の値しかとらないはず…
	//-----------------------------
	long seekpoint = getSeekPoint(g_FrameCount,port,BufCount);

	u8 nowBuf = buf[BufCount];
	if (key_movie_mode == RECORD)
	{
		//update FrameMax
		if (FrameMax < g_FrameCount) {
			FrameMax = g_FrameCount;
			fseek(key_movie_fp, FRAMEMAX_SEEKPOINT, SEEK_SET);
			fwrite(&FrameMax, 4, 1, key_movie_fp);
		}
		// write
		fseek(key_movie_fp, seekpoint, SEEK_SET);
		fwrite(&nowBuf, 1, 1, key_movie_fp);
	}
	else if (key_movie_mode == REPLAY)
	{
		if (FrameMax < g_FrameCount) {
			Movie_Pause();
			KeyMovie_Stop();
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
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]KeyMovie stop.");
	}
}

//----------------------------------
// start
//----------------------------------
void KeyMovie_Start(wxString FileName,bool fReadOnly)
{
	Movie_Pause();
	if (fReadOnly)
	{
		fopen_s(&key_movie_fp, FileName, "rb+");
		if (key_movie_fp == NULL) {
			Console.WriteLn(Color_StrongBlue, "[KeyMovie]file open fail: %s", strerror(errno));
			return;
		}
		fread(&header, sizeof(header), 1, key_movie_fp);
		// ver check
		if (header.ID != 0xCC) {
			Console.WriteLn(Color_StrongBlue, "[KeyMovie]this file is no KeyMovie file.");
			fclose(key_movie_fp);
			key_movie_fp = NULL;
			return;
		}
		// read
		fseek(key_movie_fp, FRAMEMAX_SEEKPOINT, SEEK_SET);
		fread(&FrameMax, 4, 1, key_movie_fp);
		fseek(key_movie_fp, UNDOCOUNT_SEEKPOINT, SEEK_SET);
		fread(&UndoCount, 4, 1, key_movie_fp);

		key_movie_mode = REPLAY;
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]load succsess.");
		Console.WriteLn(Color_StrongBlue, "MaxFrame:%d", FrameMax);
		Console.WriteLn(Color_StrongBlue, "UndoCount:%d", UndoCount);
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]Replay mode now. To record, press the 'R' key.");
	}
	else
	{
		// backup
		wxString bpfile = wxString::Format("%s_backup", FileName);
		if (CopyFile(wxString(FileName), bpfile, true)) {
			Console.WriteLn(Color_StrongBlue, "[KeyMovie]create backup file.[%s]", bpfile);
		}

		// create
		fopen_s(&key_movie_fp, FileName, "wb+");
		if (key_movie_fp == NULL) {
			Console.WriteLn(Color_StrongBlue, "[KeyMovie]file open fail: %s ", strerror(errno));
			return;
		}
		fwrite(&header, sizeof(header), 1, key_movie_fp);
		key_movie_mode = RECORD;
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]Record start.");
	}
}

//----------------------------------
// convert p2m -> p2m2
// The p2m file is a file generated with the following URL.
// https://code.google.com/archive/p/pcsx2-rr/
//----------------------------------
void KeyMovie_ConvertP2M(wxString filename)
{
	FILE * fp;
	FILE * fp2;
	fopen_s(&fp, filename, "rb");
	if (fp == NULL) {
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]convert fail: %s ", strerror(errno));
		return;
	}
	wxString outfile= wxString::Format("%s.p2m2", filename);
	fopen_s(&fp2, outfile, "wb");
	if (fp2 == NULL) {
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]convert fail: %s ", strerror(errno));
		fclose(fp);
		return;
	}

	//--------------------------------------
	// pcsx2_rr の内容
	// fread(&g_Movie.FrameMax, 4, 1, g_Movie.File);
	// fread(&g_Movie.Rerecs, 4, 1, g_Movie.File);
	// fread(g_PadData[0]+2, 6, 1, g_Movie.File);
	//--------------------------------------
	//head read
	u32 frame;
	u32 undo;
	fread(&undo, 4, 1, fp);
	fread(&frame, 4, 1, fp);
	// head write
	KeyMovieHeader header;
	fwrite(&header, sizeof(header), 1, fp2);
	fwrite(&undo, 4, 1, fp2);
	fwrite(&frame, 4, 1, fp2);

	// frame
	for (u32 i = 0; i < frame; i++) {
		u8 tmp[6];
		fread( tmp, 6, 1, fp);

		for (int j = 2; j < 8; j++) {
			long seekpoint2 = getSeekPoint(i, 0, j); 
			if (fseek(fp2, seekpoint2, SEEK_SET) == 0) {
				fwrite(&tmp[j-2], 1, 1, fp2);
			}
		}
	}
	fclose(fp);
	fclose(fp2);
	Console.WriteLn(Color_StrongBlue, "[KeyMovie]convert success");
}

//----------------------------------
// ver 1.0~1.2 -> ver 1.3
//----------------------------------
void KeyMovie_ConvertOld(wxString filename)
{
	FILE * fp;
	FILE * fp2;
	fopen_s(&fp, filename, "rb");
	if (fp == NULL) {
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]convert fail: %s ", strerror(errno));
		return;
	}
	wxString outfile = wxString::Format("%s_new.p2m2", filename);
	fopen_s(&fp2, outfile, "wb");
	if (fp2 == NULL) {
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]convert fail: %s ", strerror(errno));
		fclose(fp);
		return;
	}
	//head read
	u32 frame;
	u32 undo;
	fread(&frame, 4, 1, fp);
	fread(&undo, 4, 1, fp);
	// head write
	KeyMovieHeader header;
	fwrite(&header, sizeof(header), 1, fp2);
	fwrite(&frame, 4, 1, fp2);
	fwrite(&undo, 4, 1, fp2);

	// frame
	for (u32 i = 0; i < frame; i++) {
		for (u32 port = 0; port < 2; port++) {
			for (u32 buf = 1; buf <= 20 ; buf++) {
				long seekpoint1 = i;
				seekpoint1 <<= (1 + 5);
				seekpoint1 += port;
				seekpoint1 <<= 5;
				seekpoint1 += buf;
				seekpoint1 += 8;
				long seekpoint2 = getSeekPoint(i, port, buf);
				u8 tmp = 0;
				fseek(fp,seekpoint1,SEEK_SET); fread(&tmp, 1, 1, fp);
				fseek(fp2, seekpoint2, SEEK_SET); fwrite(&tmp, 1, 1, fp2);
			}
		}
	}
	fclose(fp);
	fclose(fp2);
	Console.WriteLn(Color_StrongBlue, "[KeyMovie]convert success");
}


//----------------------------------
// shortcut key
//----------------------------------
void KeyMovie_RecordModeOn()
{
	if (key_movie_mode == REPLAY) {
		FrameMax = g_FrameCount;
		key_movie_mode = RECORD;
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]Record mode on.");
	}
}





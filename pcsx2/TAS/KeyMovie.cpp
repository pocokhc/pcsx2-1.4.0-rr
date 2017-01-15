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
		g_KeyMovie.addUndoCount();
	}
	g_KeyMovie.setSaveFrame(States_GetCurrentSlot(),g_FrameCount);

}
void KeyMovie::addUndoCount() {
	UndoCount++;
	writeUndoCount();
}

//----------------------------------
// key interrupt
//----------------------------------
void KeyMovie::ControllerInterrupt(u8 &data, u8 &port, u16 & BufCount, u8 buf[])
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
	if (state == RECORD)
	{
		//update FrameMax
		FrameMax = g_FrameCount;
		writeFrameMax();

		// write
		fseek(key_movie_fp, seekpoint, SEEK_SET);
		fwrite(&nowBuf, 1, 1, key_movie_fp);
	}
	else if (state == REPLAY)
	{
		if (FrameMax < g_FrameCount) {
			g_MovieControle.Pause();
			Stop();
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
void KeyMovie::Stop() {
	state = NONE;
	if (key_movie_fp != NULL) {
		fclose(key_movie_fp);
		key_movie_fp = NULL;
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]KeyMovie stop.");
	}
}

//----------------------------------
// start
//----------------------------------
void KeyMovie::Start(wxString FileName,bool fReadOnly)
{
	g_MovieControle.Pause();
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

		state = REPLAY;
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
		state = RECORD;
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]Record start.");
	}
}

//----------------------------------
// convert p2m -> p2m2
// The p2m file is a file generated with the following URL.
// https://code.google.com/archive/p/pcsx2-rr/
//----------------------------------
void KeyMovie::ConvertP2M(wxString filename)
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
	fread(&frame, 4, 1, fp);
	fread(&undo, 4, 1, fp);
	// head write
	KeyMovieHeader header;
	fwrite(&header, sizeof(header), 1, fp2);
	fwrite(&frame, 4, 1, fp2);
	fwrite(&undo, 4, 1, fp2);

	// frame
	for (u32 i = 0; i < frame; i++) {
		u8 tmp[6];
		fread( tmp, 6, 1, fp);
		{
			long seekpoint = getSeekPoint(i, 0, 1);
			fseek(fp2, seekpoint, SEEK_SET);
			u8 n = 121;
			fwrite(&n, 1, 1, fp2);
		}
		{
			long seekpoint = getSeekPoint(i, 0, 2);
			fseek(fp2, seekpoint, SEEK_SET);
			u8 n = 90;
			fwrite(&n, 1, 1, fp2);
		}
		for (int j = 2; j < 8; j++) {
			long seekpoint2 = getSeekPoint(i, 0, j+1); 
			fseek(fp2, seekpoint2, SEEK_SET);
			fwrite(&tmp[j-2], 1, 1, fp2);
		}
	}
	fclose(fp);
	fclose(fp2);
	Console.WriteLn(Color_StrongBlue, "[KeyMovie]convert success");
}

//----------------------------------
// ver 1.0~1.2 -> ver 1.3
//----------------------------------
void KeyMovie::ConvertOld(wxString filename)
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
void KeyMovie::RecordModeOn()
{
	if (state == REPLAY) {
		state = RECORD;
		Console.WriteLn(Color_StrongBlue, "[KeyMovie]Record mode on.");
	}
}


//----------------------------------
// public
//----------------------------------
void KeyMovie::getPadData(PadData & result_pad , u32 frame)
{
	if (key_movie_fp == NULL)return;
	result_pad.frame = frame;
	for(int port=0;port<2;port++){
		for (int i = 1; i <= 20; i++) {
			long seekpoint = getSeekPoint(frame, port, i);
			fseek(key_movie_fp, seekpoint, SEEK_SET);
			u8 tmp = 0;
			fread(&tmp, 1, 1, key_movie_fp);
			result_pad.buf[port][i - 1] = tmp;
		}
	}
}


//----------------------------------
// 操作関係
//----------------------------------
bool KeyMovie::DeleteKeyFrame(u32 frame)
{
	if (key_movie_fp == NULL)return false;
	if (state != RECORD)return false;

	for (u32 i = frame; i < FrameMax-1; i++) {
		for (int port = 0; port < 2; port++) {
			for (int buf = 1; buf <= 20; buf++) {
				long seek1 = getSeekPoint(i+1, port, buf);
				long seek2 = getSeekPoint(i, port, buf);
				u8 tmp = 0;
				fseek(key_movie_fp, seek1, SEEK_SET);
				fread(&tmp, 1, 1, key_movie_fp);
				fseek(key_movie_fp, seek2, SEEK_SET);
				fwrite(&tmp, 1, 1, key_movie_fp);
			}
		}
	}
	FrameMax--;
	return true;
}
bool KeyMovie::InsertKeyFrame(const PadData& key)
{
	if (key_movie_fp == NULL)return false;
	if (state != RECORD)return false;

	for (u32 i = FrameMax - 1 ; i >= key.frame; i--) {
		for (int port = 0; port < 2; port++) {
			for (int buf = 1; buf <= 20; buf++) {
				long seek1 = getSeekPoint(i, port, buf);
				long seek2 = getSeekPoint(i+1, port, buf);
				u8 tmp = 0;
				fseek(key_movie_fp, seek1, SEEK_SET);
				fread(&tmp, 1, 1, key_movie_fp);
				fseek(key_movie_fp, seek2, SEEK_SET);
				fwrite(&tmp, 1, 1, key_movie_fp);
			}
		}
	}
	for (int port = 0; port < 2; port++) {
		for (int buf = 1; buf <= 20; buf++) {
			long seek = getSeekPoint(key.frame, port, buf);
			fseek(key_movie_fp, seek, SEEK_SET);
			fwrite(&key.buf[port][buf - 1], 1, 1, key_movie_fp);
		}
	}
	FrameMax++;
	return true;
}
bool KeyMovie::UpdateKeyFrame(const PadData& key)
{
	if (key_movie_fp == NULL)return false;
	if (state != RECORD)return false;

	for (int port = 0; port < 2; port++) {
		for (int buf = 1; buf <= 20; buf++) {
			long seek = getSeekPoint(key.frame, port, buf);
			fseek(key_movie_fp, seek, SEEK_SET);
			fwrite(&key.buf[port][buf - 1], 1, 1, key_movie_fp);
		}
	}
	return true;
}

//----------------------------------
// write
//----------------------------------
void KeyMovie::writeFrameMax() {
	if (g_KeyMovie.key_movie_fp == NULL)return;
	fseek(key_movie_fp, FRAMEMAX_SEEKPOINT, SEEK_SET);
	fwrite(&FrameMax, 4, 1, key_movie_fp);
}
void KeyMovie::writeUndoCount() {
	if (g_KeyMovie.key_movie_fp == NULL)return;
	fseek(key_movie_fp, UNDOCOUNT_SEEKPOINT, SEEK_SET);
	fwrite(&UndoCount, 4, 1, key_movie_fp);
}

//----------------------------------
// PadData
//----------------------------------
std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	std::string item;
	for (char ch : s) {
		if (ch == delim) {
			if (!item.empty())
				elems.push_back(item);
			item.clear();
		}
		else {
			item += ch;
		}
	}
	if (!item.empty())
		elems.push_back(item);
	return elems;
}
wxString PadData::getString() {
	wxString s = wxString::Format("%d,%d", frame, buf[0][0]);
	for (int i = 1; i < 20; i++) {
		s += wxString::Format(",%d", buf[0][i]);
	}
	for (int i = 0; i < 20; i++) {
		s += wxString::Format(",%d", buf[1][i]);
	}
	return s;
}
void PadData::setString(wxString s)
{
	std::vector<std::string> v = split( std::string(s.c_str()), ',');
	if (v.size() != 41)return;
	frame = std::stoi(v[0]);
	for (int i = 1; i < 21; i++) {
		buf[0][i-1] = std::stoi(v[i]);
	}
	for (int i = 21; i < 41; i++) {
		buf[1][i-21] = std::stoi(v[i]);
	}
}

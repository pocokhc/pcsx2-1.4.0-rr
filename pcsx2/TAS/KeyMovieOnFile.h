#pragma once
#ifndef __KEY_MOVIE_ONFILE_H__
#define __KEY_MOVIE_ONFILE_H__

#include "PadData.h"

//----------------------------
// header
// fseekで直接場所指定しているので変数の順番は大事
// 具体的には「FrameMax」は+2「UndoCount」は+6バイトの位置に配置
//----------------------------
struct KeyMovieHeader
{
	u8 version = 1;
	u8 ID = 0xCC;//特に意味はなし、keyファイルか判定用のID

	char emu[50] = "PCSX2-1.4.0-rr";
	char author[50] = "";
	char cdrom[50] = "";

public:
	void setAuthor(wxString author);
	void setCdrom(wxString cdrom);
	void init();
};

//----------------------------
// KeyMovieOnFile
//----------------------------
class KeyMovieOnFile {
public:
	KeyMovieOnFile() {}
	~KeyMovieOnFile() { Close(); }
public:

	// file
	bool Open(const wxString fn, bool fNewOpen);
	bool Close();

	// movie
	bool writeKeyBuf(const uint & frame, const uint port, const uint bufIndex, const u8 & buf);
	bool readKeyBuf(u8 & result, const uint & frame, const uint port, const uint bufIndex);

	// pad data
	void getPadData(PadData & result_pad, unsigned long frame);
	bool DeletePadData(unsigned long frame);
	bool InsertPadData(unsigned long frame, const PadData& key);
	bool UpdatePadData(unsigned long frame, const PadData& key);

	// convert
	void ConvertP2M(wxString filename);
	void ConvertOld(wxString filename);

private:
	FILE * fp=NULL;
	wxString filename = "";

private:

	//--------------------
	// block 
	//--------------------
	long KeyMovieOnFile::_getBlockSeekPoint(const long & frame);


public:
	//--------------------
	// header
	//--------------------
	KeyMovieHeader& getHeader() { return header; }
	unsigned long& getMaxFrame() { return MaxFrame; }
	unsigned long& getUndoCount() { return UndoCount; }
	const wxString & getFilename() { return filename; }

	bool writeHeader();
	bool writeMaxFrame();

	bool readHeaderAndCheck();
	void updateFrameMax(unsigned long frame);
	void addUndoCount();

private:
	KeyMovieHeader header;
	unsigned long  MaxFrame = 0;
	unsigned long  UndoCount = 0;


};



#endif

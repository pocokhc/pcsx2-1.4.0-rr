#pragma once
#ifndef __KEY_MOVIE_H__
#define __KEY_MOVIE_H__

//----------------------------
// header
//----------------------------
struct KeyMovieHeader {
	unsigned char version = 1;
	unsigned char ID = 0xCC;//特に意味はなし、keyファイルか判定用のID

	char emu[50] = "PCSX2-1.4.0-rr";
	char auther[50] = "";		//todo
	char RecordedFrom = 0;	//todo
	char cdrom[50] = "";	//todo
};

//----------------------------
// Pad info
//----------------------------
struct PadData
{
	u32 frame = 0;
	u8 buf[2][20] = {};

public:
	wxString getString();
	void setString(wxString s);
};

//----------------------------
// KeyMovie
//----------------------------
class KeyMovie {
public:
	KeyMovie() {}
	~KeyMovie(){}
public:
	// controller
	void ControllerInterrupt(u8 &data, u8 &port, u16 & BufCount, u8 buf[]);

	// menu bar
	void Stop();
	void Start(wxString filename, bool fReadOnly);
	void ConvertP2M(wxString filename);
	void ConvertOld(wxString filename);

	// shortcut key
	void RecordModeOn();

	// use editor
	void getPadData(PadData & result_pad, u32 frame);
	bool DeleteKeyFrame(u32 frame);
	bool InsertKeyFrame(const PadData& key);
	bool UpdateKeyFrame(const PadData& key);

	// use load
	void addUndoCount();


public:
	enum KEY_MOVIE_MODE {
		NONE,
		RECORD,
		REPLAY,
	};

public:
	// getter
	u32 getMaxFrame() { return FrameMax; }
	u32 getUndoCount() { return UndoCount; }
	KeyMovieHeader& getHeader() { return header; }
	KEY_MOVIE_MODE getModeState() { return state; }


private:

	KeyMovieHeader header;
	u32   FrameMax;
	u32   UndoCount;
	FILE * key_movie_fp = NULL;
	u32 FRAMEMAX_SEEKPOINT = sizeof(KeyMovieHeader);
	u32 UNDOCOUNT_SEEKPOINT = sizeof(KeyMovieHeader) + 4;
	
	KEY_MOVIE_MODE state = NONE;
	bool fRecordFrame = false;

private:
	
	// calc seekpoint
	// seek = headerSize + FrameMax + UndoCount + ( frame<<1 + port )*BufSize + (bufCount-1)
	long getSeekPoint(u32 frame, u8 port, u8 buf) {
		return sizeof(header) + 4 + 4 + ((frame << 1) + port) * 20 + (buf - 1);
	}

	// write
	void writeFrameMax();
	void writeUndoCount();

private:
	//save frame
	long saveFrame[10];
public:
	void setSaveFrame(int slot, long frame) {
		if (0 <= slot && slot < 10) {
			saveFrame[slot] = frame;
		}
	}
	long getSaveFrame(int slot) {
		if (0 <= slot && slot < 10) {
			return saveFrame[slot];
		}
		return 0;
	}
};
extern KeyMovie g_KeyMovie;



#endif

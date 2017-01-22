#pragma once
#ifndef __PAD_DATA_H__
#define __PAD_DATA_H__

#include <map>


#define PadDataNormalKeysSize 16
const wxString PadDataNormalKeys[PadDataNormalKeysSize] =
{
	"up",
	"right",
	"left",
	"down",
	"select",
	"start",
	"x",
	"circle",
	"square",
	"triangle",
	"l1",
	"l2",
	"l3",
	"r1",
	"r2",
	"r3"
};

#define PadDataAnalogKeysSize 4
const wxString PadDataAnalogKeys[PadDataAnalogKeysSize] =
{
	"l_updown",
	"l_leftright",
	"r_updown",
	"r_leftright"
};


//----------------------------
// Pad info
//----------------------------
struct PadData
{
public:
	PadData();
	~PadData() {}
public:

	bool fExistKey = false;
	u8 buf[2][6];
	
public:
	wxString serialize();
	void deserialize(wxString s);

	//------------------------------------------
	// normalKey
	//------------------------------------------
	std::map<wxString, bool> getNormalKeys(int port);
	void setNormalKeys(int port, std::map<wxString, bool> key);
	
	//------------------------------------------
	// analogKey 0~255
	//   max left/up    : 0
	//   neutral        : 127
	//   max right/down : 255
	//------------------------------------------
	std::map<wxString, int> getAnalogKeys(int port);
	void setAnalogKeys(int port, std::map<wxString, int> key);


private:
	void setNormalButton(int port, wxString button, bool pushed);
	bool getNormalButton(int port, wxString button);
	void getKeyBit(byte keybit[2], wxString button);

	void setAnalogButton(int port, wxString button, int push);
	int getAnalogButton(int port, wxString button);
	

};


#endif

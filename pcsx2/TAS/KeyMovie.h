#pragma once
#ifndef __KEY_MOVIE_H__
#define __KEY_MOVIE_H__

// controller
void KeyMovie_ControllerInterrupt(u8 &data, u8 &port, u16 & BufCount, u8 buf[]);

// menu bar
void KeyMovie_Stop();
void KeyMovie_Start(wxString filename, bool fReadOnly);
void KeyMovie_ConvertP2M(wxString filename);
void KeyMovie_ConvertOld(wxString filename);

// shortcut key
void KeyMovie_RecordModeOn();


#endif

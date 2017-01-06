#pragma once
#ifndef __TAS_H__
#define __TAS_H__

// menu bar
void TAS_KeyMovieStop();
void TAS_KeyMovieStart(const WCHAR*, bool fStartPuase,bool fReadOnly);

// controller
void TAS_ControllerInterrupt(u8 &data , u8 &port, u16 & BufCount,u8 buf[]);

// movie controle
bool TAS_isStop();
void TAS_StartCheck();
void TAS_StopCheck();

// shortcut key
void TAS_FrameAdvanceDoit();
void TAS_MovieReadToggleDoit();
void TAS_PauseDoit();


#endif

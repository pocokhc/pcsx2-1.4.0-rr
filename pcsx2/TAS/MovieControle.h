#pragma once
#ifndef __MOVIE_CONTROLE_H__
#define __MOVIE_CONTROLE_H__

// movie controle
bool Movie_isStop();
void Movie_StartCheck();
void Movie_StopCheck();

// shortcut key
void Movie_FrameAdvance();
void Movie_TogglePause();

// public function
void Movie_Pause();
void Movie_UnPause();


#endif

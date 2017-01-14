#pragma once
#ifndef __LUA_ENGINE_H__
#define __LUA_ENGINE_H__

void LuaFrameBoundary(void);
void LoadLuaCode(wxString filename);

void LuaCodeStop();
void LuaCodeRun();
void LuaCodeRestart();

#endif
#pragma once
#ifndef __LUA_MANAGER_H__
#define __LUA_MANAGER_H__

#include "LuaEngine.h"

class LuaManager {
public:
	LuaManager() {}
	~LuaManager() {}
public:

	void FrameBoundary(void);

	void Load(wxString filename);
	void Stop();
	void Run();
	void Restart();

	LuaEngine * getLuaEnginPtr(lua_State* l);

private:
	LuaEngine lua;

};
extern LuaManager g_Lua;


#endif
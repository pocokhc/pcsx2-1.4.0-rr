#pragma once
#ifndef __LUA_ENGINE_H__
#define __LUA_ENGINE_H__


extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaEngine {
public:
	LuaEngine() {}
	~LuaEngine() {}
public:
	enum LuaState {
		NOT_OPEN,
		OPEN,
		RUNNING,
		RESUME,
		CLOSE
	};
	void setState(LuaState _state);
	LuaState getState() { return state; }

	bool Load(wxString filename);
	bool Load();
	void Resume();
	void Close();

	bool isSelf(lua_State* l) { return (l == Lthread); }

	void setLuaCallBefore(int id) { luaCallBefore = id; }
	void setLuaCallAfter(int id) { luaCallAfter = id; }
	void setLuaCallExit(int id) { luaCallExit = id; }

	void callAfter();

private:
	lua_State* L = NULL;
	lua_State* Lthread = NULL;
	wxString file;
	LuaState state = NOT_OPEN;

	int luaCallBefore = 0;
	int luaCallAfter = 0;
	int luaCallExit = 0;

private:
	void CallbackError(lua_State *L);

};


#endif


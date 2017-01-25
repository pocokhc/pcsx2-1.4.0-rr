#include "PrecompiledHeader.h"

#include "LuaEngine.h"
#include "LuaFunctions.h"

#include "LuaFrame.h"	// use "LuaFrame"
#include "App.h"	// use "LuaFrame"


// --------------------------------------------------------------------------------------
//  LoadLuaCode
// --------------------------------------------------------------------------------------
bool LuaEngine::Load(wxString filename)
{
	Close();
	file = filename;
	return Load();
}
bool LuaEngine::Load()
{
	Close();

	L = luaL_newstate();
	Lthread = lua_newthread(L);
	luaL_openlibs(Lthread);	// add lua library

	// luaL_register
#define LUA_REFISTER( funcs , name ) {luaL_newlib(Lthread, funcs); lua_setglobal(Lthread, name);}
	LUA_REFISTER(lua_function_emulib, "emu");
	//LUA_REFISTER(lua_function_memorylib, "memory");
	LUA_REFISTER(lua_function_joypadlib, "joypad");
	LUA_REFISTER(lua_function_savestatelib, "savestate");
	LUA_REFISTER(lua_function_movielib, "movie");
	//LUA_REFISTER(lua_function_guilib, "gui");
#undef LUA_REFISTER

	// load
	if (luaL_loadfile(Lthread, file.c_str() ) != LUA_OK) {
		CallbackError(Lthread);
		Close();
		return false;
	}
	
	setState(OPEN);
	return true;
}

//--------------------------------------------------
// frameadvance controle
//--------------------------------------------------
void LuaEngine::Resume(void)
{
	if (Lthread == NULL)return;
	if ( state != RESUME )return;

	setState(RUNNING);
	int result = lua_resume(Lthread, NULL, 0);
	if (result == LUA_OK) {
		// lua script end
		Close();
		return;
	}
	else if (result == LUA_YIELD) {
		// not problem
	}
	else {
		// error
		CallbackError(Lthread);
		Close();
		return;
	}

}

//--------------------------------------------------
// Close
//--------------------------------------------------
void LuaEngine::Close(void)
{
	if (L != NULL) {
		lua_close(L);
	}
	L = NULL;
	Lthread = NULL;
	setState(CLOSE);
	
}

//--------------------------------------------------
// print
//--------------------------------------------------
void LuaEngine::CallbackError(lua_State *L)
{
	Console.WriteLn(Color_StrongBlue, "[lua]Lua Error:%s", lua_tostring(L, -1));
}

//--------------------------------------------------
// change state -> LuaFrame
// LuaFramへのアクセスはすべてここ
//--------------------------------------------------
void LuaEngine::setState(LuaEngine::LuaState _state)
{
	state = _state;
	LuaFrame * frame = wxGetApp().GetLuaFramePtr();
	wxString msg = "";
	
	if (_state == NOT_OPEN) {
		msg = "lua not open.";
	}
	else if (_state == OPEN) {
		msg = "lua open.";
	}
	else if (_state == RUNNING) {
		msg = "lua running.";
	}
	else if (_state == RESUME) {
		msg = "lua resume.";
		frame->pushRunState();
	}
	else if (_state == CLOSE) {
		msg = "lua close.";
		frame->pushStopState();
	}
	frame->drawState(wxString::Format(L"%s(%s)", msg, file));
}

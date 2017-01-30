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
#define LUA_REGISTER( funcs , name ) {luaL_newlib(Lthread, funcs); lua_setglobal(Lthread, name);}
	LUA_REGISTER(lua_function_emulib, "emu");
	LUA_REGISTER(lua_function_memorylib, "memory");
	LUA_REGISTER(lua_function_joypadlib, "joypad");
	LUA_REGISTER(lua_function_savestatelib, "savestate");
	LUA_REGISTER(lua_function_movielib, "movie");
	//LUA_REGISTER(lua_function_guilib, "gui");
#undef LUA_REGISTER

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
	if (state == CLOSE) {
		if (L != NULL) {
			lua_close(L);
			L = NULL;
			Lthread = NULL;
		}
		return;
	}
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
void LuaEngine::Close()
{
	
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

void LuaEngine::callAfter()
{
	/*if (Lthread == NULL)return;
	lua_settop(Lthread, 0);
	lua_pushinteger(Lthread, luaCallBefore);
	if (lua_isfunction(Lthread, -1))
	{
		lua_pcall(Lthread, 0, 0, 0);
	}
	lua_settop(Lthread, 0);
	*/
}

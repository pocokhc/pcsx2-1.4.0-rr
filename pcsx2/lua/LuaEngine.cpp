#include "PrecompiledHeader.h"


#include "MemoryTypes.h"// use"g_FrameCount"
#include "Counters.h"	// use"g_FrameCount"

#include "TAS/MovieControle.h"

#include "LuaEngine.h"
#include "LuaFrame.h"
#include "App.h" // use "LuaFrame"

#include "Debugger/DisassemblyDialog.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


//=============================================
//  one lua script class
//=============================================
struct LuaEngine {
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

public:
	bool LoadLuaCode();
	void Resume(void);
	void Close(void);

	lua_State* L = NULL;
	lua_State* Lthread = NULL;
	wxString file;

public:
	void CallbackError(lua_State *L);

private:
	LuaState state = NOT_OPEN;

};


//=============================================
//  lua other value
//=============================================
static uint oldFrameCount = -1;
static LuaEngine luaEngine;


//=============================================
// emu_frameadvance
//=============================================
static int emu_frameadvance(lua_State *L)
{
	if (luaEngine.getState() != LuaEngine::RUNNING)return 0;
	g_MovieControle.FrameAdvance();
	luaEngine.setState(LuaEngine::RESUME);
	return lua_yield(L, 0);
}



//=============================================
// lua library define
//=============================================

static int memory_readbyte(lua_State * L)
{
	printf("todo");
	//DisassemblyDialog *dis = wxGetApp().GetDisassemblyPtr();
	//dis->currentCpu->getMemoryView()->

	// return
	lua_pushnumber(L, 1);
	return 1;	// return num
}



//=============================================
// lua library name
//=============================================
static const struct luaL_Reg emulib[] =
{
	{ "frameadvance", emu_frameadvance },
	//{"speedmode", emu_speedmode},
	//{"wait", emu_wait},
	//{"pause", emu_pause},
	//{"unpause", emu_unpause},
	//{"emulateframe", emu_emulateframe},
	//{"emulateframefastnoskipping", emu_emulateframefastnoskipping},
	//{"emulateframefast", emu_emulateframefast},
	//{"emulateframeinvisible", emu_emulateframeinvisible},
	//{"redraw", emu_redraw},
	//{"framecount", emu_getframecount},
	//{"lagcount", emu_getlagcount},
	//{"lagged", emu_lagged},
	//{"emulating", emu_emulating},
	//{"atframeboundary", emu_atframeboundary},
	//{"registerbefore", emu_registerbefore},
	//{"registerafter", emu_registerafter},
	//{"registerstart", emu_registerstart},
	//{"registerexit", emu_registerexit},
	//{ "persistglobalvariables", emu_persistglobalvariables },
	//{ "message", emu_message },
	//{ "print", print }, // sure, why not
	//{"openscript", emu_openscript},
	//{"loadrom", emu_loadrom},
	//{"reset", emu_reset},
	// alternative names
	//{"openrom", emu_loadrom},
	{ NULL, NULL }
};

static const struct luaL_Reg memorylib[] = {
	{ "readbyte",				memory_readbyte },
	/*{ "readbytesigned",			memory_readbytesigned },
	{ "readword",				memory_readword },
	{ "readwordsigned",			memory_readwordsigned },
	{ "readdword",				memory_readdword },
	{ "readdwordsigned",		memory_readdwordsigned },
	{ "readbyterange",			memory_readbyterange },
	{ "writebyte",				memory_writebyte },
	{ "writeword",				memory_writeword },
	{ "writedword",				memory_writedword },
	{ "getregister",			memory_getregister },
	{ "setregister",			memory_setregister },
	{ "gbromreadbyte",			memory_gbromreadbyte },
	{ "gbromreadbytesigned",	memory_gbromreadbytesigned },
	{ "gbromreadword",			memory_gbromreadword },
	{ "gbromreadwordsigned",	memory_gbromreadwordsigned },
	{ "gbromreaddword",			memory_gbromreaddword },
	{ "gbromreaddwordsigned",	memory_gbromreaddwordsigned },
	{ "gbromreadbyterange",		memory_gbromreadbyterange },

	// alternate naming scheme for word and double-word and unsigned
	{ "readbyteunsigned",		memory_readbyte },
	{ "readwordunsigned",		memory_readword },
	{ "readdwordunsigned",		memory_readdword },
	{ "readshort",				memory_readword },
	{ "readshortunsigned",		memory_readword },
	{ "readshortsigned",		memory_readwordsigned },
	{ "readlong",				memory_readdword },
	{ "readlongunsigned",		memory_readdword },
	{ "readlongsigned",			memory_readdwordsigned },
	{ "writeshort",				memory_writeword },
	{ "writelong",				memory_writedword },
	{ "gbromreadbyteunsigned",	memory_gbromreadbyte },
	{ "gbromreadwordunsigned",	memory_gbromreadword },
	{ "gbromreaddwordunsigned",	memory_gbromreaddword },
	{ "gbromreadshort",			memory_gbromreadword },
	{ "gbromreadshortunsigned",	memory_gbromreadword },
	{ "gbromreadshortsigned",	memory_gbromreadwordsigned },
	{ "gbromreadlong",			memory_gbromreaddword },
	{ "gbromreadlongunsigned",	memory_gbromreaddword },
	{ "gbromreadlongsigned",	memory_gbromreaddwordsigned },

	// memory hooks
	{ "registerwrite",	   memory_registerwrite },
	{ "registerread",	   memory_registerread },
	{ "registerexec",	   memory_registerexec },
	// alternate names
	{ "register",		   memory_registerwrite },
	{ "registerrun",	   memory_registerexec },
	{ "registerexecute",   memory_registerexec },*/

	{ NULL,				   NULL }
};



// --------------------------------------------------------------------------------------
//  LoadLuaCode
// --------------------------------------------------------------------------------------
bool LuaEngine::LoadLuaCode()
{
	luaEngine.Close();

	L = luaL_newstate();
	Lthread = lua_newthread(L);
	luaL_openlibs(Lthread);	// add lua library

	// luaL_register
	luaL_newlib(Lthread, emulib); lua_setglobal(Lthread, "emu");
	luaL_newlib(Lthread, memorylib); lua_setglobal(Lthread, "memory");

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

	wxMilliSleep(1000 / 10);

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
	frame->drawState(wxString::Format("%s(%s)", msg, file));
}

// --------------------------------------------------------------------------------------
//  LoadLuaCode
// --------------------------------------------------------------------------------------
void LoadLuaCode(wxString filename)
{
	luaEngine.file = filename;
	if (!luaEngine.LoadLuaCode()) {
		return;
	}
}
void LuaCodeStop() {
	luaEngine.Close();
	Console.WriteLn(Color_StrongBlue, "[lua]Puase");
}
void LuaCodeRun() {
	if (!luaEngine.LoadLuaCode()) {
		return;
	}
	luaEngine.setState(LuaEngine::RESUME);
}
void LuaCodeRestart() {
	if (!luaEngine.LoadLuaCode()) {
		return;
	}
	luaEngine.setState(LuaEngine::RESUME);
}

//--------------------------------------------------
// LuaFrameBoundary
//--------------------------------------------------
void LuaFrameBoundary(void)
{
	// 1frame 1doit
	if (oldFrameCount == g_FrameCount)return;
	oldFrameCount = g_FrameCount;

	// lua doit
	luaEngine.Resume();

}

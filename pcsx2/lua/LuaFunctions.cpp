#include "PrecompiledHeader.h"


#include "MemoryTypes.h"// use"g_FrameCount"
#include "Counters.h"	// use"g_FrameCount"

#include "TAS/MovieControle.h"

#include "LuaManager.h"
#include "LuaEngine.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


//=============================================
// emu_frameadvance
//=============================================
static int emu_frameadvance(lua_State *L)
{
	LuaEngine * pLua = g_Lua.getLuaEnginPtr(L);
	if (pLua == NULL)return 0;

	if (pLua->getState() != LuaEngine::RUNNING)return 0;
	g_MovieControle.FrameAdvance();
	pLua->setState(LuaEngine::RESUME);
	return lua_yield(L, 0);
}

static int emu_pause(lua_State *L)
{
	g_MovieControle.Pause();
	return 0;
}
static int emu_unpause(lua_State *L)
{
	g_MovieControle.UnPause();
	return 0;
}
static int emu_getframecount(lua_State *L)
{
	lua_pushinteger(L, g_FrameCount);
	return 1;
}

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
	{"pause", emu_pause},
	{"unpause", emu_unpause},
	//{"emulateframe", emu_emulateframe},
	//{"emulateframefastnoskipping", emu_emulateframefastnoskipping},
	//{"emulateframefast", emu_emulateframefast},
	//{"emulateframeinvisible", emu_emulateframeinvisible},
	//{"redraw", emu_redraw},
	{"framecount", emu_getframecount},
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
const struct luaL_Reg *lua_function_emulib = emulib;

static const struct luaL_Reg memorylib[] = {
	/*{ "readbyte",				memory_readbyte },
	{ "readbytesigned",			memory_readbytesigned },
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
const struct luaL_Reg *lua_function_memorylib = memorylib;


static const struct luaL_Reg joypadlib[] = {
	{ "get",	  joypad_get },
	{ "set",	  joypad_set },
	{ NULL,		  NULL }
};
const struct luaL_Reg *lua_function_joypadlib= joypadlib;

static const struct luaL_Reg savestatelib[] = {
	{ "saveslot",	savestate_save },
	{ "loadslot",	savestate_load },

	{ NULL,		NULL }
};
const struct luaL_Reg *lua_function_savestatelib= savestatelib;


static const struct luaL_Reg movielib[] = {
	{ "mode",			  movie_getmode },
	{ "length",			  movie_getlength },
	{ "author",			  movie_getauthor },
	{ "getauthor",		  movie_getauthor },
	{ "name",			  movie_getfilename },
	{ "getname",		  movie_getfilename },
	{ "rerecordcount",	  movie_rerecordcount },
	{ "stop",			  movie_stop },
	{ "close",			  movie_stop },
	{ NULL,				  NULL }
};
extern const struct luaL_Reg *lua_function_movielib = movielib;

static const struct luaL_Reg guilib[] = {
	/*{ "register",	  gui_register },
	{ "text",		  gui_text },
	{ "box",		  gui_drawbox },
	{ "line",		  gui_drawline },
	{ "pixel",		  gui_drawpixel },
	{ "opacity",	  gui_setopacity },
	{ "transparency", gui_transparency },
	{ "popup",		  gui_popup },
	{ "parsecolor",	  gui_parsecolor },
	{ "gdscreenshot", gui_gdscreenshot },
	{ "gdoverlay",	  gui_gdoverlay },
	{ "getpixel",	  gui_getpixel },

	// alternative names
	{ "drawtext",	  gui_text },
	{ "drawbox",	  gui_drawbox },
	{ "drawline",	  gui_drawline },
	{ "drawpixel",	  gui_drawpixel },
	{ "setpixel",	  gui_drawpixel },
	{ "writepixel",	  gui_drawpixel },
	{ "rect",		  gui_drawbox },
	{ "drawrect",	  gui_drawbox },
	{ "drawimage",	  gui_gdoverlay },
	{ "image",		  gui_gdoverlay },
	{ "readpixel",	  gui_getpixel },*/
	{ NULL,			  NULL }
};
const struct luaL_Reg *lua_function_guilib = guilib;


static const struct luaL_Reg soundlib[] = {
	//{ "get",  sound_get },

	{ NULL,	  NULL }
};
const struct luaL_Reg *lua_function_soundlib = soundlib;



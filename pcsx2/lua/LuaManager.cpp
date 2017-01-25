#include "PrecompiledHeader.h"

#include "TAS/MovieControle.h"

#include "LuaManager.h"
#include "LuaEngine.h"

LuaManager g_Lua;

void LuaManager::FrameBoundary(void)
{
	lua.Resume();
}

void LuaManager::Load(wxString filename)
{
	if (!lua.Load(filename)) {
		return;
	}
}
void LuaManager::Stop() {
	lua.Close();
	lua.setState(LuaEngine::CLOSE);
}
void LuaManager::Run() {
	if (!lua.Load()) {
		return;
	}
	lua.setState(LuaEngine::RESUME);
	g_MovieControle.FrameAdvance();
}
void LuaManager::Restart() {
	if (!lua.Load()) {
		return;
	}
	lua.setState(LuaEngine::RESUME);
}

// lua script‚ğ•¡”“®‚©‚¹‚é‚æ‚¤‚ÉŠg’£‚·‚é—p
LuaEngine * LuaManager::getLuaEnginPtr(lua_State* l)
{
	if (lua.isSelf(l)) {
		return &lua;
	}
	return NULL;
}

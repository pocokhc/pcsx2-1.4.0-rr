#include "PrecompiledHeader.h"

#include "TAS/MovieControle.h"

#include "LuaManager.h"
#include "LuaEngine.h"

LuaManager g_Lua;

static PadData nowFramePadData;
static bool fSetFrameKey=false;


void LuaManager::ControllerInterrupt(u8 &data, u8 &port, u16 & BufCount, u8 buf[])
{
	if (port < 0 || 1 < port)return;
	int bufIndex = BufCount - 3;
	if (bufIndex < 0 || 6 < bufIndex)return;

	if (fSetFrameKey)
	{
		buf[BufCount] = nowFramePadData.buf[port][bufIndex];
	}
	else {
		nowFramePadData.buf[port][bufIndex] = buf[BufCount];
	}

	// turn end
	if (BufCount == 7) {
		fSetFrameKey = false;
	}
}
PadData & LuaManager::getNowFramePadData() {
	return nowFramePadData;
}
void LuaManager::setNowFramePadData(const PadData & pad)
{
	nowFramePadData.deserialize(pad.serialize());
	fSetFrameKey = true;
}

void LuaManager::FrameBoundary()
{
	lua.callAfter();
	lua.callBefore();
	lua.Resume();
}

void LuaManager::setFileName(wxString filename)
{
	lua.setFileName(filename);
}
void LuaManager::Stop()
{
	lua.Close();
}
void LuaManager::Run() {
	if (!lua.Load()) {
		return;
	}
	lua.setState(LuaEngine::RESUME);
	lua.Resume();
}
void LuaManager::Restart() {
	Run();
}

// lua scriptを複数動かせるように拡張する時用
LuaEngine * LuaManager::getLuaEnginPtr(lua_State* l)
{
	if (lua.isSelf(l)) {
		return &lua;
	}
	return NULL;
}

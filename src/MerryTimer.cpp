#include "MerryTimer.h"
#include "MerryTimerManager.h"
#include "MerryLua.h"
#include "MerryInformationDialog.h"

MerryTimer::MerryTimer(int funcRef)
{
	m_callback = funcRef;
}

MerryTimer::~MerryTimer()
{
	if (!g_lua)
		return;

	lua_State* L = g_lua->GetLua();
	assert(L);
	luaL_unref(L, LUA_REGISTRYINDEX, m_callback);
}

void MerryTimer::Notify()
{
	lua_State* L = g_lua->GetLua();
	assert(L);

	bool isRunning = this->IsRunning();

	lua_rawgeti(L, LUA_REGISTRYINDEX, m_callback);
	assert(lua_isfunction(L, -1));
	if (lua_pcall(L, 0, 0, 0))
	{
		new MerryInformationDialog(wxString::Format(wxT("Timer execute failed!")),
			wxString(lua_tostring(L, -1), wxConvLocal));
		lua_pop(L, 1);
	}

	if (!isRunning)
		g_timers->ClearTimer(this);
}

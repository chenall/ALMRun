#include "MerryCommand.h"
#include "MerryLua.h"
#include "MerryInformationDialog.h"

MerryCommand::MerryCommand(int commandID, const wxString& commandName, const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey)
{
	m_commandID = commandID;
	m_commandName = commandName;
	m_commandFunc = funcRef;
	m_triggerKey = triggerKey;
	m_commandDesc = commandDesc;
	m_commandLine = commandLine;
}

MerryCommand::~MerryCommand()
{
	if (!g_lua)
		return;

	lua_State* L = g_lua->GetLua();
	assert(L);
	luaL_unref(L, LUA_REGISTRYINDEX, m_commandFunc);
}

void MerryCommand::ExecuteCommand(const wxString& commandArg) const
{

	if (!g_lua)
		return;

	lua_State* L = g_lua->GetLua();
	assert(L);

	if (m_commandFunc == 0)
	{
		lua_getglobal(L, "CmdCallFunc");
		if (lua_isnil(L, 1))
		{
			lua_pop(L, 1);
			return;
		}
		lua_pushstring(L, m_commandLine.c_str());
		lua_pushstring(L, commandArg.c_str());
	}
	else
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, m_commandFunc);
		assert(lua_isfunction(L, -1));
		lua_pushstring(L, commandArg.c_str());
		lua_pushnumber(L, m_commandID);
	}

	lua_pushnumber(L, ::wxGetKeyState(WXK_CONTROL)?1:0);//Ctr¼ü°´ÏÂ×´Ì¬

	if (lua_pcall(L, 3, 0, 0))
	{
		new MerryInformationDialog(
			wxString::Format(wxT("Command %s execute failed"), m_commandName),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}
}

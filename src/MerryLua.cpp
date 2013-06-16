#include "MerryLua.h"
#include "MerryLuaExport.h"
#include "MerryConfig.h"

MerryLua* g_lua = NULL;
MerryLua* lua_bak = NULL;

MerryLua::MerryLua()
{
	L = luaL_newstate();
	assert(L);
	luaL_openlibs(L);

	lua_register(L, "addCommand",                         LuaAddCommand);
	lua_register(L, "enableCommandKey",                   LuaEnableCommandKey);
	lua_register(L, "disableCommandKey",                  LuaDisableCommandKey);
	lua_register(L, "shellExecute",                       LuaShellExecute);
	lua_register(L, "getForegroundWindow",                LuaGetForegroundWindow);
	lua_register(L, "setForegroundWindow",                LuaSetForegroundWindow);
	lua_register(L, "toggleMerry",                        LuaToggleMerry);
	lua_register(L, "showWindow",                         LuaShowWindow);
	lua_register(L, "closeWindow",                        LuaCloseWindow);
	lua_register(L, "isWindowMax",                        LuaIsWindowMax);
	lua_register(L, "isWindowMin",                        LuaIsWindowMin);
	lua_register(L, "isWindowShown",                      LuaIsWindowShown);
	lua_register(L, "getWindowText",                      LuaGetWindowText);
	lua_register(L, "setWindowText",                      LuaSetWindowText);
	lua_register(L, "getWindowSize",                      LuaGetWindowSize);
	lua_register(L, "setWindowSize",                      LuaSetWindowSize);
	lua_register(L, "getWindowPosition",                  LuaGetWindowPosition);
	lua_register(L, "setWindowPosition",                  LuaSetWindowPosition);
	lua_register(L, "findWindow",                         LuaFindWindow);
	lua_register(L, "getMousePosition",                   LuaGetMousePosition);
	lua_register(L, "setMousePosition",                   LuaSetMousePosition);
	lua_register(L, "GetClipboardData",					  LuaGetClipboardData);
	lua_register(L, "SetClipboardData",					  LuaSetClipboardData);
#ifdef __WXMSW__
	lua_register(L, "GetForegroundWindow",                LuaGetForegroundWindow);
	lua_register(L, "SetForegroundWindow",                LuaSetForegroundWindow);
	lua_register(L, "ShowWindow",                         LuaShowWindow);
	lua_register(L, "CloseWindow",                        LuaCloseWindow);
	lua_register(L, "IsWindowMax",                        LuaIsWindowMax);
	lua_register(L, "IsWindowMin",                        LuaIsWindowMin);
	lua_register(L, "IsWindowShown",                      LuaIsWindowShown);
	lua_register(L, "GetWindowText",                      LuaGetWindowText);
	lua_register(L, "SetWindowText",                      LuaSetWindowText);
	lua_register(L, "GetWindowSize",                      LuaGetWindowSize);
	lua_register(L, "SetWindowSize",                      LuaSetWindowSize);
	lua_register(L, "GetWindowPosition",                  LuaGetWindowPosition);
	lua_register(L, "SetWindowPosition",                  LuaSetWindowPosition);
	lua_register(L, "FindWindow",                         LuaFindWindow);
	lua_register(L, "GetMousePosition",                   LuaGetMousePosition);
	lua_register(L, "SetMousePosition",                   LuaSetMousePosition);
	lua_register(L, "GetShellFolder",					  LuaSHSpecialFolders);
	lua_register(L, "EmptyRecycleBin",					  LuaEmptyRecycleBin);
	lua_register(L, "SHEmptyRecycleBin",				  LuaSHEmptyRecycleBin);
#endif
	lua_register(L, "message",                            LuaMessage);
	lua_register(L, "enterKey",                           LuaEnterKey);
	lua_register(L, "enterText",                          LuaEnterText);
	lua_register(L, "setTimer",                           LuaSetTimer);
	lua_register(L, "clearTimer",                         LuaClearTimer);
	lua_register(L, "config",							  LuaConfig);
	lua_register(L, "ListDir",							  LuaDir);
	lua_register(L, "SetEnv",							  LuaSetEnv);
	lua_register(L, "GetEnv",							  LuaGetEnv);
	lua_register(L, "MessageBox",						  LuaMessageBox);
	lua_register(L, "InputBox",							  LuaInputBox);
	lua_register(L, "ReConfig",							  LuaReConfig);
	lua_register(L, "DirExists",						  LuaDirExists);
	lua_register(L, "FileExists",						  LuaFileExists);
	lua_register(L, "TestConfig",						  LuaTestConfig);
	lua_pushboolean(L, true);
#ifdef __WXMSW__
	lua_setglobal(L, "WINDOWS");
	lua_pushstring(L, "windows");
#elif __WXOSX__
	lua_setglobal(L, "MAC");
	lua_pushstring(L, "mac");
#elif __WXGTK__
	lua_setglobal(L, "GTK");
	lua_pushstring(L, "gtk");
#endif
	lua_setglobal(L, "PLATFORM");
	lua_pushstring(L,wxGetenv("ALMRUN_HOME"));
	lua_setglobal(L, "ALMRUN_HOME");
	this->DoConfig();
}

MerryLua::~MerryLua()
{
	assert(lua_gettop(L) == 0);
	lua_close(L);
}

lua_State* MerryLua::GetLua()
{
	return L;
}

void MerryLua::DoConfig()
{
	if (!wxFileExists(ALMRUN_DEFAULT_CONFIG_FILE))
		return;
	if (luaL_dofile(L, ALMRUN_DEFAULT_CONFIG_FILE))
	{
		new MerryInformationDialog(
			wxT("Configure failed"),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}
}

void MerryLua::OnClose()
{
	if (!L)
		return;

	lua_getglobal(L, "onClose");
	if (lua_isnil(L, 1))
	{
		lua_pop(L, 1);
		return;
	}

	if (lua_pcall(L, 0, 0, 0))
	{
		new MerryInformationDialog(
			wxT("Event onClose execute failed"),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}
}

bool MerryLua::onCompare(const wxString& commandName,const wxString& commandPrefix)
{
	assert(L);
	bool ret = false;
	lua_getglobal(L, "HookCompare");
	if (lua_isnil(L, 1))
	{
		lua_pop(L, 1);
		return false;
	}

	lua_pushstring(L, commandName.c_str());
	lua_pushstring(L, commandPrefix.c_str());
	if (lua_pcall(L, 2, 1, 0) == 0)
		ret = (lua_toboolean(L,-1) == 1);
	lua_pop(L, 1);
	return ret;
}

void MerryLua::OnUndefinedCommand(const wxString& commandName, const wxString& commandArg)
{
	assert(L);

	lua_getglobal(L, "onUndefinedCommand");
	if (lua_isnil(L, 1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushstring(L, commandName.c_str());
	lua_pushstring(L, commandArg.c_str());
	if (lua_pcall(L, 2, 0, 0))
	{
		new MerryInformationDialog(
			wxT("Event onUndefinedCommand execute failed"),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}
}

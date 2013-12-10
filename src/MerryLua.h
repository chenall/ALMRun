#ifndef _MERRY_LUA_H_
#define _MERRY_LUA_H_

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

typedef enum{
	LUA_toggleMerry = 0,
	LUA_ReConfig,
	LUA_EXTERNAL_FUNC,
	LUA_CMDRun_FUNC = LUA_EXTERNAL_FUNC,
	LUA_CMDCompare_FUNC,
	LUA_PluginCommand_FUNC,
	LUA_ReadALTRunConfig_FUNC,
	LUA_FUNC_MAX,
} lua_func_t;

#include "MerryWx.h"

class MerryLua
{
public:
	MerryLua();
	~MerryLua();

	lua_State* GetLua();

public:
	// events
	static const char *lua_func_str[];
	void OnClose();
	void DoConfig();
	void OnUndefinedCommand(const wxString& commandName, const wxString& commandArg);
	bool onCompare(const wxString& commandName,const wxString& commandPrefix);
	bool get_func(lua_func_t t);
	int get_funcref(lua_func_t t);
#if _DEBUG
	void stackDump(); 
#endif

private:
	int lua_func[LUA_FUNC_MAX];
	lua_State* L;
};

extern MerryLua* g_lua;

#endif

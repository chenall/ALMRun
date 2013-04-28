#include "MerryCommandManager.h"
#include "MerryError.h"
#include "MerryInformationDialog.h"
#include "MerryHotkey.h"
#include "MerryApp.h"
#include "MerryController.h"
#include "MerryTimerManager.h"
#include "MerryMacHelper.h"

static int LuaAddCommand(lua_State* L)
{
	if (!lua_istable(L, 1))
		luaL_error(L, "bad argument #1 to 'addCommand' (table expected)");

	lua_pushstring(L, "name");
	lua_rawget(L, 1);
	wxString commandName(wxString(lua_tostring(L, -1), wxConvLocal));

	lua_pushstring(L, "key");
	lua_rawget(L, 1);
	wxString triggerKey(wxString(lua_tostring(L, -1), wxConvLocal));

	if (commandName.empty() && triggerKey.empty())
	{
		lua_settop(L, 1);
		luaL_error(L, "Command name or key not found.\r\n\r\n参数错误,丢失name或key参数.");
	}

	lua_pushstring(L, "desc");
	lua_rawget(L, 1);
	wxString commandDesc(wxString(lua_tostring(L, -1), wxConvLocal));

	lua_pushstring(L, "cmd");
	lua_rawget(L, 1);
	wxString commandLine(wxString(lua_tostring(L, -1), wxConvLocal));

	lua_pushstring(L, "func");
	lua_rawget(L, 1);
	int funcRef = 0;
	if (!lua_isfunction(L, -1))
	{
		if (commandLine.empty())
		{
			lua_settop(L, 1);
			luaL_error(L, "can't find the command or func\r\n\r\n参数错误,cmd或func参数未设置.");
			return 0;
		}
	}
	else
	{
		funcRef = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	int order = 0;
	if (!commandName.empty())
	{
		lua_getglobal(L, "GetCmdOrder");
		if (!lua_isnil(L, 1))
		{
			lua_pushstring(L, commandName.c_str());
			if (lua_pcall(L, 1, 1, 0) == 0)
				order = lua_tointeger(L,-1);

		}
		lua_pop(L, 1);
	}

	const int commandID = g_commands->AddCommand(commandName, commandDesc,commandLine,funcRef, triggerKey,order);
	if (commandID == -2)
	{
		luaL_unref(L, LUA_REGISTRYINDEX, funcRef);
		lua_settop(L, 1);
		luaL_error(L, ::MerryGetLastError().c_str());
	}
	else if (commandID == -1)
	{
		lua_settop(L, 1);
		return 0;
	}

	if (!g_hotkey->RegisterHotkey(commandID))
	{
		lua_settop(L, 1);
		luaL_error(L, ::MerryGetLastError().c_str());
	}

	lua_settop(L, 1);
	lua_pushnumber(L, commandID);
	return 1;
}

static int LuaEnableCommandKey(lua_State* L)
{
	if (lua_isnil(L, 1))
		return 0;
	if (!g_hotkey->RegisterHotkey(lua_tonumber(L, 1)))
		luaL_error(L, ::MerryGetLastError().c_str());
	return 0;
}

static int LuaDisableCommandKey(lua_State* L)
{
	if (!lua_isnil(L, 1))
		g_hotkey->UnregisterHotkey(lua_tonumber(L, 1));
	return 0;
}

static int LuaShellExecute(lua_State* L)
{
	wxString commandName(wxString(lua_tostring(L, 1), wxConvLocal));
	wxString commandArg(wxString(lua_tostring(L, 2), wxConvLocal));
	wxString workingDir(wxString(lua_tostring(L, 3), wxConvLocal));
	wxString show(wxString(lua_tostring(L, 4), wxConvLocal));
	lua_pushboolean(L, g_controller->ShellExecute(commandName, commandArg, workingDir, show));
	return 1;
}

static int LuaGetForegroundWindow(lua_State* L)
{
	void* window = g_controller->GetForegroundWindow();
	if (window)
		lua_pushlightuserdata(L, window);
	else
		lua_pushnil(L);
	return 1;
}

static int LuaSetForegroundWindow(lua_State* L)
{
	g_controller->SetForegroundWindow(lua_touserdata(L, 1));
	return 0;
}

static int LuaToggleMerry(lua_State* L)
{
	MerryFrame& frame = ::wxGetApp().GetFrame();
#ifdef __WXOSX__
	if (!frame.IsShown())
		MerryActivateIgnoringOtherApps();
#endif
	frame.IsShown() ? frame.Hide() : frame.Show();
	return 0;
}

static int LuaShowWindow(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	wxString show(wxString(lua_tostring(L, 2), wxConvLocal));
	g_controller->ShowWindow(window, show);
	return 0;
}

static int LuaCloseWindow(lua_State* L)
{
	g_controller->CloseWindow(lua_touserdata(L, 1));
	return 0;
}

static int LuaIsWindowMax(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	lua_pushboolean(L, g_controller->IsWindowMax(window));
	return 1;
}

static int LuaIsWindowMin(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	lua_pushboolean(L, g_controller->IsWindowMin(window));
	return 1;
}

static int LuaIsWindowShown(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	lua_pushboolean(L, g_controller->IsWindowShown(window));
	return 1;
}

static int LuaGetWindowText(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	wxString windowText = g_controller->GetWindowText(window);
	lua_pushstring(L, windowText.c_str());
	return 1;
}

static int LuaSetWindowText(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	wxString windowText(wxString(lua_tostring(L, 2), wxConvLocal));
	g_controller->SetWindowText(window, windowText);
	return 0;
}

static int LuaGetWindowSize(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	int width, height;
	g_controller->GetWindowSize(window, width, height);
	lua_pushnumber(L, width);
	lua_pushnumber(L, height);
	return 2;
}

static int LuaSetWindowSize(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	int width = lua_tonumber(L, 2);
	int height = lua_tonumber(L, 3);
	g_controller->SetWindowSize(window, width, height);
	return 0;
}

static int LuaGetWindowPosition(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	int x, y;
	g_controller->GetWindowPosition(window, x, y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int LuaSetWindowPosition(lua_State* L)
{
	void* window = lua_touserdata(L, 1);
	int x = lua_tonumber(L, 2);
	int y = lua_tonumber(L, 3);
	g_controller->SetWindowPosition(window, x, y);
	return 0;
}

static int LuaFindWindow(lua_State* L)
{
	wxString windowName(wxString(lua_tostring(L, 1), wxConvLocal));
	void* parentWindow = lua_touserdata(L, 2);
	void* window = g_controller->FindWindow(windowName, parentWindow);
	if (window)
		lua_pushlightuserdata(L, g_controller->FindWindow(windowName, parentWindow));
	else
		lua_pushnil(L);
	return 1;
}

static int LuaGetMousePosition(lua_State* L)
{
	int x, y;
	g_controller->GetMousePosition(x, y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int LuaSetMousePosition(lua_State* L)
{
	int x = lua_tonumber(L, 1);
	int y = lua_tonumber(L, 2);
	g_controller->SetMousePosition(x, y);
	return 0;
}

static int LuaMessage(lua_State* L)
{
	wxString title,message;
	if (!lua_isstring(L,2))
	{
		title = wxT("Message");
		message = wxString(lua_tostring(L, -1),wxConvLocal);
	}
	else
	{
		title = wxString(lua_tostring(L, 1),wxConvLocal);
		message = wxString(lua_tostring(L, 2),wxConvLocal);
	}

	new MerryInformationDialog(title, message);
	return 0;
}

static int LuaEnterKey(lua_State* L)
{
	wxArrayString args;
	for (int i=1; i<=lua_gettop(L); ++i)
		args.push_back(wxString(lua_tostring(L, i), wxConvLocal));
	g_controller->EnterKey(args);
	return 0;
}

static int LuaEnterText(lua_State* L)
{
	wxString text(lua_tostring(L, 1), wxConvLocal);
	g_controller->EnterText(text);
	return 0;
}

static int LuaSetTimer(lua_State* L)
{
	if (!lua_isfunction(L, 3))
		luaL_error(L, "can't find the timer callback");

	int millisecond = lua_tonumber(L, 1);
	bool oneShot = lua_toboolean(L, 2) != 0;
	lua_pushvalue(L, 3);
	void* handler = g_timers->SetTimer(millisecond, oneShot, luaL_ref(L, LUA_REGISTRYINDEX));
	lua_pushlightuserdata(L, handler);

	return 1;
}

static int LuaClearTimer(lua_State* L)
{
	g_timers->ClearTimer(lua_touserdata(L, 1));
	return 0;
}

static void GetSubDir(const wxString& dirname,wxArrayString *files,const wxString& filespec = wxEmptyString,const int sub = -1)
{
	static int cur_sub_dir = 0;
	wxDir dir(dirname);
	if (!dir.IsOpened())
		return;
	dir.GetAllFiles(dirname,files,filespec,wxDIR_HIDDEN|wxDIR_FILES);
	if (cur_sub_dir == sub)
		return;
	++cur_sub_dir;//子目录级别+1
	wxString file;
	bool test = dir.GetFirst(&file,wxEmptyString,wxDIR_DIRS|wxDIR_HIDDEN);
	wxString path = dir.GetNameWithSep();
	while(test)
	{
		GetSubDir(path+file,files,filespec,sub);
		test = dir.GetNext(&file);
	}
	--cur_sub_dir;//子目录级别-1
	return;
}

static int LuaDir(lua_State* L)
{
	wxString ls_path,filespec;
	int sub = -1;
	int top = lua_gettop(L);
	switch(top)
	{
		case 3:
			 if (lua_isnumber(L, 3))
				 sub = lua_tointeger(L,3);
		case 2:
			if (lua_isstring(L,2))
				filespec = wxString(lua_tostring(L, 2), wxConvLocal);
		case 1:
			if (lua_isstring(L,1))
				ls_path = wxString(lua_tostring(L,1), wxConvLocal);
			break;
	}
	wxArrayString files;
	GetSubDir(ls_path,&files,filespec,sub);
	lua_newtable(L);
	int i = files.Count();
	for(int j = 0;j<i;++j)
	{
		lua_pushinteger(L,j);
		lua_pushstring(L,files[j]);
		lua_settable(L,-3);
	}
	files.Clear();
	return 1;
}

static int LuaSetClipboardData(lua_State* L)
{
	return 0;
}

static int LuaGetClipboardData(lua_State* L)
{
	return 0;
}

static int LuaSetEnv(lua_State* L)
{
	wxString var(lua_tostring(L, 1), wxConvLocal);
	wxString value(lua_tostring(L, 2), wxConvLocal);
	lua_pushboolean(L,::wxSetEnv(var,value));
	return 1;
}

static int LuaReLoadConfig(lua_State* L)
{
	return 0;
}

static int LuaGetEnv(lua_State* L)
{
	wxString var(lua_tostring(L, 1), wxConvLocal);
	if (!::wxGetEnv(var,&var))
		var.Clear();
	lua_pushstring(L,var.c_str());
	return 1;
}

static int LuaConfig(lua_State* L)
{
	if (!lua_istable(L, 1))
		return 0;
	lua_pushstring(L, "CompareMode");
	lua_rawget(L, 1);
	if (lua_isnumber(L,-1))
		Ex_CompareMode = lua_tointeger(L,-1);
	lua_pushstring(L, "ROOT");
	lua_rawget(L, 1);
	if (lua_isstring(L,-1))
	{
		wxString root(wxString(lua_tostring(L, -1), wxConvLocal));
		if (::wxDirExists(root))
		{
			::wxSetEnv(wxT("ALMRUN_ROOT"),root.c_str());
			::wxSetWorkingDirectory(root);
		}
	}
	return 0;
}
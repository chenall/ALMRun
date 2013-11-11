#include "MerryCommandManager.h"
#include "MerryError.h"


#include "MerryInformationDialog.h"
#include  <algorithm>

MerryCommandManager* g_commands = NULL;
static wxString cmdPrefix;
MerryCommandManager::~MerryCommandManager()
{
	__DEBUG_BEGIN("")
	clearCmds(plugin_commands);
	clearCmds(m_commands);
	__DEBUG_END("")
}

void MerryCommandManager::clearCmds(MerryCommandArray& cmds)
{
	for(size_t i=0;i<cmds.size();++i)
		delete cmds[i];
	cmds.clear();
}
const void MerryCommandManager::AddFiles(const wxArrayString& files)
{
	for(int i=files.GetCount()-1;i >= 0;--i)
	{
		m_commands.push_back(new MerryCommand(m_commands.size() | (CMDS_FLAG_DIRS<<16),wxFileNameFromPath(files[i]),wxEmptyString,wxString::Format("\"%s\"",files[i])));
	}
}

const void MerryCommandManager::AddFiles(const wxArrayString& files,const wxArrayString& excludes)
{
	if (excludes.empty())
		return AddFiles(files);
	for(int i=files.GetCount()-1;i >= 0;--i)
	{
		int j;
		for(j = excludes.size()-1;j >= 0;--j)
		{//过滤符合条件的内容
			if (files[i].Matches(excludes[j]))
				break;
		}
		if (j == -1)
			m_commands.push_back(new MerryCommand(m_commands.size() |(CMDS_FLAG_DIRS<<16),wxFileNameFromPath(files[i]),wxEmptyString,wxString::Format("\"%s\"",files[i])));
	}
}

bool MerryCommandManager::DelCommand(int commandID)
{
	if (GetCommand(commandID))
	{
		m_commands[commandID] = new MerryCommand(-1,wxEmptyString);
		return true;
	}
	return false;
}

const int MerryCommandManager::AddCommand(const wxString& commandName,const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey,int flags)
{
	if (m_commands.size() >= 10000)
	{
		MerrySetLastError(wxT("\n超过10000个命令限制，目前限制命令数量不可以超过10000个，有特殊需求请联系我或到到网站留言 http://chenall.net"));
		return -2;
	}

	if (commandName.empty() && triggerKey.empty())
	{
		MerrySetLastError(wxT("\n热键(KEY)或名称(NAME),至少需要设置一个"));
		return -1;
	}
	for (size_t i=0; i<m_commands.size(); ++i)
	{
		const MerryCommand* command = m_commands[i];
		assert(command);
		if (!commandName.empty() && commandName.IsSameAs(command->GetCommandName(), false))
		{
			MerrySetLastError(wxString::Format(wxT("\n命令[%s]已经存在\n\n%s"),commandName,command->GetDetails()));
			return -1;
		}
		if (!triggerKey.empty() && triggerKey.IsSameAs(command->GetTriggerKey(), false))
		{
			MerrySetLastError(wxString::Format(wxT("\n命令[%s]热键重复\n\n%s\n"), commandName,command->GetDetails()));
			return -1;
		}
	}
	MerryCommand* command = new MerryCommand(m_commands.size() | (flags<<16), commandName,commandDesc,commandLine, funcRef, triggerKey);
	m_commands.push_back(command);
	return command->GetCommandID();
}

const MerryCommand* MerryCommandManager::GetCommand(int commandID) const
{
	if (commandID >= (int)m_commands.size() || commandID < 0)
		return NULL;
	assert(m_commands[commandID]);
	return m_commands[commandID];
}

/*
	命令排序
	1.根据排序值m_order,值越大排在越前面.
	2.前缀匹配优先
	3.命令名排序
*/
static bool mysort(MerryCommand *s1,MerryCommand  *s2)
{
	int cmp  = s1->GetOrder() - s2->GetOrder();
	if (cmp == 0)//排序值一样时
	{
		if (s1->m_compare == 0)//前缀匹配优先
		{
			if (s2->m_compare != 0)
				return true;
		}
		else if (s2->m_compare == 0)
			return false;
		return s2->GetCommandName().Upper() > s1->GetCommandName().Upper();
	}
	return cmp > 0;
}
/*
	命令排序
	1.前缀匹配优先
	2.根据排序值m_order,值越大排在越前面.
	3.命令名排序
*/
static bool SortPreOrder(MerryCommand *s1,MerryCommand  *s2)
{
	if (s1->m_compare == 0)//前缀匹配
	{
		if (s2->m_compare != 0)//前缀不匹配
			return true;
		int cmp = s1->GetOrder() - s2->GetOrder();
		if (cmp != 0)
			return cmp >0 ;
	}
	else if (s2->m_compare == 0)
		return false;
	return s2->GetCommandName().Upper() > s1->GetCommandName().Upper();
}

//初始排序
static bool command_sort(MerryCommand *s1,MerryCommand  *s2)
{
	int cmp = s1->GetOrder() - s2->GetOrder();
	if (cmp == 0)
		return s2->GetCommandName().Upper() > s1->GetCommandName().Upper();
	return cmp > 0;
}

void MerryCommandManager::AddPluginCmd(lua_State* L)
{
	if (!lua_istable(L, -1))
		return;
	int it = lua_gettop(L);

	lua_pushstring(L, "name");
	lua_rawget(L, it);
	wxString commandName(wxString(lua_tostring(L, -1), wxConvLocal));
	if (commandName.empty())
	{
		lua_settop(L,it);
		return;
	}

	lua_pushstring(L, "desc");
	lua_rawget(L, it);
	wxString commandDesc(wxString(lua_tostring(L, -1), wxConvLocal));

	int funcRef = 0;
	wxString commandLine;
	lua_pushstring(L, "cmd");
	lua_rawget(L, it);
	if (lua_isfunction(L,-1))
		funcRef = luaL_ref(L, LUA_REGISTRYINDEX);
	else
		commandLine = wxString(lua_tostring(L, -1), wxConvLocal);

	lua_pushstring(L, "order");
	lua_rawget(L, it);
	int order = lua_tointeger(L,-1);
	lua_settop(L,it);
	plugin_commands.push_back(new MerryCommand(-1,commandName, commandDesc,commandLine,funcRef,wxEmptyString,order));
	return;
}

void MerryCommandManager::GetPluginCmd(const wxString& name)
{
	if (!g_lua)
		return;
	clearCmds(plugin_commands);
	lua_State* L = g_lua->GetLua();
	lua_getglobal(L, "plugin_command");
	if (!lua_isfunction(L, 1))
		goto lua_pop;

	lua_pushstring(L, name.c_str());
	if (lua_pcall(L, 1, 1, 0))
		goto lua_pop;
	int it=lua_gettop(L);
	lua_pushnil(L);                               // ？？
    while(lua_next(L, it))                         // 开始枚举，并把枚举到的值压入栈
    {
		this->AddPluginCmd(L);
        lua_pop(L, 1);                              // 将Item从栈里面弹出
    }

	sort(plugin_commands.begin(),plugin_commands.end(),command_sort);

	lua_pop://恢复
	lua_pop(L,1);
	return;
}
MerryCommandArray MerryCommandManager::Collect(const wxString& commandPrefix)
{
	MerryCommandArray commands;
	MerryCommandArray l_commands;
	bool test_cmp = false;
	int cmp_find = -1;
	for (size_t i=0; i<m_commands.size(); ++i)
	{
		if (m_commands[i]->GetCommandName().empty())
			continue;
		l_commands.push_back(m_commands[i]);
	}
	sort(l_commands.begin(),l_commands.end(),command_sort);
	cmdPrefix = commandPrefix.Upper().Trim();
	if (cmdPrefix.empty())
		return l_commands;
	if (commandPrefix.empty())
		return l_commands;
	for (size_t i=0; i<l_commands.size(); ++i)
	{
		MerryCommand* command = l_commands[i];
		assert(command);
		const wxString& commandName = command->GetCommandName(0);
		if (commandName.size() < commandPrefix.size())
			continue;
	#ifdef _ALMRUN_CONFIG_H_
		switch(g_config->CompareMode)
		{
			case 1:
				test_cmp = (commandName.Upper().compare(0, commandPrefix.size(), cmdPrefix) == 0);
				break;
			case 2:
				test_cmp = g_lua->onCompare(commandName.c_str(),commandPrefix.c_str());
				break;
			default:
	#endif//ifdef _ALMRUN_CONFIG_H_
				cmp_find = commandName.Upper().find(cmdPrefix);
				if (cmp_find == wxNOT_FOUND)
					test_cmp = false;
				else
				{
					test_cmp = true;
					int cmdLen = command->GetCommandName().Len();
					if (cmp_find > cmdLen)
						cmp_find -= cmdLen + 1;
				}
	#ifdef _ALMRUN_CONFIG_H_
				break;
		}
	#endif//ifdef _ALMRUN_CONFIG_H_
		if (test_cmp)
		{
			command->m_compare = cmp_find;
			commands.push_back(command);
			if (g_config->config[ShowTopTen] && commands.size() >= 10 && !g_config->config[OrderByPre])
				break;
		}
	}
#ifdef _ALMRUN_CONFIG_H_
	sort(commands.begin(),commands.end(),g_config->config[OrderByPre]?SortPreOrder:mysort);
	if (g_config->config[ShowTopTen] && commands.size() >= 10)
		commands.resize(10);
	if (commands.size() < 9)
	{
		this->GetPluginCmd(commandPrefix);
		for(size_t i=0;i<plugin_commands.size();++i)
			commands.push_back(plugin_commands[i]);
	}
#endif//ifdef _ALMRUN_CONFIG_H_
	return commands;
}
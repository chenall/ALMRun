#include "MerryCommandManager.h"
#include "MerryError.h"
#include "MerryLua.h"
#include "MerryInformationDialog.h"
#include "ALMRunConfig.h"
#include  <algorithm>

MerryCommandManager* g_commands = NULL;
static wxString cmdPrefix;
MerryCommandManager::~MerryCommandManager()
{
	__DEBUG_BEGIN("")
	for (size_t i=0; i<m_commands.size(); ++i)
		delete m_commands[i];
	__DEBUG_END("")
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

const int MerryCommandManager::AddCommand(const wxString& commandName,const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey,int order,int flags)
{
	int order_id = 0;
	if (m_commands.size() >= 1000)
	{
		MerrySetLastError(wxT("\n超过1000个命令限制，目前限制命令数量不可以超过1000个，有特殊需求请联系我或到到网站留言 http://chenall.net"));
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
		if (command->m_order > order_id)
			order_id = i;
	}
	MerryCommand* command = new MerryCommand(m_commands.size() | (flags<<16), commandName,commandDesc,commandLine, funcRef, triggerKey,order);
#if 1
	m_commands.push_back(command);
#else
	if (order == 0)
		m_commands.push_back(command);
	else
		m_commands.insert(m_commands.begin()+order_id,command);
#endif
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
	int cmp  = s1->m_order - s2->m_order;
	if (cmp == 0)//排序值一样时
	{
		if (s1->m_compare == 0)//前缀匹配优先
		{
			if (s2->m_compare != 0)
				return true;
		}
		else if (s2->m_compare == 0)
			return false;
		return s2->GetCommandName() > s1->GetCommandName();
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
		int cmp = s1->m_order - s2->m_order;
		if (cmp != 0)
			return cmp >0 ;
	}
	else if (s2->m_compare == 0)
		return false;
	return s2->GetCommandName() > s1->GetCommandName();
}

const int MerryCommandManager::SetCmdOrder(int commandID)
{
	assert(m_commands[commandID]);
	MerryCommand* cmd= m_commands[commandID];
	wxString commandName = cmd->GetCommandName();
	if (commandName.empty() || !g_lua)
		return ++cmd->m_order;
	lua_State* L = g_lua->GetLua();
	assert(L);
	lua_getglobal(L, "SetCmdOrder");
	if (!lua_isnil(L, 1))
	{
		lua_pushstring(L, commandName.c_str());
		lua_pushinteger(L, cmd->m_order);
		if (lua_pcall(L, 2, 1, 0) == 0)
			cmd->m_order = lua_tointeger(L,-1);

	}
	lua_pop(L, 1);
	commandName.Clear();
	return cmd->m_order;
}

MerryCommandArray MerryCommandManager::Collect(const wxString& commandPrefix) const
{
	MerryCommandArray commands;
	bool test_cmp = false;
	int cmp_find = -1;
	cmdPrefix = commandPrefix.Upper();
	for (size_t i=0; i<m_commands.size(); ++i)
	{
		MerryCommand* command = m_commands[i];
		assert(command);
		const wxString& commandName = command->GetCommandName(0);
		if (commandName.empty())
			continue;
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
		#ifdef _ALMRUN_CONFIG_H_
			if (g_config->config[ShowTopTen] && commands.size()>9)
				break;
		#endif//ifdef _ALMRUN_CONFIG_H_
		}
	}
#ifdef _ALMRUN_CONFIG_H_
	sort(commands.begin(),commands.end(),g_config->config[OrderByPre]?SortPreOrder:mysort);
#endif
	return commands;
}

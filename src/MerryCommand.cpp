#include "MerryCommand.h"
#include "MerryCommandManager.h"
#include "MerryLua.h"
#include "MerryInformationDialog.h"
#include "DlgParam.h"
#include "ALMRunConfig.h"
//
const MerryCommand* LastCmd;

MerryCommand::MerryCommand(int commandID, const ALMRunCMDBase* cmd)
{
	m_commandID = commandID;
	m_flags = cmd->Flags;
	m_commandName = cmd->Name;
	m_commandFunc = cmd->FuncRef;
	m_commandDesc = cmd->Desc;
	m_triggerKey = cmd->Key;
	m_commandLine = cmd->cmdLine;
	m_order = cmd->Order;

	conf_cmd();
}

 MerryCommand::MerryCommand(int commandID, const wxString& commandName, const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey,const int order)
{
	m_commandID = commandID & 0xFFFF;
	m_flags = commandID >> 16;
	m_commandName = commandName;
	m_commandFunc = funcRef;
	m_triggerKey = triggerKey;
	m_commandDesc = commandDesc;
	m_commandLine = commandLine;
	m_order = order;
	conf_cmd();
}

MerryCommand::~MerryCommand()
{
	if (!g_lua)
		return;

	lua_State* L = g_lua->GetLua();
	assert(L);
	luaL_unref(L, LUA_REGISTRYINDEX, m_commandFunc);
}

void MerryCommand::conf_cmd()
{
	if (m_commandName.empty())
	{
		m_commandFName = m_commandName;
		return;
	}
	m_commandFName.sprintf("%s\n%s",m_commandName,GetPinYin(m_commandName));
	if (m_flags == CMDS_FLAG_PLUGIN)
		return;
	#ifdef _ALMRUN_CONFIG_H_
	//获取排序值信息
	if (g_config && g_config->order_conf)
		m_order = g_config->order_conf->ReadLong(m_commandName,0);
	#endif
}

wxString MerryCommand::GetDetails() const
{
	wxString cmd_from;
	int cmdId = m_flags >> 4;
	if (m_flags == CMDS_FLAG_PLUGIN)
	{
		cmd_from = wxT("LUA插件扩展");
		cmdId = m_commandID;
	}
	else if (m_flags & CMDS_FLAG_DIRS)
	{
		cmd_from = wxT("目录扫描(ALMRUN.INI)");
	}
	else if (m_flags & CMDS_FLAG_ALTRUN)
	{
		cmd_from = wxT("ALTRun配置文件");
	}
	else if (m_flags & CMDS_FLAG_LUA)
	{
		cmd_from = wxT("LUA脚本扩展");
	}
	else if (m_flags & CMDS_FLAG_CMDS)
	{
		cmd_from = wxT("ALMRun命令(ALMRUN.INI)");
	}
	else
	{
		cmd_from = wxT("未知配置");
	}
	return wxString::Format(wxT("配置文件：%s\nID:[%d] %s\n命令:%s\n热键: %s\n"),cmd_from,cmdId,this->m_commandDesc,this->m_commandLine,this->m_triggerKey);
}

wxString MerryCommand::GetCmd() const
{
	if (!m_commandLine.empty())
		return m_commandLine;
	if (m_commandFunc)
		return wxT("执行LUA命令");
	return wxEmptyString;
}

int MerryCommand::SetOrder()
{
	static time_t chgtime;
#ifdef _ALMRUN_CONFIG_H_
	if (g_config && !m_commandName.empty())
		return (m_order = g_config->SetcmdOrder(m_commandName));
#endif
	return ++m_order;
}

int MerryCommand::GetOrder() const
{
	return m_order;
}

void MerryCommand::Execute(const wxString& commandArg) const
{

	if (!g_lua)
		return;

	lua_State* L = g_lua->GetLua();
	wxString cmdArg = commandArg;
	assert(L);
	if (m_commandLine.size() && cmdArg.empty() && (m_commandLine[0] == '+' || m_commandLine.Find("{%p+}") != wxNOT_FOUND))
	{
		DlgParam *dlg = new DlgParam(NULL,-1,m_commandName);
		if (dlg->ShowModal() == wxID_OK)
			cmdArg = dlg->getvalue();
		dlg->Destroy();
		dlg = NULL;
		if (cmdArg.empty())
			return;
	}

	if (m_commandFunc == 0)
	{
		lua_getglobal(L, "CmdCallFunc");
		if (lua_isnil(L, 1))
		{
			lua_pop(L, 1);
			if (RunCMD(m_commandLine,cmdArg))
				goto ExecuteEnd;
			return;
		}
		lua_pushstring(L, m_commandLine.c_str());
		lua_pushstring(L, cmdArg.c_str());
	}
	else if (m_commandFunc == -2)
	{
		if (m_commandLine == "LastCmd" && LastCmd)
			LastCmd->Execute(wxEmptyString);
		return;

	}
	else
	{
		if (m_commandFunc == -1)
			lua_getglobal(L, m_commandLine.c_str());
		else
			lua_rawgeti(L, LUA_REGISTRYINDEX, m_commandFunc);
		assert(lua_isfunction(L, -1));
		lua_pushstring(L, cmdArg.c_str());
		lua_pushnumber(L, m_commandID);
	}

	lua_pushnumber(L,0);

	if (lua_pcall(L, 3, 0, 0))
	{
		new MerryInformationDialog(
			wxString::Format(wxT("Command %s execute failed"), m_commandName),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}

	ExecuteEnd:
	if (m_commandName.empty() || m_flags == CMDS_FLAG_PLUGIN)
		return;

	const_cast<MerryCommand*>(this)->SetOrder();
	if (m_commandFunc != -1)
		LastCmd = this;
}

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
	m_commandWorkDir = cmd->WorkDir;
	m_order = cmd->Order;
	conf_cmd();
}

 MerryCommand::MerryCommand(int commandID, const wxString& commandName, const wxString& commandDesc,const wxString& commandLine,const wxString& workDir, int funcRef, const wxString& triggerKey,const int order)
{
	m_commandID = commandID & 0xFFFF;
	m_flags = commandID >> 16;
	m_commandName = commandName;
	m_commandFunc = funcRef;
	m_triggerKey = triggerKey;
	m_commandDesc = commandDesc;
	m_commandLine = commandLine;
	m_commandWorkDir = workDir;
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
	PID = 0;
	wxString luaCmd;
	BOOL x64 = IsX64();
	if (g_lua && m_commandLine.StartsWith("--LUA",&luaCmd))//是LUA脚本命令,需要转换
	{

		lua_State* L = g_lua->GetLua();
		int top = lua_gettop(L);
		m_commandLine.Clear();
		luaCmd.insert(0,"return function(args,cmdID)\n").append("\nend");

		if (!luaL_dostring(L,luaCmd) && lua_isfunction(L,-1))
		{//执行正常返回一个函数
			m_commandFunc = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		else
		{
			wxMessageBox("Error");
		}
		lua_settop(L,top);
	}
	else
	{
		if (g_config->get(cmdReadShortcut))
		{
			if (wxFileName(m_commandLine).GetExt().IsSameAs("lnk",true))
			{
				ALMRunCMDBase Lcmd;
				if (ReadShortcut(m_commandLine.c_str(),&Lcmd))
				{
					m_commandLine = Lcmd.cmdLine;
					if (m_flags & CMDS_FLAG_DIRS)
					{
						m_commandDesc = Lcmd.Desc;
						m_commandWorkDir = Lcmd.WorkDir;
					}
					else
					{
						if (m_commandDesc.IsEmpty()) m_commandDesc = Lcmd.Desc;
						if (m_commandWorkDir.IsEmpty()) m_commandWorkDir = Lcmd.WorkDir;
					}

				}
			}
		}
		if (!x64 && m_commandLine.find("x64") != wxNOT_FOUND)
		{
			m_commandID = -1;
			return;
		}
	}
	if (m_commandName.empty())
	{
		m_commandFName = m_commandName;
		return;
	}
	m_commandName.Trim();
	if (m_flags == CMDS_FLAG_PLUGIN)
	{
		size_t n = m_commandName.find('\1');
		if (n != wxNOT_FOUND)
		{
			m_commandFName = m_commandName.substr(0,n);
			m_commandName.Remove(0,n+1);
		}
		return;
	}

	m_commandFName.sprintf("%s\n%s",m_commandName,GetPinYin(m_commandName));
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
	#ifdef __WXMSW__
	if (PID > 1)//禁止多个进程
	{
		if (CheckActiveProg(PID))
		{//已经运行,查找前激活之前的窗口
			return;
		}
	}
	#endif

	if (!LocationExec && m_commandLine.size() && cmdArg.empty() && (m_commandLine[0] == '+' || m_commandLine.Find("{%p+}") != wxNOT_FOUND))
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
		if (!g_lua->get_func(LUA_CMDRun_FUNC))
		{
			const_cast<MerryCommand*>(this)->PID = RunCMD(m_commandLine,cmdArg,m_commandWorkDir);
			goto ExecuteEnd;
		}
		lua_pushstring(L, m_commandLine.c_str());
		lua_pushstring(L, cmdArg.c_str());
		lua_pushstring(L, m_commandWorkDir.c_str());
	}
	else if (m_commandFunc == LUA_NOREF)
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
		lua_pushnumber(L,0);
	}

	if (lua_pcall(L, 3, 1, 0))
	{
		new MerryInformationDialog(
			wxString::Format(wxT("Command %s execute failed"), m_commandName),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}

	const_cast<MerryCommand*>(this)->PID = lua_tointeger(L,-1);
	lua_pop(L,1);

	ExecuteEnd:
	if (m_commandName.empty() || m_flags == CMDS_FLAG_PLUGIN)
		return;

	const_cast<MerryCommand*>(this)->SetOrder();
	if (m_commandFunc != -1)
		LastCmd = this;
}

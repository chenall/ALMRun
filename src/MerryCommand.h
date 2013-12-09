#ifndef _MERRY_COMMAND_H_
#define _MERRY_COMMAND_H_
#include "MerryWx.h"
#include "ALMRunCommon.h"

#define CMDS_FLAG_CMDS 1
#define CMDS_FLAG_DIRS 2
#define CMDS_FLAG_LUA 4
#define CMDS_FLAG_ALTRUN 8
#define CMDS_FLAG_PLUGIN -1


class MerryCommand
{
public:
	MerryCommand(int commandID, const ALMRunCMDBase* cmd);
	MerryCommand(int commandID, const wxString& commandName,const wxString& commandDesc = wxEmptyString,const wxString& commandLine = wxEmptyString,const wxString& workDir = wxEmptyString, int funcRef = 0, const wxString& triggerKey = wxEmptyString,const int order = 0);
	~MerryCommand();

	int GetCommandID() const { return m_commandID; }
	int GetFlags() const { return m_flags; }
	const wxString& GetCommandName() const { return m_commandName; }
	const wxString& GetCommandName(const int) const { return m_commandFName; }
	const wxString& GetTriggerKey() const { return m_triggerKey; }
	const wxString& GetCommandDesc() const { return m_commandDesc; }
	const wxString& GetWorkDir() const{ return m_commandWorkDir;}
	wxString GetCmd() const;
	int GetOrder() const;
	int SetOrder();
	wxString GetDetails() const;
	void Execute(const wxString& commandArg) const;
	int m_compare;
private:
	void conf_cmd();
	DWORD PID;
	int m_order;
	int m_commandID;
	wxString m_commandName;
	wxString m_commandDesc;
	wxString m_commandLine;
	wxString m_commandFName;
	wxString m_commandWorkDir;
	int m_commandFunc;
	int m_flags;
	wxString m_triggerKey;
		
};
extern const MerryCommand* LastCmd;
#endif

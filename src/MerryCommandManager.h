#ifndef _MERRY_COMMAND_MANAGER_H_
#define _MERRY_COMMAND_MANAGER_H_

#include "MerryCommand.h"
#include "ALMRunConfig.h"
#include "MerryLua.h"
#include <vector>

typedef std::vector<MerryCommand*> MerryCommandArray;

class MerryCommandManager
{
public:
	~MerryCommandManager();
	const void AddFiles(const wxArrayString& files);
	const void AddFiles(const wxArrayString& files,const wxArrayString& excludes);
	const int AddCommand(const ALMRunCMDBase* cmd);
	const int AddCommand(const wxString& file);
	const int AddCommand(const wxString& commandName,const wxString& commandDesc,const wxString& commandLine = wxEmptyString,const wxString& commandWorkDir = wxEmptyString, int funcRef = 0, const wxString& triggerKey = wxEmptyString,int flags = 0);
	const MerryCommand* GetCommand(int commandID) const;
	bool DelCommand(int commandID);
	MerryCommandArray Collect(const wxString& commandPrefix);
	void GetPluginCmd(const wxString& name,MerryCommandArray& commands);
	void clearCmds(MerryCommandArray& cmds);
private:
//	void AddPluginCmd(lua_State* L);
	MerryCommandArray plugin_commands;
	MerryCommandArray m_commands;
};

extern MerryCommandManager* g_commands;
#endif

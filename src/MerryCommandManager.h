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
	const int AddCommand(const wxString& commandName,const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey,int flags = 0);
	const MerryCommand* GetCommand(int commandID) const;
	bool DelCommand(int commandID);
	MerryCommandArray Collect(const wxString& commandPrefix,int flags = 0);
	void clearCmds(MerryCommandArray& cmds);
	MerryCommandArray plugin_commands;
private:
	void GetPluginCmd(const wxString& name);
	void AddPluginCmd(lua_State* L);
	MerryCommandArray m_commands;
};

extern MerryCommandManager* g_commands;
#endif

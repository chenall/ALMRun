#include "MerryCommandManager.h"
#include "MerryError.h"
#include "MerryLua.h"
#include "MerryInformationDialog.h"

MerryCommandManager* g_commands = NULL;
int Ex_CompareMode;
MerryCommandManager::~MerryCommandManager()
{
	for (size_t i=0; i<m_commands.size(); ++i)
		delete m_commands[i];
}

const int MerryCommandManager::AddCommand(const wxString& commandName,const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey)
{
	if (m_commands.size() >= 1000)
	{
		::MerrySetLastError(wxT("1000 commands limit,If you need more please contact me!"));
		return -2;
	}

	if (commandName.empty() && triggerKey.empty())
	{
		//::MerrySetLastError(wxT("Command name and key not found"));
		return -1;
	}
	for (size_t i=0; i<m_commands.size(); ++i)
	{
		const MerryCommand* command = m_commands[i];
		assert(command);
		if (!commandName.empty() && commandName.IsSameAs(command->GetCommandName(), false))
		{
			new MerryInformationDialog(wxT("Command name exists"),wxString::Format(wxT("以下命令名称重复:\r\n%s"),commandName));
			//::MerrySetLastError(wxString::Format(wxT("Command name \"%s\" already exists[%d]"), commandName,command->GetCommandID()));
			return -1;
		}
		if (!triggerKey.empty() && triggerKey.IsSameAs(command->GetTriggerKey(), false))
		{
			new MerryInformationDialog(wxT("Command key exists"),wxString::Format(wxT("以下命令热键重复:\r\n%s"),triggerKey));
			//::MerrySetLastError(wxString::Format(wxT("Command key \"%s\" already exists[%d]"), triggerKey,command->GetCommandID()));
			return -1;
		}
	}
	MerryCommand* command = new MerryCommand(m_commands.size(), commandName,commandDesc,commandLine, funcRef, triggerKey);
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

MerryCommandArray MerryCommandManager::Collect(const wxString& commandPrefix) const
{
	MerryCommandArray commands;
	bool test_cmp = false;
	for (size_t i=0; i<m_commands.size(); ++i)
	{
		MerryCommand* command = m_commands[i];
		assert(command);
		const wxString& commandName = command->GetCommandName();
		if (commandName.empty())
			continue;
		if (commandName.size() < commandPrefix.size())
			continue;
		switch(Ex_CompareMode)
		{
			case 1:
				test_cmp = (commandName.Upper().compare(0, commandPrefix.size(), commandPrefix.Upper()) == 0);
				break;
			case 2:
				test_cmp = g_lua->onCompare(commandName.c_str(),commandPrefix.c_str());
				break;
			default:
				test_cmp = (commandName.Upper().find(commandPrefix.Upper()) != wxNOT_FOUND);
				break;
		}
		if (test_cmp)
			commands.push_back(command);
	}
	return commands;
}

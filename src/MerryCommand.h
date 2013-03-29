#ifndef _MERRY_COMMAND_H_
#define _MERRY_COMMAND_H_

#include "MerryWx.h"

class MerryCommand
{
public:
	MerryCommand(int commandID, const wxString& commandName,const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey);
	~MerryCommand();

	int GetCommandID() const { return m_commandID; }
	const wxString& GetCommandName() const { return m_commandName; }
	const wxString& GetTriggerKey() const { return m_triggerKey; }
	const wxString& GetCommandDesc() const { return m_commandDesc; }
	const wxString& GetCommandLine() const { return m_commandLine; }
	void ExecuteCommand(const wxString& commandArg) const;

private:
	int m_commandID;
	wxString m_commandName;
	wxString m_commandDesc;
	wxString m_commandLine;
	int m_commandFunc;
	wxString m_triggerKey;
};

#endif

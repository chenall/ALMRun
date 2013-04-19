#ifndef _MERRY_COMMAND_H_
#define _MERRY_COMMAND_H_
#include "MerryWx.h"

class MerryCommand
{
public:
	MerryCommand(int commandID, const wxString& commandName,const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey,int order);
	~MerryCommand();

	int GetCommandID() const { return m_commandID; }
	const wxString& GetCommandName() const { return m_commandName; }
	const wxString& GetCommandName(const int) const { return m_commandFName; }
	const wxString& GetTriggerKey() const { return m_triggerKey; }
	const wxString& GetCommandDesc() const { return m_commandDesc; }
	const wxString& GetCommandLine() const { return m_commandLine; }
	void ExecuteCommand(const wxString& commandArg) const;
	int m_order;
private:
	int m_commandID;
	wxString m_commandName;
	wxString m_commandDesc;
	wxString m_commandLine;
	wxString m_commandFName;
	int m_commandFunc;
	wxString m_triggerKey;
		
};

#endif

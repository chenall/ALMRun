#ifndef _MERRY_COMMAND_H_
#define _MERRY_COMMAND_H_
#include "MerryWx.h"
#include "ALMRunConfig.h"

class MerryCommand
{
public:
	MerryCommand(int commandID, const wxString& commandName,const wxString& commandDesc = wxEmptyString,const wxString& commandLine = wxEmptyString, int funcRef = 0, const wxString& triggerKey = wxEmptyString,int order = 0);
	~MerryCommand();

	int GetCommandID() const { return m_commandID; }
	const wxString& GetCommandName() const { return m_commandName; }
	const wxString& GetCommandName(const int) const { return m_commandFName; }
	const wxString& GetTriggerKey() const { return m_triggerKey; }
	const wxString& GetCommandDesc() const { return m_commandDesc; }
	const wxString& GetCommandLine() const { return m_commandLine; }
	void ExecuteCommand(const wxString& commandArg) const;
	int m_order;
	int m_compare;
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

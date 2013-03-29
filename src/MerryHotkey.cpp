#include "MerryHotkey.h"
#include "MerryCommandManager.h"
#include "MerryHelper.h"
#include "MerryError.h"

MerryHotkey* g_hotkey = NULL;

MerryHotkey::~MerryHotkey()
{

}

void MerryHotkey::OnDelete()
{
	std::set<int>::const_iterator it = m_registerCommands.begin();
	for (; it != m_registerCommands.end(); ++it)
	{
		int commandID = *it;
		this->OnUnregisterHotkey(commandID);
	}
}

bool MerryHotkey::RegisterHotkey(int commandID)
{
	if (m_registerCommands.find(commandID) != m_registerCommands.end())
	{
		::MerrySetLastError(wxString::Format(wxT("Command id %d already register"), commandID));
		return false;
	}
	const MerryCommand* command = g_commands->GetCommand(commandID);
	if (!command)
	{
		::MerrySetLastError(wxString::Format(wxT("Command id %d not found"), commandID));
		return false;
	}
	const wxString& triggerKey = command->GetTriggerKey();
	if (triggerKey.IsEmpty())
		return true;

	int modifiers, keyCode;
	if (!::MerryParseKeyStr(triggerKey, modifiers, keyCode))
	{
		::MerrySetLastError(wxString::Format(wxT("Parse command key %s failed"), triggerKey));
		return false;
	}
	if (!this->OnRegisterHotkey(commandID, modifiers, keyCode))
	{
		::MerrySetLastError(wxString::Format(wxT("Register command key %s failed"), triggerKey));
		return false;
	}

	bool isOk = m_registerCommands.insert(commandID).second;
	assert(isOk);

	return true;
}

void MerryHotkey::UnregisterHotkey(int commandID)
{
	this->OnUnregisterHotkey(commandID);
	m_registerCommands.erase(commandID);
}

void MerryHotkey::OnTriggerKey(int commandID) const
{
	const MerryCommand* command = g_commands->GetCommand(commandID);
	assert(command);
	command->ExecuteCommand(wxEmptyString);
}

#include "MerryHotkeyWx.h"

#ifdef MERRY_HOTKEY_WX

MerryHotkey* NewMerryHotkey()
{
	return new MerryHotkeyWx();
}

MerryHotkeyWx::MerryHotkeyWx():
	wxTopLevelWindow(NULL, wxID_ANY, wxT("MerryHotkey"))
{
	
}

MerryHotkeyWx::~MerryHotkeyWx()
{
	__DEBUG_BEGIN("")
	this->OnDelete();
	__DEBUG_END("")
}

bool MerryHotkeyWx::OnRegisterHotkey(int commandID, int modifiers, int keyCode)
{
	if (!this->RegisterHotKey(commandID, modifiers, keyCode))
		return false;
	this->Connect(commandID, wxEVT_HOTKEY, wxObjectEventFunction(&MerryHotkeyWx::OnTriggerKeyEvent));
	return true;
}

void MerryHotkeyWx::OnUnregisterHotkey(int commandID)
{
	this->Disconnect(commandID);
	this->UnregisterHotKey(commandID);
}

void MerryHotkeyWx::OnTriggerKeyEvent(wxKeyEvent& e)
{
	this->OnTriggerKey(e.GetId());
}

#endif

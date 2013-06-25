#ifndef _MERRY_HOTKEY_WX_H_
#define _MERRY_HOTKEY_WX_H_

#include "MerryHotkey.h"

#ifdef MERRY_HOTKEY_WX

class MerryHotkeyWx : public MerryHotkey, private wxTopLevelWindow
{
public:
	MerryHotkeyWx();
	virtual ~MerryHotkeyWx();

private:
	virtual bool OnRegisterHotkey(int commandID, int modifiers, int keyCode);
	virtual void OnUnregisterHotkey(int commandID);

private:
	void OnTriggerKeyEvent(wxKeyEvent& e);
};

#endif

#endif

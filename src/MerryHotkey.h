#ifndef _MERRY_HOTKEY_H_
#define _MERRY_HOTKEY_H_

#include "MerryWx.h"
#include <set>

#if defined(__WXMSW__) || defined(__WXOSX__)
#	define MERRY_HOTKEY_WX
#elif defined(__WXGTK__)
#	define MERRY_HOTKEY_X
#endif

class MerryHotkey
{
public:
	virtual ~MerryHotkey();
	void OnDelete();

	bool RegisterHotkey(int commandID);
	void UnregisterHotkey(int commandID);

protected:
	void OnTriggerKey(int commandID) const;

	virtual bool OnRegisterHotkey(int commandID, int modifiers, int keyCode) = 0;
	virtual void OnUnregisterHotkey(int commandID) = 0;

private:
	std::set<int> m_registerCommands;
};

extern MerryHotkey* g_hotkey;
extern MerryHotkey* NewMerryHotkey();

#endif

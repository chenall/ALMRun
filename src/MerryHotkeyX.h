#ifndef _MERRY_HOTKEY_X_H_
#define _MERRY_HOTKEY_X_H_

#include "MerryHotkey.h"

#ifdef MERRY_HOTKEY_X

#include <X11/Xlib.h>
#include <map>
#include <stdint.h>

class MerryHotkeyX : public MerryHotkey, private wxTimer
{
public:
	MerryHotkeyX();
	~MerryHotkeyX();

private:
	virtual bool OnRegisterHotkey(int commandID, int modifiers, int keyCode);
	virtual void OnUnregisterHotkey(int commandID);

	virtual void Notify();

	uint64_t MakeKeyCodeAndModifiers(int keyCode, int modifiers) const;
	void GetKeyCodeAndModifiers(uint64_t keyCodeAndModifiers, int& keyCode, int& modifiers) const;

private:
	Display* m_display;
	Window m_rootWindow;
	std::map<uint64_t, int> m_toCommandID;
	std::map<int, uint64_t> m_toKeyCodeAndModifiers;
};

#endif

#endif

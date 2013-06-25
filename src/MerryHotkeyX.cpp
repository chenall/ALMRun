#include "MerryHotkeyX.h"

#ifdef MERRY_HOTKEY_X

#include <X11/Xutil.h>
#include <wx/unix/utilsx11.h>

MerryHotkey* NewMerryHotkey()
{
	return new MerryHotkeyX();
}

MerryHotkeyX::MerryHotkeyX()
{
	m_display = XOpenDisplay(NULL);
	m_rootWindow = DefaultRootWindow(m_display);
	XSelectInput(m_display, m_rootWindow, KeyPressMask);

	this->Start(50);
}

MerryHotkeyX::~MerryHotkeyX()
{
	XCloseDisplay(m_display);
	this->OnDelete();
}

bool MerryHotkeyX::OnRegisterHotkey(int commandID, int modifiers, int keyCode)
{
	if (keyCode == 0)
		return false;

	int xModifiers = 0;
	if (modifiers & wxMOD_ALT)
		xModifiers |= Mod1Mask;
	else if (modifiers & wxMOD_CONTROL)
		xModifiers |= ControlMask;
	else if (modifiers & wxMOD_SHIFT)
		xModifiers |= ShiftMask;
	else if (modifiers & wxMOD_WIN)
		xModifiers |= 0;
	else if (modifiers & wxMOD_META)
		xModifiers |= 0;

	int xKeyCode = wxCharCodeWXToX(keyCode);
	xKeyCode = XKeysymToKeycode(m_display, xKeyCode);
	XGrabKey(m_display, xKeyCode, xModifiers, m_rootWindow, False, GrabModeAsync, GrabModeAsync);

	uint64_t keyCodeAndModifiers = this->MakeKeyCodeAndModifiers(xKeyCode, xModifiers);
	m_toCommandID[keyCodeAndModifiers] = commandID;
	m_toKeyCodeAndModifiers[commandID] = keyCodeAndModifiers;

	return true;
}

void MerryHotkeyX::OnUnregisterHotkey(int commandID)
{
	if (m_toKeyCodeAndModifiers.find(commandID) == m_toKeyCodeAndModifiers.end())
		return;

	uint64_t keyCodeAndModifiers = m_toKeyCodeAndModifiers[commandID];
	assert(m_toCommandID[keyCodeAndModifiers] == commandID);
	int keyCode, modifiers;
	this->GetKeyCodeAndModifiers(keyCodeAndModifiers, keyCode, modifiers);
	XUngrabKey(m_display, keyCode, modifiers, m_rootWindow);

	m_toCommandID.erase(keyCodeAndModifiers);
	m_toKeyCodeAndModifiers.erase(commandID);
}

void MerryHotkeyX::Notify()
{
	XEvent xEvent;
	if (XCheckWindowEvent(m_display, m_rootWindow, KeyPressMask, &xEvent))
	{
		uint64_t keyCodeAndModifiers = this->MakeKeyCodeAndModifiers(xEvent.xkey.keycode, xEvent.xkey.state);
		// assert(m_toCommandID.find(keyCodeAndModifiers) != m_toCommandID.end());
		if (m_toCommandID.find(keyCodeAndModifiers) != m_toCommandID.end())
			this->OnTriggerKey(m_toCommandID[keyCodeAndModifiers]);
	}
}

uint64_t MerryHotkeyX::MakeKeyCodeAndModifiers(int keyCode, int modifiers) const
{
	assert(sizeof(int) == 4);

	uint64_t keyCodeAndModifiers = 0;
	keyCodeAndModifiers |= (((uint64_t)keyCode & 0x00000000FFFFFFFF) << 32);
	keyCodeAndModifiers |= ((uint64_t)modifiers & 0x00000000FFFFFFFF);
	return keyCodeAndModifiers;
}

void MerryHotkeyX::GetKeyCodeAndModifiers(uint64_t keyCodeAndModifiers, int& keyCode, int& modifiers) const
{
	keyCode = (keyCodeAndModifiers >> 32);
	modifiers = keyCodeAndModifiers;
}

#endif

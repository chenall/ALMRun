#ifndef _MERRY_KEY_H_
#define _MERRY_KEY_H_

#include "MerryWx.h"
#include <map>

class MerryKey
{
public:
	MerryKey();
	int GetWxKeyCode(const wxString& key) const;
#ifdef __WXMSW__
	int GetWindowsKeyCode(const wxString& key) const;
#endif

private:
	std::map<wxString, int> m_wxKeys;
#ifdef __WXMSW__
	std::map<wxString, int> m_windowsKeys;
#endif
};

extern MerryKey g_keys;

#endif

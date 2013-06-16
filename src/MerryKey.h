#ifndef _MERRY_KEY_H_
#define _MERRY_KEY_H_

#include "MerryWx.h"
#include <map>
#include <algorithm>
class map_value_finder
{
public:
       map_value_finder(const int key):code(key){}
       bool operator ()(const std::map<wxString, int>::value_type &pair)
       {
            return pair.second == code;
       }
private:
        const int code;
};

class MerryKey
{
public:
	MerryKey();
	int GetWxKeyCode(const wxString& key) const;
#ifdef __WXMSW__
	int GetWindowsKeyCode(const wxString& key) const;
	wxString GetKeyString(int key) const;
#endif

private:
	std::map<wxString, int> m_wxKeys;
#ifdef __WXMSW__
	std::map<wxString, int> m_windowsKeys;
#endif
};

extern MerryKey g_keys;

#endif

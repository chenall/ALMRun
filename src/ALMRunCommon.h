#pragma once
#ifndef _ALMRUN_COMMON_H_
#define _ALMRUN_COMMON_H_
#include <shlobj.h>
#include <wx/fileconf.h>
#include "MerryWx.h"
#include "MerryLua.h"

class ALMRunCMDBase
{
	public:
		wxString Name;
		wxString Desc;
		wxString cmdLine;
		wxString Key;
		int FuncRef;
		int Order;
		int Flags;
};

BOOL CreateFileShortcut(LPCWSTR lpszFileName, LPCWSTR lpszLnkFilePath, LPCWSTR lpszWorkDir, WORD wHotkey = 0, LPCTSTR lpszDescription = NULL, int iShowCmd = SW_SHOWNORMAL);
void ListFiles(const wxString& dirname,wxArrayString *files,const wxArrayString& filespec,const int sub = -1);
void ListFiles(const wxString& dirname,wxArrayString *files,const wxString& filespec,const wxString& exclude  = wxEmptyString,const int sub = -1);
ALMRunCMDBase* lua_GetCommand(lua_State* L, int flags = 0);
wxString GetPinYin(const wxString& source);
wxString GetClipboardText();
#ifdef __WXMSW__

wxString GetCMDPath(const wxString& commandLine,const wxString& workingDir = wxEmptyString);
wxString ParseCmd(const wxString& cmdLine,wxString* const cmdArg = NULL);
bool RunCMD(const wxString& cmdLine);

#endif

#endif
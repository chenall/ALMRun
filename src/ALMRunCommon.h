#pragma once
#ifndef _ALMRUN_COMMON_H_
#define _ALMRUN_COMMON_H_
#include <shlobj.h>
#include <wx/fileconf.h>
#include "MerryWx.h"
#include "MerryLua.h"
#include "wx/sstream.h"
#include "wx/url.h"

class ALMRunCMDBase
{
	public:
		ALMRunCMDBase(const wxString& commandName = wxEmptyString,const wxString& commandDesc = wxEmptyString,const wxString& commandLine = wxEmptyString,const wxString& commandworkDir = wxEmptyString, int funcRef = 0, const wxString& triggerKey = wxEmptyString,const int order = 0)
		{
			this->Name = commandName;
			this->Desc = commandDesc;
			this->cmdLine = commandLine;
			this->WorkDir = commandworkDir;
			this->Key = triggerKey;
			this->FuncRef = funcRef;
			this->Order = order;
		}

		wxString Name;
		wxString Desc;
		wxString cmdLine;
		wxString Key;
		wxString WorkDir;
		int FuncRef;
		int Order;
		int Flags;
};

const char CMDFLAG_STRS[]="?@+>*| ";
void ListFiles(const wxString& dirname,wxArrayString *files,const wxArrayString& filespec,const int sub = -1);
void ListFiles(const wxString& dirname,wxArrayString *files,const wxString& filespec,const wxString& exclude  = wxEmptyString,const int sub = -1);
ALMRunCMDBase* lua_GetCommand(lua_State* L, int flags = 0);
wxString GetPinYin(const wxString& source);
wxString GetClipboardText();
int importCMD(wxString& filename);
wxString GetCMDPath(const wxString& commandLine,const wxString& workingDir = wxEmptyString);
wxString UnEscapeString(const wxString& str);
wxString EscapeString(const wxString& str);
void setWinHelpText(wxWindowBase* win,const wxString& text,bool ShowToolTips = true);
wxString wxURL_GET(const wxString &uri,const wxString& proxy = wxEmptyString);

#ifdef __WXMSW__
BOOL IsX64();
BOOL ActiveWindow(HWND hwnd);
BOOL CheckActiveProg(DWORD PID);
static wxString _GetCMDPath(const wxString& commandLine);
wxString ParseCmd(const wxString& cmdLine,wxString* const cmdArg = NULL,const wxString& workDir = wxEmptyString);
DWORD RunCMD(const wxString& cmdLine,const wxString& cmdArg,const wxString& workDir = wxEmptyString);
BOOL CreateFileShortcut(LPCWSTR lpszFileName, LPCWSTR lpszLnkFilePath, LPCWSTR lpszWorkDir, WORD wHotkey = 0, LPCTSTR lpszDescription = NULL, int iShowCmd = SW_SHOWNORMAL);
BOOL ReadShortcut(LPCWSTR lpwLnkFile, ALMRunCMDBase *cmd);
#endif

#endif
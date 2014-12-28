#include "MerryController.h"

#ifdef __WXMSW__
#include "ALMRunConfig.h"
#include "MerryInformationDialog.h"
#include "MerryHelper.h"
#include <vector>
#include <Windows.h>

MerryController::MerryController()
{
	
}

MerryController::~MerryController()
{
	
}

void MerryController::AddInputKey(std::vector<INPUT>& input, int keyCode, bool isDown) const
{
	INPUT tmpInput;
	memset(&tmpInput, 0, sizeof(tmpInput));

	if (keyCode == VK_LBUTTON || keyCode == VK_RBUTTON || keyCode == VK_MBUTTON)
	{
		tmpInput.type = INPUT_MOUSE;
		int x, y;
		this->GetMousePosition(x, y);
		tmpInput.mi.dx = x;
		tmpInput.mi.dy = y;
		if (keyCode == VK_LBUTTON)
			tmpInput.mi.dwFlags = isDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
		else if (keyCode == VK_RBUTTON)
			tmpInput.mi.dwFlags = isDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
		else if (keyCode == VK_MBUTTON)
			tmpInput.mi.dwFlags = isDown ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
	}
	else
	{
		WORD wScan = 0;
		tmpInput.type = INPUT_KEYBOARD;
		tmpInput.ki.dwFlags = isDown ? 0 : KEYEVENTF_KEYUP;
		tmpInput.ki.wVk = keyCode;
		switch(keyCode)
		{
			case VK_LSHIFT:// 0xA0
				wScan = 0x2a;
				break;
			case VK_RSHIFT:// 0xA1
				wScan = 0x36;
				break;
			case VK_LCONTROL:// 0xA2
				wScan = 0x1d;
				break;
			case VK_RCONTROL:// 0xA3
				wScan = 0xe01d;
				break;
			case VK_LMENU:// 0xA4
			case VK_RMENU:// 0xA5
			default:
				break;

		}
		if (wScan)
		{
			if (!isDown)
				wScan |= 0x80;
			tmpInput.ki.dwFlags |= KEYEVENTF_SCANCODE;
			tmpInput.ki.wScan = wScan;
		}
	}
	
	input.push_back(tmpInput);
}

void* MerryController::GetForegroundWindow() const
{
	return ::GetForegroundWindow();
}

void MerryController::SetForegroundWindow(void* window) const
{
	::SetForegroundWindow((HWND)window);
}

void MerryController::ShowWindow(void* window, const wxString& show) const
{
	int cmdShow = SW_SHOW;
	if (show.IsSameAs(wxT("hide"), false))
		cmdShow = SW_HIDE;
	else if (show.IsSameAs(wxT("min"), false))
		cmdShow = SW_SHOWMINIMIZED;
	else if (show.IsSameAs(wxT("max"), false))
		cmdShow = SW_SHOWMAXIMIZED;
	else if (show.IsSameAs(wxT("restore"), false))
		cmdShow = SW_RESTORE;

	::ShowWindow((HWND)window, cmdShow);
}

void MerryController::CloseWindow(void* window) const
{
	::PostMessage((HWND)window, WM_CLOSE, 0, 0);
}

bool MerryController::IsWindowMax(void* window) const
{
	return ::IsZoomed((HWND)window) == TRUE;
}

bool MerryController::IsWindowMin(void* window) const
{
	return ::IsIconic((HWND)window) == TRUE;
}

bool MerryController::IsWindowShown(void* window) const
{
	return ::IsWindowVisible((HWND)window) == TRUE;
}

wxString MerryController::GetWindowText(void* window) const
{
	const int TEXT_BUFFER_LEN = 512;
	wxChar textBuffer[TEXT_BUFFER_LEN];
	::GetWindowText((HWND)window, textBuffer, TEXT_BUFFER_LEN);
	return textBuffer;
}

void MerryController::SetWindowText(void* window, const wxString& text) const
{
	::SetWindowText((HWND)window, text.c_str());
}

void MerryController::GetWindowSize(void* window, int& width, int& height) const
{
	RECT r;
	if (!::GetWindowRect((HWND)window, &r))
	{
		width = 0;
		height = 0;
		return;
	}

	width = r.right - r.left;
	height = r.bottom - r.top;
}

void MerryController::SetWindowSize(void* window, int width, int height) const
{
	RECT r;
	if (!::GetWindowRect((HWND)window, &r))
		return;
	::MoveWindow((HWND)window, r.left, r.top, width, height, ::IsWindowVisible((HWND)window));
}

void MerryController::GetWindowPosition(void* window, int& x, int& y) const
{
	RECT r;
	if (!::GetWindowRect((HWND)window, &r))
	{
		x = 0;
		y = 0;
		return;
	}

	x = r.left;
	y = r.top;
}

void MerryController::SetWindowPosition(void* window, int x, int y) const
{
	RECT r;
	if (!::GetWindowRect((HWND)window, &r))
		return;
	::MoveWindow((HWND)window, x, y, r.right - r.left, r.bottom - r.top, ::IsWindowVisible((HWND)window));
}

void* MerryController::FindWindow(const wxString& ClassName,const wxString& WindowName) const
{
	return this->FindWindowEx(NULL,NULL,ClassName,WindowName);
//	return ::FindWindow(ClassName.c_str(),WindowName.c_str());
}

void*  MerryController::FindWindowEx(void* Parent,void *Child,const wxString& ClassName,const wxString&  WindowName) const
{
    const wchar_t *className = NULL;
	const wchar_t *windowName = NULL;
	if (!ClassName.IsEmpty())
		className = ClassName.c_str();
	if (!WindowName.IsEmpty())
		windowName = WindowName.c_str();
	return ::FindWindowEx((HWND)Parent,(HWND)Child,className,windowName);
}

BOOL MerryController::SetWindowPos(void *hWnd,void *hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags) const
{
	return ::SetWindowPos((HWND)hWnd,(HWND)hWndInsertAfter,X,Y,cx,cy,uFlags);
}

void MerryController::EnterKey(const wxArrayString& keys)
{
	const int WIN_MOD[] = { VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN };
	const int WIN_MOD_NUM = sizeof(WIN_MOD) / sizeof(int);

	std::vector<INPUT> input;
	for (int i=0; i<WIN_MOD_NUM; ++i)
		this->AddInputKey(input, WIN_MOD[i], false);

	for (size_t i=0; i<keys.size(); ++i)
	{
		const wxString& key = keys[i];
		int modifiers, keyCode;
		if (!::MerryParseKeyStr(key, modifiers, keyCode))
		{
			new MerryInformationDialog(wxT("enterKey"),
				wxString::Format(wxT("Enter key %s failed"), key));
			return;
		}

		if (modifiers & wxMOD_ALT)
			this->AddInputKey(input, VK_MENU);
		if (modifiers & wxMOD_CONTROL)
			this->AddInputKey(input, VK_CONTROL);
		if (modifiers & wxMOD_SHIFT)
			this->AddInputKey(input, VK_SHIFT);
		if (modifiers & wxMOD_META)
			this->AddInputKey(input, VK_LWIN);

		this->AddInputKey(input, keyCode);
		this->AddInputKey(input, keyCode, false);

		if (modifiers & wxMOD_ALT)
			this->AddInputKey(input, VK_MENU, false);
		if (modifiers & wxMOD_CONTROL)
			this->AddInputKey(input, VK_CONTROL, false);
		if (modifiers & wxMOD_SHIFT)
			this->AddInputKey(input, VK_SHIFT, false);
		if (modifiers & wxMOD_META)
			this->AddInputKey(input, VK_LWIN, false);
	}

	for (int i=0; i<WIN_MOD_NUM; ++i)
	{
		if (::GetAsyncKeyState(WIN_MOD[i]) & 0x8000)
			this->AddInputKey(input, WIN_MOD[i]);
	}

	::BlockInput(TRUE);
	::SendInput(input.size(), &input[0], sizeof(INPUT));
	::BlockInput(FALSE);
}
/*
wxString GetFullCmdName(const wxString& commandName,const wxString& workingDir,const bool retOld = false)
{
	wxString wdir = workingDir;
	wxString cmdName = commandName;
	wxFileName fn = wxFileName(cmdName);
	if (wdir.empty())
		wdir = ::wxGetCwd();
	//如果文件存在或包含路径信息就不再进行其它额外的判断
	if (fn.Exists() == false)
	{
		bool wFound = false;
		wxString cwd = ::wxGetCwd();
		wxArrayString mcwd;//根据PATH路径查询文件
		wxString NameTmp;
		wxString wExt = ::wxGetenv("PATHEXT");
		wExt.UpperCase();
		if (!fn.HasExt())
			cmdName.Append(".*");
		if (cmdName.GetChar(0) == '\\')
			mcwd.Add("\\");
		else if (cmdName.find('\\') == wxNOT_FOUND)//在PATH和当前目录中查询
			mcwd = ::wxSplit(::wxGetCwd().Append(';').Append(::wxGetenv("PATH")),';','\0');
		else//只在当前目录中查询
			mcwd.Add(::wxGetCwd());

		for(size_t i = 0;i< mcwd.size();++i)
		{
			if (!::wxDirExists(mcwd[i]) || !::wxSetWorkingDirectory(mcwd[i]))//路径错误或不存在不查找
				continue;
			NameTmp =::wxFindFirstFile(cmdName);
			if (NameTmp.empty())
				continue;
			do
			{
				if (wExt.find(wxFileName(NameTmp).GetExt().Upper()) != wxNOT_FOUND)//文件扩展名必须在PATHEXT变量里面
				{
					wFound = true;
					wdir = ::wxGetCwd();
					cmdName = NameTmp;
					break;
				}
				NameTmp = ::wxFindNextFile();
			} while (!NameTmp.empty());
			if (wFound)
				break;

		}
		::wxSetWorkingDirectory(cwd);
	}
	if (wxFileName(cmdName).IsAbsolute() == false)//如果不是绝对路径,转换成绝对路径
	{
		if (cmdName.GetChar(0) != '\\')
		{
			cmdName.insert(0,"\\");
			cmdName.insert(0,wdir);
		}
	}
	if (wxFileName(cmdName).Exists() == false && retOld == false)
		cmdName.Clear();
	return cmdName;
}
*/
/*
bool MerryController::ShellExecute(const wxString& commandName,
	const wxString& commandArg,
	const wxString& workingDir,
	const wxString& show) const
{
	// wxLogNull logNo;


	int showCommand = SW_SHOW;
	bool runas = false;
	bool winexec = false;
	bool NeedGetPath = true;

	if (show.IsSameAs(wxT("hide"), false))
		showCommand = SW_HIDE;
	else if (show.IsSameAs(wxT("min"), false))
		showCommand = SW_SHOWMINIMIZED;
	else if (show.IsSameAs(wxT("max"), false))
		showCommand = SW_SHOWMAXIMIZED;
	size_t len = commandName.size();
	size_t n;
	for(n = 0;n<len;++n)
	{
		wxChar c = commandName[n];
		switch(c)
		{
			case '@'://前导'@'隐藏窗口运行
				showCommand = SW_HIDE;
				continue;
			case '*'://以管理员权限运行
				runas = true;
				continue;
			case '>'://根据系统判断,NT6以上以管理员权限运行.
				if (::wxGetWinVersion() >= wxWinVersion_6)
					runas = true;
				continue;
			case '+'://强制参数标志
				continue;
			case '\2'://强制使用WinExec 函数执行
				winexec = true;
			case '\1'://\1\2 标志说明该命令已经经过处理,不需要再次处理
				NeedGetPath = false;
				continue;
			case ' ':
			case '\t':
				continue;
			default:
				break;
		}
		break;
	}

	wxString cmdName = commandName.substr(n);
	wxString FullcmdName = NeedGetPath?GetCMDPath(cmdName):cmdName;

	if (!LocationExec)
	{
		if (winexec || FullcmdName.empty())//没有获取到文件路径直接调用WINEXEC执行
		{
			__DEBUG_BEGIN(wxString::Format("cmd.exe /c start \"\" /D \"%s\" \"%s\" %s",workingDir,cmdName,commandArg));
			return (int)::WinExec(wxString::Format("cmd.exe /c start \"\" /D \"%s\" \"%s\" %s",workingDir,cmdName,commandArg),SW_HIDE) > 32;
		}
		//有获取到文件路径，直接启动。
		__DEBUG_BEGIN(FullcmdName.c_str());
		return (int)::ShellExecute(NULL,(runas?_T("RunAs"):NULL), FullcmdName.c_str(), commandArg.c_str(), workingDir.c_str(), showCommand) > 32;

		//wxString tmpName = GetFullCmdName(cmdName,workingDir,false);
		//if (tmpName.empty())
		//{
		//	::wxMessageBox(wxString::Format(wxT("无法运行 '%s'。请确定文件名是否正确后，再试一次。"),cmdName),cmdName,wxOK | wxICON_ERROR);
		//	return false;
		//}
		//return (int)::ShellExecute(NULL,(runas?_T("RunAs"):NULL), tmpName.c_str(), commandArg.c_str(), workingDir.c_str(), showCommand)>32;
	}

	LocationExec = false;//定位文件位置标志复位
	if (FullcmdName.empty())
		return false;

	FullcmdName.Replace('/','\\');

#ifdef _ALMRUN_CONFIG_H_
	if (!g_config->Explorer.empty())
		return (int)::WinExec(wxString::Format("%s \"%s\"",g_config->Explorer,FullcmdName),SW_SHOW) > 32;
#endif//ifdef _ALMRUN_CONFIG_H_
	return (int)::ShellExecute(NULL,NULL,_T("explorer.exe"),_T("/n,/select,")+FullcmdName.c_str(),NULL, SW_SHOW) > 32;
	
	//return (int)::ShellExecute(NULL,NULL,g_config->Explorer.c_str(),wxString::Format(wxT("\"%s\""),cmdName),NULL, SW_SHOW) > 32;
}
*/

DWORD MerryController::ShellExecute(const wxString& commandName,
	const wxString& commandArg,
	const wxString& workingDir,
	const wxString& show) const
{
	// wxLogNull logNo;


	int showCommand = SW_SHOW;
	bool runas = false;
	bool winexec = false;
	bool NeedGetPath = true;
	bool SavePid = false;

	if (g_config && g_config->get(cmdSingleProecss))
		SavePid = true;

	if (show.IsSameAs(wxT("hide"), false))
		showCommand = SW_HIDE;
	else if (show.IsSameAs(wxT("min"), false))
		showCommand = SW_SHOWMINIMIZED;
	else if (show.IsSameAs(wxT("max"), false))
		showCommand = SW_SHOWMAXIMIZED;
	size_t len = commandName.size();
	size_t n;
	for(n = 0;n<len;++n)
	{
		wxChar c = commandName[n];
		switch(c)
		{
			case '?'://前导'?'唯一进程
				SavePid = true;
				continue;
			case '@'://前导'@'隐藏窗口运行
				showCommand = SW_HIDE;
				continue;
			case '*'://以管理员权限运行
				runas = true;
				continue;
			case '>'://根据系统判断,NT6以上以管理员权限运行.
				if (::wxGetWinVersion() >= wxWinVersion_6)
					runas = true;
				continue;
			case '+'://强制参数标志
				continue;
			case '\2'://强制使用WinExec 函数执行
				winexec = true;
			case '\1'://\1\2 标志说明该命令已经经过处理,不需要再次处理
				NeedGetPath = false;
				continue;
			case '|'://命令标志结尾.
				++n;
				break;
			case ' ':
			case '\t':
				continue;
			default:
				break;
		}
		break;
	}

	wxString cmdName = commandName.substr(n);
	if (cmdrun_flags & CMDRUN_FLAG_RUNAS) runas = true;
	if (cmdrun_flags & CMDRUN_FLAG_HIDE) showCommand = SW_HIDE;
	cmdrun_flags = 0;

	if (!LocationExec)
	{
		__DEBUG_BEGIN(cmdName.c_str());
		SHELLEXECUTEINFO ShExecInfo = {0};
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = runas?_T("RunAs"):NULL;
		ShExecInfo.lpFile = cmdName.c_str();    //要运行的文件
		ShExecInfo.lpParameters =commandArg.c_str();
		ShExecInfo.lpDirectory = workingDir.c_str();
		ShExecInfo.nShow = showCommand;
		ShExecInfo.hInstApp = NULL;
		if (::ShellExecuteEx(&ShExecInfo))
			return SavePid?GetProcessId(ShExecInfo.hProcess):1;
		return 0;
	}

	LocationExec = false;//定位文件位置标志复位
	wxString FullcmdName = NeedGetPath?GetCMDPath(cmdName):cmdName;
	if (FullcmdName.empty())
		return 0;

	FullcmdName.Replace('/','\\');

#ifdef _ALMRUN_CONFIG_H_
	if (!g_config->Explorer.empty())
		::WinExec(wxString::Format("%s \"%s\"",g_config->Explorer,FullcmdName),SW_SHOW);
	else
#endif//ifdef _ALMRUN_CONFIG_H_
		::ShellExecute(NULL,NULL,_T("explorer.exe"),_T("/n,/select,")+FullcmdName.c_str(),NULL, SW_SHOW);
	return 0;
}

typedef    int        (__stdcall  *  Wow)(LPVOID); 
bool wowcall(wxString func,LPVOID old)
{
	HINSTANCE  hlibrary; 
	typedef    bool        (__stdcall  *  Wow)(LPVOID); 
	bool ret = false;
	hlibrary  =  LoadLibrary(L"Kernel32.dll"); 
	Wow f_wow =(Wow)GetProcAddress(hlibrary,func.c_str());
	if (f_wow)
	{
		ret = f_wow(old);
	}
	FreeLibrary(hlibrary);
	return ret;

}
bool Wow64Disable(PVOID *old)
{
	return wowcall(_T("Wow64DisableWow64FsRedirection"),old);
}

bool Wow64Revert(PVOID old)
{
	return wowcall(_T("Wow64RevertWow64FsRedirection"),old);
}

#endif

#include "MerryController.h"

#ifdef __WXMSW__

#include "MerryInformationDialog.h"
#include "MerryHelper.h"
#include <vector>
#include <Windows.h>
wxString Explorer;
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
		tmpInput.type = INPUT_KEYBOARD;
		tmpInput.ki.dwFlags = isDown ? 0 : KEYEVENTF_KEYUP;
		tmpInput.ki.wVk = keyCode;
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

void* MerryController::FindWindow(const wxString& name, void* parentWindow) const
{
	return ::FindWindowEx((HWND)parentWindow, NULL, NULL, name.c_str());
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
		else if (modifiers & wxMOD_CONTROL)
			this->AddInputKey(input, VK_CONTROL);
		else if (modifiers & wxMOD_SHIFT)
			this->AddInputKey(input, VK_SHIFT);
		else if (modifiers & wxMOD_META)
			this->AddInputKey(input, VK_LWIN);

		this->AddInputKey(input, keyCode);
		this->AddInputKey(input, keyCode, false);

		if (modifiers & wxMOD_ALT)
			this->AddInputKey(input, VK_MENU, false);
		else if (modifiers & wxMOD_CONTROL)
			this->AddInputKey(input, VK_CONTROL, false);
		else if (modifiers & wxMOD_SHIFT)
			this->AddInputKey(input, VK_SHIFT, false);
		else if (modifiers & wxMOD_META)
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

bool MerryController::ShellExecute(const wxString& commandName,
	const wxString& commandArg,
	const wxString& workingDir,
	const wxString& show) const
{
	// wxLogNull logNo;
	wxString cmdName = commandName;
	int showCommand = SW_SHOW;
	bool runas = false;
	if (show.IsSameAs(wxT("hide"), false))
		showCommand = SW_HIDE;
	else if (show.IsSameAs(wxT("min"), false))
		showCommand = SW_SHOWMINIMIZED;
	else if (show.IsSameAs(wxT("max"), false))
		showCommand = SW_SHOWMAXIMIZED;

	while(true)
	{
		UCHAR T = cmdName.GetChar(0).GetValue();
		if (T == '@')//前导'@'隐藏窗口运行
			showCommand = SW_HIDE;
		else if (T == '*')//前导'*' NT6以上请会管理员权限,NT5运行为
			runas = true;
		else if (T == '>')//前导'>'请求管理员权限(NT6以上有效)
		{
			if (::wxGetWinVersion() >= wxWinVersion_6)
				runas = true;
		}
		else
			break;
		cmdName.erase(0,1);
	}
	if (!LocationExec)
		return (int)::ShellExecute(NULL,(runas?wxT("RunAs"):NULL), cmdName.c_str(), commandArg.c_str(), workingDir.c_str(), showCommand) > 32;

	LocationExec = false;//定位文件位置标志复位
	wxString wdir = workingDir;
	if (wdir.empty())
		wdir = ::wxGetCwd();
	//如果文件存在或包含路径信息就不再进行其它额外的判断
	if (!::wxFileExists(cmdName) && !::wxDirExists(cmdName) && cmdName.find('/') == wxNOT_FOUND)
	{
		bool wFound = false;
		wxString cwd = ::wxGetCwd();
		wxArrayString mcwd;//根据PATH路径查询文件
		wxString Ext,NameTmp,sxPath;
		wxString wExt = ::wxGetenv("PATHEXT");
		wExt.UpperCase();

		::wxSplitPath(cmdName,&sxPath,NULL,&Ext);
		if (Ext.empty())
			cmdName.Append(".*");
		if (cmdName.find('\\') == wxNOT_FOUND)//在PATH和当前目录中查询
			mcwd = ::wxSplit(::wxGetCwd().Append(';').Append(::wxGetenv("PATH")),';','\0');
		else if (cmdName.GetChar(1) == ':')//有带盘符信息,只在对应目录下查找
			mcwd.Add(sxPath);
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
				::wxSplitPath(NameTmp,NULL,NULL,&Ext);
				if (wExt.find(Ext.Upper()) != wxNOT_FOUND)//文件扩展名必须在PATHEXT变量里面
				{
					wFound = true;
					wdir = mcwd[i];
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
	if (Explorer.empty())
		return (int)::ShellExecute(NULL,NULL,_T("explorer.exe"),_T("/n,/select,")+cmdName.c_str(),wdir.c_str(), SW_SHOW) > 32;
	if (::wxIsAbsolutePath(cmdName) == false)
		cmdName.insert(0,wdir);
	return (int)::ShellExecute(NULL,NULL,Explorer.c_str(),wxString::Format(wxT("\"%s\""),cmdName),wdir.c_str(), SW_SHOW) > 32;
}

#endif

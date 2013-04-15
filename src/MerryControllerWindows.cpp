#include "MerryController.h"

#ifdef __WXMSW__

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

wxString MerryController::GetClipboardData(void) const
{
	return "Test";
}

void MerryController::SetClipboardData(const wxString& text) const
{
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

	int showCommand = SW_SHOW;
	if (show.IsSameAs(wxT("hide"), false))
		showCommand = SW_HIDE;
	else if (show.IsSameAs(wxT("min"), false))
		showCommand = SW_SHOWMINIMIZED;
	else if (show.IsSameAs(wxT("max"), false))
		showCommand = SW_SHOWMAXIMIZED;

	return (int)::ShellExecute(NULL, NULL, commandName.c_str(), commandArg.c_str(), workingDir.c_str(), showCommand) > 32;
}

#endif

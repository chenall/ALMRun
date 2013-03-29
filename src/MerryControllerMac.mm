#include "MerryController.h"

#ifdef __WXOSX__

#import <cocoa/cocoa.h>

MerryController::MerryController()
{
	
}

MerryController::~MerryController()
{
	
}

void* MerryController::GetForegroundWindow() const
{
	return NULL;
}

void MerryController::SetForegroundWindow(void* window) const
{

}

void MerryController::ShowWindow(void* window, const wxString& show) const
{

}

void MerryController::CloseWindow(void* window) const
{

}

bool MerryController::IsWindowMax(void* window) const
{
	return false;
}

bool MerryController::IsWindowMin(void* window) const
{
	return false;
}

bool MerryController::IsWindowShown(void* window) const
{
    return false;
}

wxString MerryController::GetWindowText(void* window) const
{
	return "";
}

void MerryController::SetWindowText(void* window, const wxString& text) const
{

}

void MerryController::GetWindowSize(void* window, int& width, int& height) const
{

}

void MerryController::SetWindowSize(void* window, int width, int height) const
{

}

void MerryController::GetWindowPosition(void* window, int& x, int& y) const
{

}

void MerryController::SetWindowPosition(void* window, int x, int y) const
{

}

void* MerryController::FindWindow(const wxString& name, void* parentWindow) const
{
	return NULL;
}

void MerryController::EnterKey(const wxArrayString& keys)
{
	
}

bool MerryController::ShellExecute(const wxString& commandName,
					   const wxString& commandArg,
					   const wxString& workingDir,
					   const wxString& show) const
{
	wxString cwd = ::wxGetCwd();
	if (!workingDir.IsEmpty())
		::wxSetWorkingDirectory(workingDir);

	wxString shell = wxT("open \"") + commandName + wxT("\"");
	if (!commandArg.IsEmpty())
		shell += wxT(" --args ") + commandArg;
	bool isOk = ::wxShell(shell);

	::wxSetWorkingDirectory(cwd);

	return isOk;
}

#endif

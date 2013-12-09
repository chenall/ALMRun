#ifndef _MERRY_CONTROLLER_H_
#define _MERRY_CONTROLLER_H_

#include "MerryWx.h"
#include <wx/uiaction.h>
#include <vector>
#ifdef __WXGTK__
#include <X11/Xlib.h>
#endif

class MerryController
{
public:	
	MerryController();
	~MerryController();

	void* GetForegroundWindow() const;
	void SetForegroundWindow(void* window) const;
	// show = normal, hide, min, max, restore
	void ShowWindow(void* window, const wxString& show) const;
	void CloseWindow(void* window) const;
	bool IsWindowMax(void* window) const;
	bool IsWindowMin(void* window) const;
	bool IsWindowShown(void* window) const;
	wxString GetWindowText(void* window) const;
	void SetWindowText(void* window, const wxString& text) const;
	void GetWindowSize(void* window, int& width, int& height) const;
	void SetWindowSize(void* window, int width, int height) const;
	void GetWindowPosition(void* window, int& x, int& y) const;
	void SetWindowPosition(void* window, int x, int y) const;
	void* FindWindow(const wxString& ClassName,const wxString& WindowName) const;
	void* FindWindowEx(void* Parent,void *Child,const wxString& ClassName,const wxString&  WindowName) const;
	BOOL SetWindowPos(void *hWnd,void *hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags) const;
	wxString GetClipboardData(void) const;
	void SetClipboardData(const wxString& text) const;
	void GetMousePosition(int& x, int& y) const;
	void SetMousePosition(int x, int y);

	void EnterKey(const wxArrayString& keys);
	void EnterText(const wxString& text);

#ifdef __WXMSW__
	DWORD ShellExecute(const wxString& commandName,
		const wxString& commandArg = wxEmptyString,
		const wxString& workingDir = wxEmptyString,
		const wxString& show = wxEmptyString) const;
#else
	// show = normal, hide, min, max
	bool ShellExecute(const wxString& commandName,
		const wxString& commandArg = wxEmptyString,
		const wxString& workingDir = wxEmptyString,
		const wxString& show = wxEmptyString) const;
#endif
private:
	wxUIActionSimulator m_simulator;

private:
#ifdef __WXMSW__
	void AddInputKey(std::vector<INPUT>& input, int keyCode, bool isDown = true) const;
#endif

#ifdef __WXGTK__
	bool IsFeatureSupported(const char* feature) const;
	unsigned char* GetWindowPropertyByAtom(Window window, Atom atom,
		long* nitems, Atom* type, int* size) const;
	long GetDesktopForWindow(Window window) const;
	bool SetCurrentDesktop(long desktop) const;
#endif

private:
#ifdef __WXGTK__
	Display* m_display;
#endif
};

#ifdef __WXMSW__
	bool Wow64Revert(PVOID old);
	bool Wow64Disable(PVOID *old);
#endif
extern MerryController* g_controller;

#endif

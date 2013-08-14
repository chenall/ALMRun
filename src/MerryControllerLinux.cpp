// xdotool

#include "MerryController.h"

#ifdef __WXGTK__

MerryController::MerryController()
{
	m_display = XOpenDisplay(NULL);
}

MerryController::~MerryController()
{
	XCloseDisplay(m_display);
}

bool MerryController::IsFeatureSupported(const char* feature) const
{
	Atom request = XInternAtom(m_display, "_NET_SUPPORTED", False);
	Atom featureAtom = XInternAtom(m_display, feature, False);
	Window root = XDefaultRootWindow(m_display);

	Atom type = 0;
	long nitems = 0L;
	int size = 0;
	Atom* results = (Atom*)this->GetWindowPropertyByAtom(root, request, &nitems, &type, &size);
	for (int i=0; i<nitems; ++i)
	{
		if (results[i] == featureAtom)
			return true;
	}
	free(results);

	return false;
}

unsigned char* MerryController::GetWindowPropertyByAtom(Window window, Atom atom,
	long* nitems, Atom* type, int* size) const
{
	Atom actualType;
	int actualFormat;
	unsigned long _nitems;
	unsigned long bytesAfter; /* unused */
	unsigned char *prop; /* unsigned long nbytes */
	int status = XGetWindowProperty(m_display, window, atom, 0, (~0L),
		False, AnyPropertyType, &actualType,
		&actualFormat, &_nitems, &bytesAfter,
		&prop);
	if (status == BadWindow)
		return NULL;
	if (status != Success)
		return NULL;

	if (nitems != NULL)
		*nitems = _nitems;
	if (type != NULL)
		*type = actualType;
	if (size != NULL)
		*size = actualFormat;

	return prop;
}

long MerryController::GetDesktopForWindow(Window window) const
{
	assert(this->IsFeatureSupported("_NET_WM_DESKTOP"));
	if (!this->IsFeatureSupported("_NET_WM_DESKTOP"))
		return -1;

	Atom request = XInternAtom(m_display, "_NET_WM_DESKTOP", False);
	Atom type;
	int size;
	long nitems;
	unsigned char* data = this->GetWindowPropertyByAtom(window, request, &nitems, &type, &size);

	long desktop = -1;
	if (nitems > 0)
		desktop = *((long*)data);
	free(data);

	return desktop;
}

bool MerryController::SetCurrentDesktop(long desktop) const
{
	assert(this->IsFeatureSupported("_NET_CURRENT_DESKTOP"));
	if (!this->IsFeatureSupported("_NET_CURRENT_DESKTOP"))
		return false;

	Window root = RootWindow(m_display, 0);
	XEvent xev;
	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.display = m_display;
	xev.xclient.window = root;
	xev.xclient.message_type = XInternAtom(m_display, "_NET_CURRENT_DESKTOP", False);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = desktop;
	xev.xclient.data.l[1] = CurrentTime;

	return XSendEvent(m_display, root, False, 
			SubstructureNotifyMask | SubstructureRedirectMask,
			&xev) == 0;
}

void* MerryController::GetForegroundWindow() const
{
	assert(this->IsFeatureSupported("_NET_ACTIVE_WINDOW"));
	if (!this->IsFeatureSupported("_NET_ACTIVE_WINDOW"))
		return 0;

	Atom request = XInternAtom(m_display, "_NET_ACTIVE_WINDOW", False);
	Window root = XDefaultRootWindow(m_display);
	Atom type;
	int size;
	long nitems;
	unsigned char* data = this->GetWindowPropertyByAtom(root, request, &nitems, &type, &size);

	Window foregroundWindow = 0;
	if (nitems > 0)
		foregroundWindow = *((Window*)data);
	free(data);

	return (void*)foregroundWindow;
}

void MerryController::SetForegroundWindow(void* window) const
{

}

void MerryController::ShowWindow(void* _window, const wxString& show) const
{
	Window window = (Window)_window;

	assert(this->IsFeatureSupported("_NET_ACTIVE_WINDOW"));
	if (!this->IsFeatureSupported("_NET_ACTIVE_WINDOW"))
		return;

	if (this->IsFeatureSupported("_NET_WM_DESKTOP") && this->IsFeatureSupported("_NET_CURRENT_DESKTOP"))
	{
		long desktop = this->GetDesktopForWindow(window);
		this->SetCurrentDesktop(desktop);
	}

	XEvent xev;
	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.display = m_display;
	xev.xclient.window = window;
	xev.xclient.message_type = XInternAtom(m_display, "_NET_ACTIVE_WINDOW", False);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 2L; /* 2 == Message from a window pager */
	xev.xclient.data.l[1] = CurrentTime;

	XWindowAttributes wattr;
	XGetWindowAttributes(m_display, window, &wattr);
	XSendEvent(m_display, wattr.screen->root, False,
		SubstructureNotifyMask | SubstructureRedirectMask,
		&xev);
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
	return wxT("");
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

void* MerryController::FindWindow(const wxString& ClassName,const wxString& WindowName) const
{
	return NULL;
}

void* MerryController::FindWindowEx(void* Parent,void *Child,const wxString& ClassName,const wxString&  WindowName) const
{
	return NULL;
}

BOOL MerryController::SetWindowPos(void *hWnd,void *hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags) const
{
	return False;
}

void MerryController::EnterKey(const wxArrayString& keys)
{
	
}

bool MerryController::ShellExecute(const wxString& commandName,
	const wxString& commandArg,
	const wxString& workingDir,
	const wxString& show) const
{
	return false;
}

#endif

#include "ALMRunConfig.h"
#include "MerryApp.h"
ALMRunConfig* g_config = NULL;
ALMRunConfig::ALMRunConfig(void)
{
	CompareMode = 0;
	NumberKey = false;
	ShowTrayIcon = true;
	ShowTopTen = true;
	ExecuteIfOnlyOne = false;
}

bool ALMRunConfig::set(const wxString& name,const wxString &value)
{
	if (name.CmpNoCase("Explorer") == 0)
		Explorer = value;
	else if (name.CmpNoCase("Root") == 0)
	{
		Root = value;
		::wxSetWorkingDirectory(Root);
	}
	else
		return false;
	return ::wxSetEnv(wxT("ALMRUN_")+name.Upper(),value);
}

bool ALMRunConfig::set(const wxString& name,const int value)
{
	if (name.CmpNoCase("CompareMode") == 0)
		CompareMode = value;
	else if (name.CmpNoCase("NumberKey") == 0)
		NumberKey = value != 0;
	else if (name.CmpNoCase("ShowTrayIcon") == 0)
	{
		ShowTrayIcon = value != 0;
		::wxGetApp().GetFrame().ShowTrayIcon(ShowTrayIcon);
	}
	else if (name.CmpNoCase("ShowTopTen") == 0)
		ShowTopTen = value != 0;
	else if (name.CmpNoCase("ExecuteIfOnlyOne") == 0)
		ExecuteIfOnlyOne = value != 0;
	else
		return false;

	return ::wxSetEnv(wxT("ALMRUN_")+name.Upper(),wxString::Format("%d",value));
}

void ALMRunConfig::get(const wxString& name)
{

}

ALMRunConfig::~ALMRunConfig(void)
{
}

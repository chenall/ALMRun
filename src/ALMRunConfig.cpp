#include "ALMRunConfig.h"
#include "MerryApp.h"
ALMRunConfig* g_config = NULL;
ALMRunConfig::ALMRunConfig(void)
{
	CompareMode = 0;
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
	else
		return false;

	return ::wxSetEnv(wxT("ALMRUN_")+name.Upper(),wxString::Format("%d",value));
}

void ALMRunConfig::get(const wxString& name)
{
	if (name.CmpNoCase("CompareMode") == 0)
		return (void)CompareMode;
}

ALMRunConfig::~ALMRunConfig(void)
{
}

#include "MerryCommandManager.h"
#include "MerryHotkey.h"
#include "ALMRunConfig.h"
#include "MerryApp.h"
ALMRunConfig* g_config = NULL;
ALMRunConfig::ALMRunConfig(void)
{
	if (::wxGetEnv("ALMRUN_HOME",&Home))
		cfg_file = Home + wxGetApp().GetAppName().Append(".ini");
	if (wxFileExists(cfg_file) == false)
	{
		CompareMode = 0;
		NumberKey = false;
		ShowTrayIcon = true;
		ShowTopTen = true;
		ExecuteIfOnlyOne = false;
		conf = NULL;
		cfg_time = 0;
		return;
	}

	conf = new wxFileConfig("ALMRun",wxEmptyString,cfg_file,wxEmptyString,wxCONFIG_USE_LOCAL_FILE);
	conf->SetPath("/Config");
	conf->SetRecordDefaults(false);
	CompareMode = conf->ReadLong("CompareMode",0);
	NumberKey = conf->ReadBool("NumberKey",false);
	ShowTopTen = conf->ReadBool("ShowTopTen",true);
	ExecuteIfOnlyOne = conf->ReadBool("ExecuteIfOnlyOne",false);
	this->set("Explorer",conf->Read("Explorer"));
	this->set("Root",conf->Read("Root"));
	this->set("HotKey",conf->Read("HotKey"));
	this->set("HotKeyReLoad",conf->Read("HotKeyReLoad"));
	this->set("ShowTrayIcon",conf->ReadBool("ShowTrayIcon",true));
	this->ConfigCommand();
	conf->SetPath("/Config");
	cfg_time = wxFileModificationTime(cfg_file);
}

bool ALMRunConfig::set(const wxString& name,const wxString &value)
{
	if (name.Cmp("Explorer") == 0)
		Explorer = value;
	else if (name.Cmp("Root") == 0)
	{
		Root = value;
		if (!Root.empty())
			::wxSetWorkingDirectory(Root);
	}
	else if(name.Cmp("HotKey") == 0)
		HotKey = value;
	else if(name.Cmp("HotKeyReLoad") == 0)
		HotKeyReLoad = value;
	else
		return false;
	return ::wxSetEnv(wxT("ALMRUN_")+name.Upper(),value);
}

bool ALMRunConfig::set(const wxString& name,const int value)
{
	if (name.Cmp("CompareMode") == 0)
		CompareMode = value;
	else if (name.Cmp("NumberKey") == 0)
		NumberKey = value != 0;
	else if (name.Cmp("ShowTrayIcon") == 0)
	{
		ShowTrayIcon = value != 0;
		::wxGetApp().GetFrame().ShowTrayIcon(ShowTrayIcon);
	}
	else if (name.Cmp("ShowTopTen") == 0)
		ShowTopTen = value != 0;
	else if (name.Cmp("ExecuteIfOnlyOne") == 0)
		ExecuteIfOnlyOne = value != 0;
	else
		return false;
	return true;
}

void ALMRunConfig::WriteConfig(const wxString& name,const wxString& value)
{
	if (conf)
	{
		conf->Write(name,value);
		conf->Flush();
	}
}

bool ALMRunConfig::Changed()
{
	return (cfg_time  && cfg_time != wxFileModificationTime(cfg_file));
}

void ALMRunConfig::ConfigCommand()
{
	conf->SetPath("/Command");
	size_t sizes;
	wxString name;
	wxString key;
	wxString cmd;
	wxString desc;
	wxString cmds;
	int cmdId;
	sizes = conf->ReadLong("cmds.size",0);
	while(sizes)
	{
		cmds = wxString::Format("cmds.%d.",sizes);
		--sizes;
		cmd = conf->Read(cmds + "cmd");
		if (cmd.empty() && key.empty())
			continue;
		name = conf->Read(cmds + "name");
		key = conf->Read(cmds + "key");
		desc = conf->Read(cmds + "desc");
		cmdId = g_commands->AddCommand(name,desc,cmd,0,key,0);
		if (cmdId < 0 || key.empty())
			continue;
		g_hotkey->RegisterHotkey(cmdId);
	}
	conf->SetPath("/directories");
	sizes = conf->ReadLong("cmds.size",0);
	while(sizes)
	{
		cmds = wxString::Format("cmds.%d.",sizes);
		--sizes;
		name = conf->Read(cmds + "dir");
		if (name.empty())
			continue;
	}
}

void ALMRunConfig::get(const wxString& name)
{

}

ALMRunConfig::~ALMRunConfig(void)
{
	if (conf)
	{
		conf->Flush();
		delete conf;
	}
}

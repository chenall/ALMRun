#pragma once
#ifndef _ALMRUN_CONFIG_H_
#define _ALMRUN_CONFIG_H_
#include "ALMRunVersion.h"
#include "MerryWx.h"
#include <wx/fileconf.h>

#define TASKBARICON_TIP "ALMRun v"##VERSION_STR##"\n±ã½ÝÆô¶¯¹¤¾ß"


class ALMRunConfig
{
public:
	int CompareMode;
	bool NumberKey;
	bool ShowTrayIcon;
	bool ShowTopTen;
	bool ExecuteIfOnlyOne;
	wxString Explorer;
	wxString Root;
	wxString Home;
	wxString HotKey;
	wxString HotKeyReLoad;
	void Check();
	bool Changed();
	void get(const wxString& name);
	bool set(const wxString& name,const wxString& value);
	bool set(const wxString& name,const int value);
	void WriteConfig(const wxString& name,const wxString& value);
	void ConfigCommand();
	ALMRunConfig(void);
	virtual ~ALMRunConfig(void);
private:
	wxString cfg_file;
	time_t cfg_time;
	wxFileConfig *conf;
};

extern ALMRunConfig* g_config;

enum
{
	MENU_ITEM_OPEN = 10001,
	MENU_ITEM_OPEN_CONFIG,
	MENU_ITEM_CONFIG,
	MENU_ITEM_ABOUT,
	MENU_ITEM_EXIT,
};

#endif
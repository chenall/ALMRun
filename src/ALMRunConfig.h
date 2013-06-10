#pragma once
#ifndef _ALMRUN_CONFIG_H_
#define _ALMRUN_CONFIG_H_
#include "ALMRunVersion.h"
#include "MerryWx.h"
#include <wx/fileconf.h>
#define CMDS_FLAG_ALMRUN_CMDS 1
#define CMDS_FLAG_ALMRUN_DIRS 2
#define CMDS_FLAG_ALMRUN_LUA 4
#define TASKBARICON_TIP "ALMRun v"##VERSION_STR##"\n±ã½ÝÆô¶¯¹¤¾ß"


class ALMRunConfig
{
public:
	int CompareMode;
	bool NumberKey;
	bool ShowTrayIcon;
	bool ShowTopTen;
	bool ExecuteIfOnlyOne;
	bool IndexFrom0to9;
	bool OrderByPre;
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
	void ListFiles(const wxString& dirname,wxArrayString *files,const wxArrayString& filespec,const int sub = -1);
	void ListFiles(const wxString& dirname,wxArrayString *files,const wxString& filespec,const int sub = -1);
	void WriteConfig(const wxString& name,const wxString& value);
	bool SaveCfg();
	bool DeleteCmd(const int id);
	bool DeleteDir(const int id);
	int AddDir(const wxString& path,const wxString& inc = wxEmptyString,const wxString& exc = wxEmptyString ,const int sub = 0);
	bool AddCmd(const wxString& cmd,const wxString& name = wxEmptyString,const wxString& key=wxEmptyString,const wxString& desc = wxEmptyString,const int id = -1);
	void ConfigCommand();
	void OldToNew();
	ALMRunConfig(void);
	virtual ~ALMRunConfig(void);
private:
	int lastId;
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
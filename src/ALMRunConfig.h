#pragma once
#ifndef _ALMRUN_CONFIG_H_
#define _ALMRUN_CONFIG_H_
#include "ALMRunVersion.h"
#include "MerryWx.h"
#include <wx/fileconf.h>

#define TASKBARICON_TIP "ALMRun v"##VERSION_STR##"\n±ã½ÝÆô¶¯¹¤¾ß"
#define CONFIG_VERSION 1
#define ParamHistoryLimit_default 50
enum gui_config_item
{
	ListFont,
	GUI_CONFIG_MAX,
};

typedef enum
{
	AutoRun,
	StayOnTop,
	NumberKey,
	ShowTrayIcon,
	ShowTopTen,
	ExecuteIfOnlyOne,
	RememberFavouratMatch,
	IndexFrom0to9,
	OrderByPre,
	ShowTip,
	DisableWow64FsRedirection,
	AddToSendTo,
	PlayPopupNotify,
	SpaceKey,
	AutoPopup,
	DoubleToggleFunc,
	DoubleClick,
	DuplicateCMD,
	CONFIG_ITEM_MAX,
} config_item_t;

struct DirCfg
{
	wxString include;
	wxString exclude;
	int sub;
};

class ALMRunConfig
{
public:
	~ALMRunConfig();
	ALMRunConfig();
	int CompareMode;

	wxString gui_config[GUI_CONFIG_MAX];
	static const char *config_str[];
	static const char *config_tip[];
	wxString Explorer;
	wxString Root;
	wxString Home;
	wxString HotKey;
	wxString HotKeyReLoad;
	void Check();
	bool Changed();
	void get(const wxString& name);
	bool get(config_item_t config_type);
	bool set(const wxString& name,const wxString& value);
	bool set(const wxString& name,const int value);
	void ListFiles(const wxString& dirname,wxArrayString *files,const wxArrayString& filespec,const int sub = -1);
	void ListFiles(const wxString& dirname,wxArrayString *files,const wxString& filespec,const wxString& exclude  = wxEmptyString,const int sub = -1);
	bool SaveCfg();
	bool DeleteCmd(const int id);
	bool DeleteDir(const int id);
	int AddDir(const wxString& path,const wxString& inc = wxEmptyString,const wxString& exc = wxEmptyString ,const int sub = 0,const UINT id = -1);
	int AddCmd(const wxString& cmd,const wxString& name = wxEmptyString,const wxString& key=wxEmptyString,const wxString& desc = wxEmptyString,const int id = -1);
	bool ModifyCmd(const int id,const wxString& cmd = wxEmptyString,const wxString& name = wxEmptyString,const wxString& key=wxEmptyString,const wxString& desc = wxEmptyString);
	wxString& GetDefaultDirInclude() { return def_dir_cfg.include;};
	wxString& GetDefaultDirExclude() { return def_dir_cfg.exclude;};
	int GetDefaultDirSub() { return def_dir_cfg.sub;};

	void GuiConfig();
	int SetcmdOrder(wxString& cmd,int order = 0);
	wxFileConfig *conf;
	wxFileConfig *order_conf;

protected:
	bool config[CONFIG_ITEM_MAX];

private:
	void OldToNew();
	void ConfigCommand();
	wxString LastItemHotKey;
	//int lastId;
	bool cfg_changed;
	wxString cfg_file;
	DirCfg def_dir_cfg;
	time_t cfg_time;
	time_t order_cfg_time;
	wxString order_cfg_file;
	size_t ParamHistoryLimit;
	int config_ver;
};

#ifdef __WXMSW__
	extern HWND HWND_TEXTCTRL;
#endif
extern ALMRunConfig* g_config;
extern bool LocationExec;
enum
{
	MENU_ITEM_OPEN = 10001,
	MENU_ITEM_OPEN_CONFIG,
	MENU_ITEM_CONFIG,
	MENU_ITEM_GUI_CONFIG,
	MENU_ITEM_ABOUT,
	MENU_ITEM_EXIT,
	MENU_ITEM_CMDMGR,
	MENU_CMD_ADD,
	MENU_CMD_DEL,
	MENU_CMD_EDIT,
	MENU_CMD_OPENDIR,
	MENU_CMD_INFO,
};
#endif
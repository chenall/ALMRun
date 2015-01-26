#ifndef _ALMRUN_CONFIG_H_
#define _ALMRUN_CONFIG_H_
#include "ALMRunVersion.h"
#include "ALMRunCommon.h"
#include "MerryWx.h"
#include "SkinConfig.h"
#include <wx/fileconf.h>

#define TASKBARICON_TIP "ALMRun v"##VERSION_STR##"\n±ã½ÝÆô¶¯¹¤¾ß"
#define CONFIG_VERSION 2
#define FAVORITELIST_FILE wxT("config/FavoriteList.txt")
#define ParamHistoryLimit_default 50

typedef enum gui_config_item
{
	ListFont,
	CurrentSkin,
	GUI_CONFIG_MAX,
} gui_config_str_t;

typedef enum _config_item_t
{
	AutoRun,
	StayOnTop,
	NumberKey,
	ShowTrayIcon,
	ShowTopTen,
	ShowCommandLine,
	ExecuteIfOnlyOne,
	RememberFavouratMatch,
	MatchAnywhere,
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
	cmdSingleProecss,
	cmdReadShortcut,
	SaveWinPos,
	CONFIG_BOOL_ITEMS,
	CONFIG_ERROR_TYPE = CONFIG_BOOL_ITEMS,
	ShowCMDErrInfo = CONFIG_ERROR_TYPE,
	CONFIG_BOOL_MAX,
	CONFIG_VER = 0,
	ParamHistoryLimit,
	CONFIG_UINT_MAX,
} config_item_t;

typedef _config_item_t CONFIG_ITEM_T;
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

	size_t get_u(config_item_t config_type) const;
	bool set(const wxString& name,const wxString& value);
	bool set(const wxString& name,const int value);

	void get(const wxString& name);
	wxString get(gui_config_str_t item) const;
	bool get(config_item_t config_type);
	bool set(size_t item,bool value);
	bool SaveCfg();
	bool DeleteCmd(const int id);
	bool DeleteDir(const int id);
	int AddDir(const wxString& path,const wxString& inc = wxEmptyString,const wxString& exc = wxEmptyString ,const int sub = 0,const UINT id = -1);
	int AddCmd(const wxString& cmd,const wxString& name = wxEmptyString,const wxString& key=wxEmptyString,const wxString& desc = wxEmptyString,const wxString& path = wxEmptyString,const int id = -1);
	bool ModifyCmd(const int id,const wxString& cmd = wxEmptyString,const wxString& name = wxEmptyString,const wxString& key=wxEmptyString,const wxString& desc = wxEmptyString,const wxString& path = wxEmptyString);
	wxString& GetDefaultDirInclude() { return def_dir_cfg.include;};
	wxString& GetDefaultDirExclude() { return def_dir_cfg.exclude;};
	int GetDefaultDirSub() { return def_dir_cfg.sub;};

	void GuiConfig();
	bool SetFavorite(const wxString& key,const wxString& name);
	const wxString GetFavorite(const wxString& key);
	int SetcmdOrder(wxString& cmd,int order = 0);
	ALMRunCMDBase GetCmd(const int cmdid);
	wxFileConfig *conf;
	wxFileConfig *order_conf;

protected:
	bool confWrite(const wxString& key,const wxString& value);
	bool config[CONFIG_BOOL_MAX];
	size_t config_u[CONFIG_UINT_MAX];
	wxFileConfig *FavoriteList;

private:
	void configv2();
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
};

#ifdef __WXMSW__
	extern HWND HWND_TEXTCTRL;
#endif
extern ALMRunConfig* g_config;
extern SkinConfig *skin;
extern bool LocationExec;
extern int cmdrun_flags;

#define CMDRUN_FLAG_RUNAS 1
#define CMDRUN_FLAG_HIDE 2
#define CMDRUN_FLAG_FORCEARG 4

enum
{
	MENU_ITEM_FIRST = 10000,
	MENU_ITEM_SHOW,
	MENU_ITEM_OPEN_CONFIG,
	MENU_ITEM_RECONFIG,
	MENU_ITEM_GUI_CONFIG,
	MENU_ITEM_ABOUT,
	MENU_ITEM_EXIT,
	MENU_ITEM_EXIT1,
	MENU_ITEM_CMDMGR,
	MENU_ITEM_SKIN,
	MENU_ITEM_LAST,
	MENU_CMD_FIRST = 10000,
	MENU_CMD_ADD,
	MENU_CMD_DEL,
	MENU_CMD_EDIT,
	MENU_CMD_OPENDIR,
	MENU_CMD_INFO,
	MENU_CMD_RUNAS,
	MENU_CMD_RUNHIDE,
	MENU_CMD_LAST,
};
#endif
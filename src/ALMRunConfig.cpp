#include "MerryCommandManager.h"
#include "MerryError.h"
#include "MerryHotkey.h"
#include "ALMRunConfig.h"
#include "MerryListBoxPanel.h"
#include "MerryApp.h"
#include "DlgConfig.h"
#include <wx/stdpaths.h>
#include "ALMRunCommon.h"

ALMRunConfig* g_config = NULL;
const char *ALMRunConfig::config_str[] = {"AutoRun","StayOnTop","NumberKey","ShowTrayIcon","ShowTopTen","ExecuteIfOnlyOne","RememberFavouratMatch","IndexFrom0to9","OrderByPre","ShowTip","DisableWow64FsRedirection","AddToSendTo","PlayPopupNotify","SpaceKey","AutoPopup","DoubleToggleFunc","DoubleClick","DuplicateCMD"};
const char *ALMRunConfig::config_tip[] = {
	"如果选中，随系统启动而自动运行",
	"保持程序窗口置顶,默认禁用.",
	"选中时敲0-9键执行对应编号的快捷项",
	"选中时在系统托盘显示图标",
	"选中时仅显示前10项快捷项",
	"选中时列表只剩一项时无需按键立即执行",
	"如果选中，记住最近一次关键字和快捷项的对应关系",
	"如果未选中，编号顺序为 1, 2, ..., 9, 0",
	"如果选中, 命令列表中前辍匹配的排前面",
	"如果选中,鼠标移动列表框项目时会显示备注信息或命令行",
	"运行程序之前禁用系统的WOW64重定向,解决在64位系统上部份64位程序无法运行的问题",
	"如果选中，将本软件添加到“发送到”菜单",
	"如果选中，当窗体弹出时播放声音",
	"如果选中，按下空格键就启动当前条目",
	"如果选中，显示ALMRun界面时自动显示列表框",
	"如果选中，连续按两次程序热键相当于ALT+L功能，重复执行上一次的命令",
	"如果选中，鼠标单击选中列表条目，双击运行",
	"如果选中，允许命令名称一样，否则添加命令时名称一样会失败。"
};

ALMRunConfig::ALMRunConfig()
{
	//初始化默认参数
	config[OrderByPre] = false;
	config[NumberKey] = false;
	config[ExecuteIfOnlyOne] = false;
	config[ShowTip] = false;
	config[AutoRun] = false;
	config[AddToSendTo] = false;
	config[PlayPopupNotify] = false;
	config[SpaceKey] = false;
	config[DoubleToggleFunc] = false;
	config[DoubleClick] = false;
	config[DuplicateCMD] = false;
	cfg_changed = false;
	FavoriteList = NULL;
	order_conf = NULL;
	conf = NULL;
	order_cfg_time = 0;
	cfg_time = 0;

	if (wxGetEnv(wxT("ALMRUN_HOME"),&Home))
	{	
		cfg_file = Home + wxT("config/ALMRun.ini");
		order_cfg_file = Home + wxT("config/Order.ini");
		if (wxFileExists(order_cfg_file))
			order_cfg_time =  wxFileModificationTime(order_cfg_file);
		order_conf = new wxFileConfig(wxT("ALMRun"),wxEmptyString,order_cfg_file,wxEmptyString,wxCONFIG_USE_LOCAL_FILE);
		order_conf->SetExpandEnvVars(false);
		if (!wxFileExists(cfg_file))
			MoveFile(Home + wxGetApp().GetAppName().Append(".ini"),cfg_file);
	}
	if (wxFileExists(cfg_file) == false)
		cfg_file = wxEmptyString;
	else
		cfg_time = wxFileModificationTime(cfg_file);

	//lastId = 0;
	conf = new wxFileConfig(wxT("ALMRun"),wxEmptyString,cfg_file,wxEmptyString,7);
	gui_config[ListFont] = conf->Read("/GUI/ListFont");
	MerryListBoxPanel* listBoxPanel = ::wxGetApp().GetFrame().GetListBoxPanel();
	listBoxPanel->SetFont(gui_config[ListFont]);
	conf->SetPath("/Config");
	conf->SetRecordDefaults(false);
	config_u[CONFIG_VER] = conf->ReadLong("Version",-1);
	if (config_u[CONFIG_VER] == -1)//配置文件未标注版本
		cfg_changed = conf->Write("Version",(config_u[CONFIG_VER] = CONFIG_VERSION));
	config_u[ParamHistoryLimit] = conf->ReadLong("ParamHistoryLimit",ParamHistoryLimit_default);
	CompareMode = conf->ReadLong("CompareMode",0);
	this->set("Explorer",conf->Read("Explorer"));
	this->set("Root",conf->Read("Root"));

	//从配置文件中读取参数，如果不存在则使用默认值
	for(int i=0;i<CONFIG_BOOL_MAX;++i)
		config[i] = conf->ReadBool(config_str[i],config[i]);

	this->set("ShowTrayIcon",config[ShowTrayIcon]);
	this->set("StayOnTop",config[StayOnTop]);
	g_config = this;

	//程序显隐热键配置
	HotKey = conf->Read("HotKey","A-R");
	if (!g_hotkey->RegisterHotkey(g_commands->AddCommand(wxEmptyString,wxEmptyString,"toggleMerry",-1,HotKey,0)))
	{
		this->set("ShowTrayIcon",true);
		wxMessageBox(wxString::Format("热键 %s 注册失败!",HotKey),"错误提示",0x00000100);
	}
	//重载配置热键配置
	HotKeyReLoad = conf->Read("HotKeyReLoad");
	if (!HotKeyReLoad.empty())
		g_hotkey->RegisterHotkey(g_commands->AddCommand(wxEmptyString,wxEmptyString,"ReConfig",-1,HotKeyReLoad,0));
	//上一个运行的命令热键配置
	LastItemHotKey = conf->Read("LastItemHotKey");
	if (!LastItemHotKey.empty())
		g_hotkey->RegisterHotkey(g_commands->AddCommand(wxEmptyString,wxEmptyString,"LastCmd",-2,LastItemHotKey,0));

	this->OldToNew();

	ConfigCommand();
	if (config[RememberFavouratMatch])
		FavoriteList = new wxFileConfig(wxT("ALMRun"),wxEmptyString,Home +  FAVORITELIST_FILE ,wxEmptyString,wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);

#ifndef _DEBUG
	conf->SetPath("/Config");

	#ifdef __WXMSW__
	wxString Sendto = wxStandardPaths::MSWGetShellDir(0x0009) + "/ALMRun.lnk";//CSIDL_SENDTO                    0x0009        // <user name>\SendTo
	wxString Startup = wxStandardPaths::MSWGetShellDir(0x0007) + "/ALMRun.lnk";//CSIDL_STARTUP
	bool InSendTo = ::wxFileExists(Sendto);
	bool InStartup = ::wxFileExists(Startup);
	if (config[AddToSendTo])
	{
		if (InSendTo == false)
		{
			if (!CreateFileShortcut(::wxGetApp().argv[0],Sendto.c_str(),Home,0,_T("ALMRun 快速启动工具")))
				wxMessageBox("添加到<发送到>菜单失败!");
		}
		
	}
	else if (InSendTo)
	{
		wxRemoveFile(Sendto);
	}

	if (config[AutoRun])
	{
		if (InStartup == false)
		{
			if (!CreateFileShortcut(::wxGetApp().argv[0],Startup.c_str(),Home,0,_T("ALMRun 快速启动工具")))
				wxMessageBox("添加自动启动到<开始/启动>失败!");
		}

	}
	else if (InStartup)
	{
		wxRemoveFile(Startup);
	}
	#endif
#endif
}

bool ALMRunConfig::set(const wxString& name,const wxString &value)
{
	if (name.Cmp("Explorer") == 0)
		Explorer = value;
	else if (name.Cmp("Root") == 0)
	{
		Root = value;
		if (Root.empty())
			return false;
		wxSetWorkingDirectory(Root);
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
		config[NumberKey] = (value != 0);
	else if (name.Cmp("ShowTrayIcon") == 0)
	{
		config[ShowTrayIcon] = value != 0;
		::wxGetApp().GetFrame().ShowTrayIcon(config[ShowTrayIcon]);
	}
	else if (name.Cmp("ShowTopTen") == 0)
		config[ShowTopTen] = value != 0;
	else if (name.Cmp("ExecuteIfOnlyOne") == 0)
		config[ExecuteIfOnlyOne] = value != 0;
	else if (name.Cmp("StayOnTop") == 0)
	{
		config[StayOnTop] = value != 0;
		MerryFrame& frm = wxGetApp().GetFrame();
		long style = frm.GetWindowStyle();
		if (config[StayOnTop])
			frm.SetWindowStyle(style|wxSTAY_ON_TOP);
		else if (style & wxSTAY_ON_TOP)
			frm.SetWindowStyle(style^wxSTAY_ON_TOP);
	}
	else
		return false;
	return true;
}

void ALMRunConfig::GuiConfig()
{
	if (!conf)
		return;
	DlgConfig* dlg = new DlgConfig(0);
	dlg->config_hotkey->SetValue(HotKey);
	for(int i=sizeof(config)-1;i>=0;--i)
	{
		if (config[i])
			dlg->config->Check(i);
	}
	if (dlg->ShowModal() == wxID_OK)
	{
		conf->SetPath("/Config");
		if (!dlg->config_hotkey->GetValue().IsSameAs(HotKey))
			conf->Write("HotKey",dlg->config_hotkey->GetValue());
		for(int i=sizeof(config)-1;i>=0;--i)
		{
			if (config[i] != dlg->config->IsChecked(i))
				conf->Write(config_str[i],!config[i]);
		}
		conf->Flush();
		#ifdef __WXMSW__
			//使用POST发送消息，使得这个函数可以快速返回.
			PostMessage(::wxGetApp().GetFrame().GetHWND(),WM_COMMAND,MENU_ITEM_RECONFIG,0);
		#else
			::wxGetApp().GetFrame().NewConfig();
		#endif
	}
	dlg->Destroy();
}

bool ALMRunConfig::SaveCfg()
{
	if (!conf)
		return false;
	conf->Flush();
	if (cfg_time)
	{
		cfg_time = wxFileModificationTime(cfg_file);
		cfg_changed = false;
	}
	return true;
}

int ALMRunConfig::AddCmd(const wxString& cmd,const wxString& name,const wxString& key,const wxString& desc,const int id)
{
	if (!conf)
		return false;
	wxString cmdName = name;
	if (cmdName.empty() && key.empty())
	{
		cmdName = wxFileName(cmd).GetFullName();
		//if (!cmdName.empty())
		//	cmdName.Append(".ALMRun");
	}
	int Id = id;
	if (id == -1)
	{
		for(Id=0;;++Id)
		{
			if (!conf->HasGroup(wxString::Format("/cmds/%d",Id)))
				break;
			if (Id >= 4000)
				return -1;
		}
	}
	int cmdId = g_commands->AddCommand(cmdName,desc,cmd,0,key,(Id << 4) | CMDS_FLAG_CMDS);
	if (cmdId < 0)
	{
		wxMessageBox(wxString::Format("添加命令失败->命令[%d]:%s\n%s",Id,name,MerryGetLastError()),"失败");
		return -1;
	}
	if (!key.empty() && !g_hotkey->RegisterHotkey(cmdId))
	{
		wxMessageBox(wxString::Format("添加命令失败->注册热键失败:%s\n,命令[%d]:%s",key,Id,name),"错误!");
		g_commands->DelCommand(cmdId);
		return -1;
	}
	if (id == -1)
	{
		if (!this->ModifyCmd(Id,cmd,name,key,desc))
			return -1;
	}

	return cmdId;
}

bool ALMRunConfig::ModifyCmd(const int id,const wxString& cmd,const wxString& name,const wxString& key,const wxString& desc)
{
	if (!conf || id < 0)
		return false;
	if (cmd.empty() && !conf->DeleteGroup(wxString::Format("/cmds/%d",id)))
		return false;

	wxString oldPath = conf->GetPath();
	conf->SetPath(wxString::Format("/cmds/%d",id));
	conf->Write("cmd",cmd);
	if (!name.empty())
		conf->Write("name",name);
	else
		conf->DeleteEntry("name");

	if (!key.empty())
		conf->Write("key",key);
	else
		conf->DeleteEntry("key");

	if (!desc.empty())
		conf->Write("desc",desc);
	else
		conf->DeleteEntry("desc");

	conf->SetPath(oldPath);
	return (cfg_changed = true);
}

bool ALMRunConfig::DeleteCmd(const int id)
{
	if (conf && conf->DeleteGroup(wxString::Format("/cmds/%d",id)))
		return (cfg_changed = true);
	return false;
}

bool ALMRunConfig::DeleteDir(const int id)
{
	if (conf && conf->DeleteGroup(wxString::Format("/dirs/%d",id)))
		return true;
	return false;
}

int ALMRunConfig::AddDir(const wxString& path,const wxString& inc,const wxString& exc,const int sub,const UINT id)
{
	if (!conf)
		return -1;
	int i = 0;
	wxString entry;
	if (id == -1)
	{
		do
		{
			entry.Printf("/dirs/%d",i++);
			if (i == 1000)
				return -1;
		
		} while (conf->HasGroup(entry));
	}
	else
	{
		i = id;
		entry.Printf("/dirs/%d",i);
	}
	wxString oldPath = conf->GetPath();
	conf->SetPath(entry);
	conf->Write("path",path);
	conf->Write("sub",sub);
	if (!inc.empty())
		conf->Write("include",inc);
	if (!exc.empty())
		conf->Write("exclude",exc);
	cfg_changed = true;
	return i;
}

bool ALMRunConfig::get(config_item_t config_type)
{
	if (config_type<0 || config_type >= CONFIG_BOOL_MAX)
		return false;
	return this->config[config_type];
}

size_t ALMRunConfig::get_u(config_item_t config_type) const
{
	if (config_type < 0 || config_type >= CONFIG_UINT_MAX)
		return -1;
	return this->config_u[config_type];
}

bool ALMRunConfig::Changed()
{
	if (cfg_changed)
		this->SaveCfg();
	return (cfg_time  && cfg_time != wxFileModificationTime(cfg_file));
}

//新版的配置文件?
void ALMRunConfig::ConfigCommand()
{
	if (!conf)
		return;
	wxString name;
	wxString key;
	wxString cmd;
	wxString desc;
	wxString cmds;
	long cmdId;
	long index = 0;
	//命令
	conf->SetPath("/cmds");
	for(bool test = conf->GetFirstGroup(cmds,index); test ; conf->SetPath("../"),test = conf->GetNextGroup(cmds,index))
    {
		if (!cmds.IsNumber())
			continue;
		conf->SetPath(cmds);
		cmd = conf->Read("cmd");
		if (cmd.empty() && key.empty())
			continue;
		name = conf->Read("name");
		key = conf->Read("key");
		desc = conf->Read("desc");
		cmds.ToLong(&cmdId,10);
		//if (cmdId < 1000 && cmdId > lastId)
		//	lastId = cmdId + 1;
		this->AddCmd(cmd,name,key,desc,cmdId);
    }
	//自动扫描目录配置
	conf->SetPath("/dirs");
	//读取默认参数
	def_dir_cfg.sub = conf->ReadLong("sub",0);
	def_dir_cfg.include = conf->Read("include");
	def_dir_cfg.exclude = conf->Read("exclude");

	wxArrayString paths;
	for(bool test = conf->GetFirstGroup(cmds,index); test ; conf->SetPath("../"),test = conf->GetNextGroup(cmds,index))
	{
		wxArrayString files;
		conf->SetPath(cmds);
		paths = wxSplit(conf->Read("path"),'|');
		if (paths.empty())
			continue;
		for(int i=paths.size()-1;i>=0;--i)
			ListFiles(paths[i],&files, conf->Read("include",def_dir_cfg.include) , conf->Read("exclude",def_dir_cfg.exclude) , conf->ReadLong("sub",def_dir_cfg.sub));
		g_commands->AddFiles(files);
	}
	conf->SetPath("/Config");
}

//旧版自动转换为新版本
void ALMRunConfig::OldToNew()
{
	size_t sizes;
	wxString name;
	wxString key;
	wxString cmd;
	wxString desc;
	wxString cmds;
	conf->SetExpandEnvVars(false);
	if (conf->HasGroup("/Command"))
	{
		sizes = conf->ReadLong("/Command/cmds.size",0);
		while(sizes)
		{
			conf->SetPath(wxString::Format("/cmds/%d",1000+sizes));
			cmds = wxString::Format("/Command/cmds.%d.",sizes);
			--sizes;
			cmd = conf->Read(cmds + "cmd");
			if (cmd.empty() && key.empty())
				continue;
			conf->Write("cmd",cmd);
			conf->Write("name",conf->Read(cmds + "name"));
			conf->Write("key",conf->Read(cmds + "key"));
			conf->Write("desc",conf->Read(cmds + "desc"));
		}
		conf->DeleteGroup("/Command");
		conf->Flush();
	}
	if (conf->HasGroup("/directories"))
	{
		conf->SetPath("/directories");
		sizes = conf->ReadLong("dirs.size",0);
		conf->Write("/dirs/sub",conf->ReadLong("dirs.def.sub",0));
		conf->Write("/dirs/include",conf->Read("dirs.def.include"));
		conf->Write("/dirs/exclude",conf->Read("dirs.def.exclude"));
		while(sizes)
		{
			conf->SetPath(wxString::Format("/dirs/%d",1000+sizes));
			cmds = wxString::Format("/directories/dirs.%d.",sizes);
			--sizes;
			conf->Write("path",conf->Read(cmds + "path"));
			conf->Write("include",conf->Read(cmds + "include"));
			conf->Write("exclude", conf->Read(cmds + "exclude"));
			conf->Write("sub",conf->ReadLong(cmds + "sub",0));
		}
		conf->DeleteGroup("/directories");
		conf->Flush();
	}
	conf->SetExpandEnvVars(true);
	conf->SetPath("/Config");
}

int ALMRunConfig::SetcmdOrder(wxString& cmd,int order)
{
	if (order_cfg_time && order_cfg_time != wxFileModificationTime(order_cfg_file))
	{
		wxDELETE(order_conf);
		order_conf = new wxFileConfig(wxT("ALMRun"),wxEmptyString,order_cfg_file,wxEmptyString,wxCONFIG_USE_LOCAL_FILE); 
	}
	order = order_conf->ReadLong(cmd,0);
	order_conf->Write(cmd,++order);
	order_conf->Flush();
	order_cfg_time	= wxFileModificationTime(order_cfg_file);
	return order;
}

bool ALMRunConfig::SetFavorite(const wxString& key,const wxString& name)
{
	if (FavoriteList)
	{
		FavoriteList->Write(key,name);
		return FavoriteList->Flush();
	}
	return false;
}

const wxString ALMRunConfig::GetFavorite(const wxString& key)
{
	if (FavoriteList)
		return FavoriteList->Read(key);
	return wxEmptyString;
}

void ALMRunConfig::get(const wxString& name)
{

}

ALMRunConfig::~ALMRunConfig()
{
	__DEBUG_BEGIN("")
	if (conf && cfg_changed)
		conf->Flush();
	wxDELETE(conf);
	wxDELETE(order_conf);
	wxDELETE(FavoriteList);
	__DEBUG_END("")
}

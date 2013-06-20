#include "MerryCommandManager.h"
#include "MerryHotkey.h"
#include "ALMRunConfig.h"
#include "MerryApp.h"
#include "DlgConfig.h"
ALMRunConfig* g_config = NULL;
ALMRunConfig::ALMRunConfig(void)
{
	config_tip[NumberKey] = "选中时敲0-9键执行对应编号的快捷项";
	config_tip[ShowTrayIcon] = "选中时在系统托盘显示图标";
	config_tip[ShowTopTen] = "选中时仅显示前10项快捷项";
	config_tip[ExecuteIfOnlyOne] = "选中时列表只剩一项时无需按键立即执行";
	config_tip[IndexFrom0to9] = "如果未选中，编号顺序为 1, 2, ..., 9, 0";
	config_tip[OrderByPre] = "如果选中, 命令列表中前辍匹配的排前面";
	if (wxGetEnv("ALMRUN_HOME",&Home))
	{	
		cfg_file = Home + "config/ALMRun.ini";
		if (!wxFileExists(cfg_file))
			MoveFile(Home + wxGetApp().GetAppName().Append(".ini"),cfg_file);
	}
	if (wxFileExists(cfg_file) == false)
	{
		CompareMode = 0;
		config[NumberKey] = false;
		config[ShowTrayIcon] = true;
		config[ShowTopTen] = true;
		config[ExecuteIfOnlyOne] = false;
		config[IndexFrom0to9] = false;
		config[OrderByPre] = false;
		conf = NULL;
		cfg_time = 0;
		return;
	}
	lastId = 0;
	conf = new wxFileConfig("ALMRun",wxEmptyString,cfg_file,wxEmptyString,wxCONFIG_USE_LOCAL_FILE);
	conf->SetPath("/Config");
	conf->SetRecordDefaults(false);
	config[OrderByPre] = conf->ReadBool("OrderByPre",false);
	config[NumberKey] = conf->ReadBool("NumberKey",false);
	config[ShowTopTen] = conf->ReadBool("ShowTopTen",true);
	config[ExecuteIfOnlyOne] = conf->ReadBool("ExecuteIfOnlyOne",false);
	config[IndexFrom0to9] = conf->ReadBool("IndexFrom0to9",true);
	CompareMode = conf->ReadLong("CompareMode",0);
	HotKey = conf->Read("HotKey","A-R");
	HotKeyReLoad = conf->Read("HotKeyReLoad");
	this->set("Explorer",conf->Read("Explorer"));
	this->set("Root",conf->Read("Root"));
	this->set("ShowTrayIcon",conf->ReadBool("ShowTrayIcon",true));
	if (!g_hotkey->RegisterHotkey(g_commands->AddCommand(wxEmptyString,wxEmptyString,"toggleMerry",-1,HotKey,0)))
	{
		this->set("ShowTrayIcon",true);
		wxMessageBox(wxString::Format("热键 %s 注册失败!",HotKey),"错误提示",0x00000100);
	}
	if (!HotKeyReLoad.empty())
		g_hotkey->RegisterHotkey(g_commands->AddCommand(wxEmptyString,wxEmptyString,"ReConfig",-1,HotKeyReLoad,0));
	this->OldToNew();
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

void ALMRunConfig::GuiConfig()
{
	if (!conf)
		return;
	wxString config_str[] = {"NumberKey","ShowTrayIcon","ShowTopTen","ExecuteIfOnlyOne","IndexFrom0to9","OrderByPre",};
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
		::wxGetApp().GetFrame().NewConfig();
	}
	dlg->Destroy();
}

bool ALMRunConfig::SaveCfg()
{
	if (conf)
	{
		conf->Flush();
		cfg_time = wxFileModificationTime(cfg_file);
		return true;
	}
	return false;
}

bool ALMRunConfig::AddCmd(const wxString& cmd,const wxString& name,const wxString& key,const wxString& desc,const int id)
{
	wxString cmdName = name;
	if (cmdName.empty() && key.empty())
	{
		cmdName = wxFileName(cmd).GetName();
		if (!cmdName.empty())
			cmdName.Append(".ALMRun");
	}
	int cmdId = id;
	if (cmdId == -1)
	{
		for(;;++lastId)
		{
			if (!conf->Exists(wxString::Format("/cmds/%d",lastId)))
				break;
		}
		cmdId = lastId;
	}
	cmdId = g_commands->AddCommand(cmdName,desc,cmd,0,key,0,(cmdId << 4) | CMDS_FLAG_ALMRUN_CMDS);
	if (cmdId < 0)
	{
		wxMessageBox(wxString::Format("添加命令失败->命令[%d]:%s",id,name),"参数错误");
		return false;
	}
	if (!key.empty() && !g_hotkey->RegisterHotkey(cmdId))
	{
		wxMessageBox(wxString::Format("添加命令失败->注册热键失败:%s\n,命令[%d]:%s",key,id,name),"错误!");
		g_commands->DelCommand(cmdId);
		return false;
	}
	if (id == -1)
		return this->ModifyCmd(lastId++,cmd,name,key,desc);

	return true;
}

bool ALMRunConfig::ModifyCmd(const int id,const wxString& cmd,const wxString& name,const wxString& key,const wxString& desc)
{
	if (!conf)
		return false;
	if (cmd.empty() && !conf->DeleteGroup(wxString::Format("/cmds/%d",id)))
		return false;

	wxString oldPath = conf->GetPath();
	conf->SetPath(wxString::Format("/cmds/%d",id));
	conf->Write("cmd",cmd);
	if (!name.empty())
		conf->Write("name",name);
	if (!key.empty())
		conf->Write("key",key);
	if (!desc.empty())
		conf->Write("desc",desc);
	conf->SetPath(oldPath);
	return this->SaveCfg();
}

bool ALMRunConfig::DeleteCmd(const int id)
{
	if (conf && conf->DeleteGroup(wxString::Format("/cmds/%d",id)))
		return this->SaveCfg();
	return false;
}

bool ALMRunConfig::DeleteDir(const int id)
{
	if (conf && conf->DeleteGroup(wxString::Format("/dirs/%d",id)))
	{
		conf->Flush();
		return true;
	}
	return false;
}

int ALMRunConfig::AddDir(const wxString& path,const wxString& inc,const wxString& exc,const int sub)
{
	if (!conf)
		return -1;
	int i = 0;
	wxString entry;
	do
	{
		entry.Printf("/dirs/%d",i++);
		if (i == 1000)
			return -1;
		
	} while (conf->HasGroup(entry));
	wxString oldPath = conf->GetPath();
	conf->SetPath(entry);
	conf->Write("path",path);
	conf->Write("sub",sub);
	if (!inc.empty())
		conf->Write("include",inc);
	if (!exc.empty())
		conf->Write("exclude",exc);
	conf->Flush();
	return i;
}

bool ALMRunConfig::Changed()
{
	return (cfg_time  && cfg_time != wxFileModificationTime(cfg_file));
}

void ALMRunConfig::ListFiles(const wxString& dirname,wxArrayString *files,const wxString& filespec,const int sub)
{
	wxArrayString specs = wxSplit(filespec,'|');
	if (specs.GetCount() == 0)
		specs.Add(wxEmptyString);
	return this->ListFiles(dirname,files,specs,sub);
}

void ALMRunConfig::ListFiles(const wxString& dirname,wxArrayString *files,const wxArrayString& filespec,const int sub)
{
	static int cur_sub_dir = 0;
	int flags = wxDIR_HIDDEN|wxDIR_FILES;
	wxDir dir(dirname);
	if (!dir.IsOpened())
		return;
	if (sub == -1)
		flags |= wxDIR_DIRS;
	for(int i=filespec.GetCount() - 1; i >= 0;--i)
		dir.GetAllFiles(dirname,files,filespec[i],flags);

	if (sub == -1 || cur_sub_dir == sub)
		return;
	++cur_sub_dir;//子目录级别+1
	wxString file;
	bool test = dir.GetFirst(&file,wxEmptyString,wxDIR_DIRS|wxDIR_HIDDEN);
	wxString path = dir.GetNameWithSep();
	while(test)
	{
		ListFiles(path+file,files,filespec,sub);
		test = dir.GetNext(&file);
	}
	--cur_sub_dir;//子目录级别-1
	return;
}

/*
旧版的配置方法
void ALMRunConfig::ConfigCommand()
{
	conf->SetPath("/Command");
	size_t sizes;
	wxString name;
	wxString key;
	wxString cmd;
	wxString desc;
	wxString cmds;
	wxString def_incl;
	wxString def_excl;
	int cmdId,defsub;
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
		cmdId = g_commands->AddCommand(name,desc,cmd,0,key,0,CMDS_FLAG_ALMRUN_CMDS<<16);
		if (cmdId < 0 || key.empty())
			continue;
		g_hotkey->RegisterHotkey(cmdId);
	}
	conf->SetPath("/directories");
	sizes = conf->ReadLong("dirs.size",0);
	defsub = conf->ReadLong("dirs.def.sub",0);
	def_incl = conf->Read("dirs.def.include");
	def_excl = conf->Read("dirs.def.exclude");
	wxArrayString paths;
	while(sizes)
	{
		wxArrayString files;
		cmds = wxString::Format("dirs.%d.",sizes);
		--sizes;
		paths = wxSplit(conf->Read(cmds + "path"),'|');
		if (paths.empty())
			continue;
		key = conf->Read(cmds + "include",def_incl);
		cmdId = conf->ReadLong(cmds + "sub",defsub);
		for(int i=paths.size()-1;i>=0;--i)
			this->ListFiles(paths[i],&files,key,cmdId);
		key = conf->Read(cmds + "exclude",def_excl);
		g_commands->AddFiles(files,wxSplit(key,'|'));
	}
	conf->SetPath("/Config");
}
*/

//新版的配置文件
void ALMRunConfig::ConfigCommand()
{
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
		if (cmdId < 1000 && cmdId > lastId)
			lastId = cmdId + 1;
		this->AddCmd(cmd,name,key,desc,cmdId);
    }
	//自动扫描目录配置
	conf->SetPath("/dirs");
	int defsub = conf->ReadLong("sub",0);
	wxString def_incl = conf->Read("include");
	wxString def_excl = conf->Read("exclude");
	wxArrayString paths;
	for(bool test = conf->GetFirstGroup(cmds,index); test ; conf->SetPath("../"),test = conf->GetNextGroup(cmds,index))
	{
		wxArrayString files;
		conf->SetPath(cmds);
		paths = wxSplit(conf->Read("path"),'|');
		if (paths.empty())
			continue;
		key = conf->Read("include",def_incl);
		cmdId = conf->ReadLong("sub",defsub);
		for(int i=paths.size()-1;i>=0;--i)
			this->ListFiles(paths[i],&files,key,cmdId);
		key = conf->Read("exclude",def_excl);
		g_commands->AddFiles(files,wxSplit(key,'|'));
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

void ALMRunConfig::get(const wxString& name)
{

}

ALMRunConfig::~ALMRunConfig(void)
{
	if (conf)
		delete conf;
	conf = NULL;
}

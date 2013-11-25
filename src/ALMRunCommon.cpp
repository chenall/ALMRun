#include "ALMRunCommon.h"
#include "MerryCommand.h"
#include "wx/dir.h"
#include <wx/clipbrd.h>
#include "MerryController.h"

#ifdef __WXMSW__
	#include <wx/msw/registry.h>
#endif
/* 
函数功能：对指定文件在指定的目录下创建其快捷方式 
函数参数： 
lpszFileName    指定文件，为NULL表示当前进程的EXE文件。 
lpszLnkFilePath 要创建的快捷方式全路径.
wHotkey         为0表示不设置快捷键 
pszDescription  备注 
iShowCmd        运行方式，默认为常规窗口 
*/  
BOOL CreateFileShortcut(LPCWSTR lpszFileName, LPCWSTR lpszLnkFilePath, LPCWSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd)  
{  
    if (lpszLnkFilePath == NULL)  
        return FALSE;  
  
    HRESULT hr;  
    IShellLink     *pLink;  //IShellLink对象指针  
    IPersistFile   *ppf; //IPersisFil对象指针  
      
    //创建IShellLink对象  
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);  
    if (FAILED(hr))  
        return FALSE;  
      
    //从IShellLink对象中获取IPersistFile接口  
    hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);  
    if (FAILED(hr))  
    {  
        pLink->Release();  
        return FALSE;  
    }  
      
    //目标  
    if (lpszFileName == NULL)  
        pLink->SetPath(_wpgmptr);  
    else  
        pLink->SetPath(lpszFileName);  
    //工作目录  
    if (lpszWorkDir != NULL)  
		pLink->SetWorkingDirectory(lpszWorkDir);  
      
    //快捷键  
    if (wHotkey != 0)  
        pLink->SetHotkey(wHotkey);  
      
    //备注  
    if (lpszDescription != NULL)  
        pLink->SetDescription(lpszDescription);  
      
    //显示方式  
    pLink->SetShowCmd(iShowCmd);  
  
  
    //快捷方式的路径 + 名称  

    //保存快捷方式到指定目录下  
    hr = ppf->Save(lpszLnkFilePath,TRUE);  
      
    ppf->Release();  
    pLink->Release();  
    return SUCCEEDED(hr);  
}

void ListFiles(const wxString& dirname,wxArrayString *files,const wxString& filespec,const wxString& exclude, const int sub)
{
	wxArrayString specs = wxSplit(filespec,'|');
	wxArrayString exc = wxSplit(exclude,'|');
	if (specs.GetCount() == 0)
		specs.Add(wxEmptyString);
	ListFiles(dirname,files,specs,sub);
	for(int i = files->Count() - 1;i>=0;--i)
	{
		int j = -1;
		for(j = exc.size()-1;j >= 0;--j)
		{//过滤符合条件的内容
			if (files->Item(i).Matches(exc[j]))
				break;
		}
		if (j >= 0)
			files->RemoveAt(i);
	}
	specs.Clear();
	exc.Clear();
}

void ListFiles(const wxString& dirname,wxArrayString *files,const wxArrayString& filespec,const int sub)
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

wxString GetClipboardText()
{
	wxString ClipText = wxEmptyString;
	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported(wxDF_TEXT))
		{
			wxTextDataObject data;
			wxTheClipboard->GetData(data);
			ClipText = data.GetText();
		}
		wxTheClipboard->Close();
	}
	return ClipText;
}

ALMRunCMDBase* lua_GetCommand(lua_State* L,int flags)
{
	if (!lua_istable(L, -1))
		return NULL;
	ALMRunCMDBase *cmd = new ALMRunCMDBase();
	int it = lua_gettop(L);

	lua_pushstring(L, "name");
	lua_rawget(L, it);
	cmd->Name = wxString(lua_tostring(L, -1), wxConvLocal);

	lua_pushstring(L, "desc");
	lua_rawget(L, it);
	cmd->Desc = wxString(lua_tostring(L, -1), wxConvLocal);

	lua_pushstring(L, "key");
	lua_rawget(L, it);
	cmd->Key = wxString(lua_tostring(L, -1), wxConvLocal);

	lua_pushstring(L, "cmd");
	lua_rawget(L, it);
	if (lua_isfunction(L,-1))
	{
		cmd->FuncRef = luaL_ref(L, LUA_REGISTRYINDEX);
		cmd->cmdLine = wxEmptyString;
	}
	else
	{
		cmd->FuncRef = 0;
		cmd->cmdLine = wxString(lua_tostring(L, -1), wxConvLocal);
	}

	lua_pushstring(L, "order");
	lua_rawget(L, it);
	cmd->Order = lua_tointeger(L,-1);

	lua_pushstring(L, "flags");
	lua_rawget(L,it);
	cmd->Flags = lua_tointeger(L,-1) | CMDS_FLAG_LUA | flags;

	//func参数是为了兼容旧版配置文件，新的版本简化配置，不再使用func参数
	if (cmd->FuncRef == 0)
	{
		lua_pushstring(L, "func");
		lua_rawget(L, it);
		if (lua_isfunction(L, -1))
			cmd->FuncRef = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	lua_settop(L,it);
	return cmd;
}

//根据汉字区位表获取中文拼音首字母

//区位表汉字一区，按拼音首字母归类区
static int li_SecPosValue[] = {
		0xB0A1, 0xB0C5, 0xB2C1, 0xB4EE, 0xB6EA, 0xB7A2, 0xB8C1, 0xB9FE, 0xBBF7, 0xBFA6, 0xC0AC,
		0xC2E8, 0xC4C3, 0xC5B6, 0xC5BE, 0xC6DA, 0xC8BB, 0xC8F6, 0xCBFA, 0xCDDA, 0xCEF4, 0xD1B9, 0xD4D1
	};
static char* lc_FirstLetter = "ABCDEFGHJKLMNOPQRSTWXYZ";
//第二区，无规则区，只能通过查表的方式获取
static char* ls_SecondSecTable[] ={
	"CJWGNSPGCGNEGYPBTYYZDXYKYGTZJNMJQMBSGZSCYJSYYFPGKBZGYDYWJKGKLJSWKPJQHYJWRDZLSYMRYPYWWCCKZNKYYG",
	"TTNGJEYKKZYTCJNMCYLQLYPYSFQRPZSLWBTGKJFYXJWZLTBNCXJJJJTXDTTSQZYCDXXHGCKBPHFFSSTYBGMXLPBYLLBHLX",
	"SMZMYJHSOJNGHDZQYKLGJHSGQZHXQGKXZZWYSCSCJXYEYXADZPMDSSMZJZQJYZCJJFWQJBDZBXGZNZCPWHWXHQKMWFBPBY",
	"DTJZZKXHYLYGXFPTYJYYZPSZLFCHMQSHGMXXSXJYQDCSBBQBEFSJYHWWGZKPYLQBGLDLCDTNMAYDDKSSNGYCSGXLYZAYPN",
	"PTSDKDYLHGYMYLCXPYCJNDQJWXQXFYYFJLEJPZRXCCQWQQSBZKYMGPLBMJRQCFLNYMYQMSQYRBCJTHZTQFRXQHXMQJCJLY",
	"QGJMSHZKBSWYEMYLTXFSYDXWLYCJQXSJNQBSCTYHBFTDCYZDJWYGHQFRXWCKQKXEBPTLPXJZSRMEBWHJLBJSLYYSMDXLCL",
	"QKXLHXJRZJMFQHXHWYWSBHTRXXGLHQHFNMGYKLDYXZPYLGGSMTCFBAJJZYLJTYANJGBJPLQGSZYQYAXBKYSECJSZNSLYZH",
	"ZXLZCGHPXZHZNYTDSBCJKDLZAYFFYDLEBBGQYZKXGLDNDNYSKJSHDLYXBCGHXYPKDJMMZNGMMCLGWZSZXZJFZNMLZZTHCS",
	"YDBDLLSCDDNLKJYKJSYCJLKWHQASDKNHCSGAGHDAASHTCPLCPQYBSZMPJLPCJOQLCDHJJYSPRCHNWJNLHLYYQYYWZPTCZG",
	"WWMZFFJQQQQYXACLBHKDJXDGMMYDJXZLLSYGXGKJRYWZWYCLZMSSJZLDBYDCFCXYHLXCHYZJQSQQAGMNYXPFRKSSBJLYXY",
	"SYGLNSCMHCWWMNZJJLXXHCHSYZSTTXRYCYXBYHCSMXJSZNPWGPXXTAYBGAJCXLYXDCCWZOCWKCCSBNHCPDYZNFCYYTYCKX",
	"KYBSQKKYTQQXFCMCHCYKELZQBSQYJQCCLMTHSYWHMKTLKJLYCXWHEQQHTQKZPQSQSCFYMMDMGBWHWLGSLLYSDLMLXPTHMJ",
	"HWLJZYHZJXKTXJLHXRSWLWZJCBXMHZQXSDZPSGFCSGLSXYMJSHXPJXWMYQKSMYPLRTHBXFTPMHYXLCHLHLZYLXGSSSSTCL",
	"SLDCLRPBHZHXYYFHBMGDMYCNQQWLQHJJCYWJZYEJJDHPBLQXTQKWHLCHQXAGTLXLJXMSLJHTZKZJECXJCJNMFBYCSFYWYB",
	"JZGNYSDZSQYRSLJPCLPWXSDWEJBJCBCNAYTWGMPAPCLYQPCLZXSBNMSGGFNZJJBZSFZYNTXHPLQKZCZWALSBCZJXSYZGWK",
	"YPSGXFZFCDKHJGXTLQFSGDSLQWZKXTMHSBGZMJZRGLYJBPMLMSXLZJQQHZYJCZYDJWFMJKLDDPMJEGXYHYLXHLQYQHKYCW",
	"CJMYYXNATJHYCCXZPCQLBZWWYTWBQCMLPMYRJCCCXFPZNZZLJPLXXYZTZLGDLTCKLYRZZGQTTJHHHJLJAXFGFJZSLCFDQZ",
	"LCLGJDJZSNZLLJPJQDCCLCJXMYZFTSXGCGSBRZXJQQCTZHGYQTJQQLZXJYLYLBCYAMCSTYLPDJBYREGKLZYZHLYSZQLZNW",
	"CZCLLWJQJJJKDGJZOLBBZPPGLGHTGZXYGHZMYCNQSYCYHBHGXKAMTXYXNBSKYZZGJZLQJTFCJXDYGJQJJPMGWGJJJPKQSB",
	"GBMMCJSSCLPQPDXCDYYKYPCJDDYYGYWRHJRTGZNYQLDKLJSZZGZQZJGDYKSHPZMTLCPWNJYFYZDJCNMWESCYGLBTZZGMSS",
	"LLYXYSXXBSJSBBSGGHFJLYPMZJNLYYWDQSHZXTYYWHMCYHYWDBXBTLMSYYYFSXJCBDXXLHJHFSSXZQHFZMZCZTQCXZXRTT",
	"DJHNRYZQQMTQDMMGNYDXMJGDXCDYZBFFALLZTDLTFXMXQZDNGWQDBDCZJDXBZGSQQDDJCMBKZFFXMKDMDSYYSZCMLJDSYN",
	"SPRSKMKMPCKLGTBQTFZSWTFGGLYPLLJZHGJJGYPZLTCSMCNBTJBQFKDHBYZGKPBBYMTDSSXTBNPDKLEYCJNYCDYKZTDHQH",
	"SYZSCTARLLTKZLGECLLKJLQJAQNBDKKGHPJTZQKSECSHALQFMMGJNLYJBBTMLYZXDXJPLDLPCQDHZYCBZSCZBZMSLJFLKR",
	"ZJSNFRGJHXPDHYJYBZGDLQCSEZGXLBLGYXTWMABCHECMWYJYZLLJJYHLGNDJLSLYGKDZPZXJYYZLWCXSZFGWYYDLYHCLJS",
	"CMBJHBLYZLYCBLYDPDQYSXQZBYTDKYXJYYCNRJMPDJGKLCLJBCTBJDDBBLBLCZQRPYXJCJLZCSHLTOLJNMDDDLNGKATHQH",
	"JHYKHEZNMSHRPHQQJCHGMFPRXHJGDYCHGHLYRZQLCYQJNZSQTKQJYMSZSWLCFQQQXYFGGYPTQWLMCRNFKKFSYYLQBMQAMM",
	"MYXCTPSHCPTXXZZSMPHPSHMCLMLDQFYQXSZYJDJJZZHQPDSZGLSTJBCKBXYQZJSGPSXQZQZRQTBDKYXZKHHGFLBCSMDLDG",
	"DZDBLZYYCXNNCSYBZBFGLZZXSWMSCCMQNJQSBDQSJTXXMBLTXZCLZSHZCXRQJGJYLXZFJPHYMZQQYDFQJJLZZNZJCDGZYG",
	"CTXMZYSCTLKPHTXHTLBJXJLXSCDQXCBBTJFQZFSLTJBTKQBXXJJLJCHCZDBZJDCZJDCPRNPQCJPFCZLCLZXZDMXMPHJSGZ",
	"GSZZQLYLWTJPFSYASMCJBTZYYCWMYTZSJJLJCQLWZMALBXYFBPNLSFHTGJWEJJXXGLLJSTGSHJQLZFKCGNNNSZFDEQFHBS",
	"AQTGYLBXMMYGSZLDYDQMJJRGBJTKGDHGKBLQKBDMBYLXWCXYTTYBKMRTJZXQJBHLMHMJJZMQASLDCYXYQDLQCAFYWYXQHZ",
};

wxString GetPinYin(const wxString& source)
{
	wxString pinyin = wxEmptyString;
	int W;
	UCHAR H,L;
	UINT i, len;
	std::string name = source.ToStdString();
	len = name.length();
	for (i = 0; i < len; ++i)
	{
		H = name[i];
		L = ((i+1)< len)?name[i+1]:0;//防止访问越界.
		//汉字从16区(A1)开始到87区(F7)
		//一个区94个,位码 A1 - FE
		if (H < 0xB0 ||  L < 0xA1 || H > 0xF7 || L == 0xFF)
		{
			pinyin.Append(H);
			continue;
		}

		if (H < 0xD8)//(H >= 0xB0 && H <=0xD7)//查询文字在一级汉字区(16-55)
		{
			W = (H << 8) | L;
			for (int j = 22; j >= 0; --j)
			{
				if (W >= li_SecPosValue[j])
				{
					pinyin.Append(lc_FirstLetter[j]);
					break;
				}
			}
		}
		else// if (H >= 0xD8 && H <= 0xF7)//查询中文在二级汉字区(56-87)
			pinyin.Append(ls_SecondSecTable[H-0xD8][L-0xA1]);
		++i;
	}
	return pinyin;
}

#ifdef __WXMSW__

BOOL chkSysCmd(const wxString& cmdLine,size_t * const cmdStart = NULL)
{
	wxString cmdFlags("@+>*");
	int n = 0;
	int n_size = cmdLine.size();
	for(n = 0;n<n_size;++n)
	{
		if (cmdFlags.Index(cmdLine[n]) == wxNOT_FOUND)
			break;
	}
	if (n >= n_size)
		return true;

	if (cmdStart)
		*cmdStart = n;
	
	wxString cmdName = cmdLine.substr(n);

	if (cmdName.find("://",3,3) !=  wxNOT_FOUND //网址类型
		 || cmdName.StartsWith("::") //:: 类型系统功能调用
		 || cmdName.StartsWith("\\\\")//网络地址或系统功能调用
		)
	{
		return true;
	}

	return false;
}
//简单的分离命令和参数函数
wxString ParseCmd(const wxString& cmdLine,wxString* const cmdArg)
{
	size_t cmd_len = cmdLine.size();
	wxString cmdFlags("@+>*");
	wxString cmd = wxEmptyString;
	size_t n;
	for(n = 0;n<cmd_len;++n)
	{
		if (cmdFlags.Index(cmdLine[n]) == wxNOT_FOUND)
			break;
	}
	if (n >= cmd_len)//命令行可能为空
		return cmdLine;

	wxString cmd_flag = cmdLine.Left(n);
	size_t cmd_pos = n;
	if (cmdLine[n] == '\"')//带引号
	{
		cmd_pos += 1;
		for(++n;n<cmd_len;++n)
		{
			if (cmdLine[n] == '\"')
				break;
		}
		cmd = GetCMDPath(cmdLine.substr(cmd_pos,n-cmd_pos));

		goto getParam;
	}
	else if (chkSysCmd(cmdLine.substr(n)))
		return wxEmptyString;
	else
	{
		cmd = GetCMDPath(cmdLine.substr(n));
		if (!cmd.empty())
			goto checkCmd;
	}

	for(;n<cmd_len;++n)
	{
		wxChar c = cmdLine[n];
		if (c == ' ')
		{
			wxString _tmp = GetCMDPath(cmdLine.substr(cmd_pos,n-cmd_pos));
			if (!_tmp.empty())
			{
				cmd = _tmp;
				break;
			}
		}
	}

getParam:
	if (cmdArg)
	{
		for(++n;n<cmd_len;++n)
		{
			wxChar  c = cmdLine[n];
			switch(c)
			{
				case ' ':
				case '\t':
				case '\r':
				case '\n':
					continue;
				default:
					break;
			}
			break;
		}
		if (n<cmd_len)
			*cmdArg = cmdLine.substr(n);
	}
checkCmd:
	if (cmd.empty())
		return cmd;
	return cmd_flag + cmd;
}

wxString GetCMDPath(const wxString& commandLine,const wxString& workingDir)
{
	size_t cmdIndex;
	if (commandLine.empty() || chkSysCmd(commandLine,&cmdIndex))
		return commandLine;

	wxString cmdName =::wxExpandEnvVars(commandLine.substr(cmdIndex));
	wxFileName fn = wxFileName(cmdName);

	//如果文件存在返回文件路径
	if (!workingDir.empty() && wxDirExists(workingDir))
		fn.SetCwd(workingDir);

	if (fn.IsDir() && !::wxDirExists(cmdName))
		return wxEmptyString;

	if (fn.Exists())
	{
		fn.MakeAbsolute();
		return fn.GetFullPath();
	}

	bool hasvol = fn.HasVolume();
	bool hasext = fn.HasExt();

	if (fn.GetPath().empty())//无路径信息，首先尝试读取注册表App Paths位置
	{
		wxString regPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + cmdName;
		if (!hasext)//无扩展名，添加.exe
			regPath.Append(".exe");
		wxRegKey reg;
		wxRegKey::StdKey hk[2] = {wxRegKey::HKCU,wxRegKey::HKLM};//在HKCU和HKLM中查找
		for(int i=0;i<2;++i)
		{
			reg.SetName(hk[i],regPath);
			if (!reg.Exists())
				continue;
			regPath = reg.QueryDefaultValue();
			if (regPath.empty())
				continue;
			if (wxFileExists(regPath))//有找到，直接返回
				return regPath;
		}
	}

	wxArrayString mcwd;//根据PATH路径查询文件
	wxArrayString wExt;//文件扩展名

	if (fn.IsRelative() && !fn.IsPathSeparator(cmdName[0]))//相对路径在PATH和当前目录中查询
		mcwd = ::wxSplit(wxString::Format("%s;%s",wxGetCwd(),wxGetenv("PATH")),';','\0');
	else//是个绝对路径或根路径
		mcwd.Add("");

	if (!hasext)//无扩展，查找符合PATHEXT变量设定扩展的文件
	{
		cmdName.Append(".*");//使用.*查找所有，再根据PATHEXT的扩展名判断
		wExt = ::wxSplit(wxGetenv("PATHEXT"),';','\0');
	}

	size_t mc_size = mcwd.size();

	for(size_t i = 0;i< mc_size;++i)//循环查找所有路径下的文件
	{
		wxString strPath = mcwd[i];
		if (!strPath.empty())
		{
			if (!::wxDirExists(mcwd[i]))//路径错误或不存在不查找
				continue;
			if(!::wxEndsWithPathSeparator(strPath))//路径不是以"\\"结尾自动添加
				strPath += '\\';
		}
		TCHAR szFind[MAX_PATH] = {_T("\0")};
		WIN32_FIND_DATA findFileData;
		BOOL bRet = true;
		BOOL bFind = false;

		strPath += cmdName;
		_tcscpy_s(szFind, MAX_PATH, strPath.c_str());
 
		HANDLE hFind = ::FindFirstFile(szFind, &findFileData);

		if (INVALID_HANDLE_VALUE == hFind)
			continue;
			while (bRet)
		{
			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{//非目录
				wxString file(findFileData.cFileName);
				size_t pos = file.find_last_of('.');
				//查找的文件有扩展名说明已经找到了
				//若没有扩展名，并且找到的文件也没有扩展名，也是符合
				//否则查找符合PATHEXT的扩展名
				if (hasext || pos == wxNOT_FOUND||wExt.Index(file.substr(pos),false,true) != wxNOT_FOUND)
				{
					bFind = true;
					strPath = wxFileName(strPath).GetPathWithSep() + file;
					break;
				}
			}
			bRet = ::FindNextFile(hFind, &findFileData);
		}

 		::FindClose(hFind);
		if (bFind)
			return strPath;
	}

	return wxEmptyString;
}

bool RunCMD(const wxString& cmdLine,const wxString& cmdArg)
{
	wxString arg;
	wxString argt = wxEmptyString;
	wxString cmd = ParseCmd(cmdLine,&arg);
	bool winexec = false;
	if (cmd.empty())//没有找到命令
	{
		winexec = true;
		cmd = cmdLine;
	}

	//替换{%c}为剪贴板内容
	if (cmd.find("{%c}") != wxNOT_FOUND)
		cmd.Replace("{%c}",GetClipboardText());

	//替换{%wt}为窗体标题
	if (cmd.find("{%wt}") != wxNOT_FOUND)
		cmd.Replace("{%wt}",g_controller->GetWindowText(g_controller->GetForegroundWindow()));

	if (cmd.find("{%p}") != wxNOT_FOUND)
		cmd.Replace("{%p}",cmdArg);
	else if (cmd.find("{%p+}") != wxNOT_FOUND)
		cmd.Replace("{%p+}",cmdArg);
	else
		argt = wxT(' ') + cmdArg;

	if (winexec)
	{
		cmd += argt;
		cmd = wxString::Format("\2%s",::wxExpandEnvVars(cmd));
	}
	else
	{
		cmd.insert(0,'\1');
		arg += argt;
		arg = ::wxExpandEnvVars(arg);
	}
	return g_controller->ShellExecute(cmd,arg);
}

#endif
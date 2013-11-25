#include "ALMRunCommon.h"
#include "MerryCommand.h"
#include "wx/dir.h"
#include <wx/clipbrd.h>
#include "MerryController.h"

#ifdef __WXMSW__
	#include <wx/msw/registry.h>
#endif
/* 
�������ܣ���ָ���ļ���ָ����Ŀ¼�´������ݷ�ʽ 
���������� 
lpszFileName    ָ���ļ���ΪNULL��ʾ��ǰ���̵�EXE�ļ��� 
lpszLnkFilePath Ҫ�����Ŀ�ݷ�ʽȫ·��.
wHotkey         Ϊ0��ʾ�����ÿ�ݼ� 
pszDescription  ��ע 
iShowCmd        ���з�ʽ��Ĭ��Ϊ���洰�� 
*/  
BOOL CreateFileShortcut(LPCWSTR lpszFileName, LPCWSTR lpszLnkFilePath, LPCWSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd)  
{  
    if (lpszLnkFilePath == NULL)  
        return FALSE;  
  
    HRESULT hr;  
    IShellLink     *pLink;  //IShellLink����ָ��  
    IPersistFile   *ppf; //IPersisFil����ָ��  
      
    //����IShellLink����  
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);  
    if (FAILED(hr))  
        return FALSE;  
      
    //��IShellLink�����л�ȡIPersistFile�ӿ�  
    hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);  
    if (FAILED(hr))  
    {  
        pLink->Release();  
        return FALSE;  
    }  
      
    //Ŀ��  
    if (lpszFileName == NULL)  
        pLink->SetPath(_wpgmptr);  
    else  
        pLink->SetPath(lpszFileName);  
    //����Ŀ¼  
    if (lpszWorkDir != NULL)  
		pLink->SetWorkingDirectory(lpszWorkDir);  
      
    //��ݼ�  
    if (wHotkey != 0)  
        pLink->SetHotkey(wHotkey);  
      
    //��ע  
    if (lpszDescription != NULL)  
        pLink->SetDescription(lpszDescription);  
      
    //��ʾ��ʽ  
    pLink->SetShowCmd(iShowCmd);  
  
  
    //��ݷ�ʽ��·�� + ����  

    //�����ݷ�ʽ��ָ��Ŀ¼��  
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
		{//���˷�������������
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
	++cur_sub_dir;//��Ŀ¼����+1
	wxString file;
	bool test = dir.GetFirst(&file,wxEmptyString,wxDIR_DIRS|wxDIR_HIDDEN);
	wxString path = dir.GetNameWithSep();
	while(test)
	{
		ListFiles(path+file,files,filespec,sub);
		test = dir.GetNext(&file);
	}
	--cur_sub_dir;//��Ŀ¼����-1
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

	//func������Ϊ�˼��ݾɰ������ļ����µİ汾�����ã�����ʹ��func����
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

//���ݺ�����λ����ȡ����ƴ������ĸ

//��λ������һ������ƴ������ĸ������
static int li_SecPosValue[] = {
		0xB0A1, 0xB0C5, 0xB2C1, 0xB4EE, 0xB6EA, 0xB7A2, 0xB8C1, 0xB9FE, 0xBBF7, 0xBFA6, 0xC0AC,
		0xC2E8, 0xC4C3, 0xC5B6, 0xC5BE, 0xC6DA, 0xC8BB, 0xC8F6, 0xCBFA, 0xCDDA, 0xCEF4, 0xD1B9, 0xD4D1
	};
static char* lc_FirstLetter = "ABCDEFGHJKLMNOPQRSTWXYZ";
//�ڶ������޹�������ֻ��ͨ������ķ�ʽ��ȡ
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
		L = ((i+1)< len)?name[i+1]:0;//��ֹ����Խ��.
		//���ִ�16��(A1)��ʼ��87��(F7)
		//һ����94��,λ�� A1 - FE
		if (H < 0xB0 ||  L < 0xA1 || H > 0xF7 || L == 0xFF)
		{
			pinyin.Append(H);
			continue;
		}

		if (H < 0xD8)//(H >= 0xB0 && H <=0xD7)//��ѯ������һ��������(16-55)
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
		else// if (H >= 0xD8 && H <= 0xF7)//��ѯ�����ڶ���������(56-87)
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

	if (cmdName.find("://",3,3) !=  wxNOT_FOUND //��ַ����
		 || cmdName.StartsWith("::") //:: ����ϵͳ���ܵ���
		 || cmdName.StartsWith("\\\\")//�����ַ��ϵͳ���ܵ���
		)
	{
		return true;
	}

	return false;
}
//�򵥵ķ�������Ͳ�������
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
	if (n >= cmd_len)//�����п���Ϊ��
		return cmdLine;

	wxString cmd_flag = cmdLine.Left(n);
	size_t cmd_pos = n;
	if (cmdLine[n] == '\"')//������
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

	//����ļ����ڷ����ļ�·��
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

	if (fn.GetPath().empty())//��·����Ϣ�����ȳ��Զ�ȡע���App Pathsλ��
	{
		wxString regPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + cmdName;
		if (!hasext)//����չ��������.exe
			regPath.Append(".exe");
		wxRegKey reg;
		wxRegKey::StdKey hk[2] = {wxRegKey::HKCU,wxRegKey::HKLM};//��HKCU��HKLM�в���
		for(int i=0;i<2;++i)
		{
			reg.SetName(hk[i],regPath);
			if (!reg.Exists())
				continue;
			regPath = reg.QueryDefaultValue();
			if (regPath.empty())
				continue;
			if (wxFileExists(regPath))//���ҵ���ֱ�ӷ���
				return regPath;
		}
	}

	wxArrayString mcwd;//����PATH·����ѯ�ļ�
	wxArrayString wExt;//�ļ���չ��

	if (fn.IsRelative() && !fn.IsPathSeparator(cmdName[0]))//���·����PATH�͵�ǰĿ¼�в�ѯ
		mcwd = ::wxSplit(wxString::Format("%s;%s",wxGetCwd(),wxGetenv("PATH")),';','\0');
	else//�Ǹ�����·�����·��
		mcwd.Add("");

	if (!hasext)//����չ�����ҷ���PATHEXT�����趨��չ���ļ�
	{
		cmdName.Append(".*");//ʹ��.*�������У��ٸ���PATHEXT����չ���ж�
		wExt = ::wxSplit(wxGetenv("PATHEXT"),';','\0');
	}

	size_t mc_size = mcwd.size();

	for(size_t i = 0;i< mc_size;++i)//ѭ����������·���µ��ļ�
	{
		wxString strPath = mcwd[i];
		if (!strPath.empty())
		{
			if (!::wxDirExists(mcwd[i]))//·������򲻴��ڲ�����
				continue;
			if(!::wxEndsWithPathSeparator(strPath))//·��������"\\"��β�Զ�����
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
			{//��Ŀ¼
				wxString file(findFileData.cFileName);
				size_t pos = file.find_last_of('.');
				//���ҵ��ļ�����չ��˵���Ѿ��ҵ���
				//��û����չ���������ҵ����ļ�Ҳû����չ����Ҳ�Ƿ���
				//������ҷ���PATHEXT����չ��
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
	if (cmd.empty())//û���ҵ�����
	{
		winexec = true;
		cmd = cmdLine;
	}

	//�滻{%c}Ϊ����������
	if (cmd.find("{%c}") != wxNOT_FOUND)
		cmd.Replace("{%c}",GetClipboardText());

	//�滻{%wt}Ϊ�������
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
#include "ALMRunCommon.h"
#include "ALMRunConfig.h"
#include "MerryCommand.h"
#include "wx/dir.h"
#include <wx/clipbrd.h>
#include "MerryController.h"

#ifdef __WXMSW__
	#include <wx/msw/registry.h>
	#include "TlHelp32.h"
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

BOOL ReadShortcut(LPCWSTR lpwLnkFile, ALMRunCMDBase *cmd)
{
	IShellLink     *pLink;
	IPersistFile   *ppf;
	HRESULT hr = CoInitialize(NULL) == S_OK;
	WCHAR temp[MAX_PATH * 5];

	if (FAILED(hr))
		return FALSE;

	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,IID_IShellLink, (void **)&pLink) >= 0;
	if (FAILED(hr))
        return FALSE;

	if (FAILED(pLink-> QueryInterface(IID_IPersistFile, (void **)&ppf)))
	{
		pLink->Release();
		return FALSE;
	}

	if (FAILED(ppf->Load(lpwLnkFile,true)))
		return FALSE;

	if (SUCCEEDED(pLink->GetPath(temp,MAX_PATH,NULL,0)))
		cmd->cmdLine = temp;
	if (SUCCEEDED(pLink->GetArguments(temp,MAX_PATH * 5)))
	{
		cmd->cmdLine.Append(" ");
		cmd->cmdLine.Append(temp);
	}
	if (SUCCEEDED(pLink->GetDescription(temp,MAX_PATH*5)))
		cmd->Desc = temp;
	if (SUCCEEDED(pLink->GetWorkingDirectory(temp,MAX_PATH)))
		cmd->WorkDir = temp;
	ppf->Release();
	pLink->Release();
	return TRUE;
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

#ifdef __WXMSW__
static int x64sys = -1;
BOOL IsX64()
{
	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	BOOL bX64 = FALSE;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	if (x64sys != -1) return x64sys;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if(NULL != pGNSI)
	   pGNSI(&si);
	else
	   GetSystemInfo(&si);
	if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
	   bX64 = true;
	x64sys = bX64;
	return bX64;
}

BOOL ActiveWindow(HWND hwnd)
{
	if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
	return (BringWindowToTop(hwnd) && SetForegroundWindow(hwnd));
}

BOOL CheckActiveProg(DWORD PID)
{
	PROCESSENTRY32 ProInfo;
	ProInfo.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProSnap;
	HWND h = GetTopWindow(0);
	while (h)
	{
		DWORD pid = 0;
		DWORD dwTheardId = GetWindowThreadProcessId( h,&pid);
		if (dwTheardId != 0)
		{
			if ( pid == PID && GetParent(h) == NULL && wxGetWindowText(h).length())
			{
				if (ActiveWindow(h)) return TRUE;
			}
		}
		h = GetNextWindow( h , GW_HWNDNEXT);
	}

	hProSnap=CreateToolhelp32Snapshot(TH32CS_SNAPALL,0);
	if (::Process32First(hProSnap,&ProInfo))
	{
		for (;::Process32Next(hProSnap,&ProInfo);)
		{
			if (ProInfo.th32ProcessID == PID)
			{
				wxMessageBox("�����Ѿ�������!","����ʧ��!");
				return TRUE;
			}
		}
	}
	return FALSE;
}

#endif

void setWinHelpText(wxWindowBase* win,const wxString& text,bool ShowToolTips)
{
	win->SetHelpText(text);
	if (ShowToolTips)
		win->SetToolTip(text);
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

	lua_pushstring(L, "workdir");
	lua_rawget(L, it);
	cmd->WorkDir = wxString(lua_tostring(L, -1), wxConvLocal);

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

//���ݺ�����λ���ȡ����ƴ������ĸ

//��λ����һ������ƴ������ĸ������
static int li_SecPosValue[] = {
		0xB0A1, 0xB0C5, 0xB2C1, 0xB4EE, 0xB6EA, 0xB7A2, 0xB8C1, 0xB9FE, 0xBBF7, 0xBFA6, 0xC0AC,
		0xC2E8, 0xC4C3, 0xC5B6, 0xC5BE, 0xC6DA, 0xC8BB, 0xC8F6, 0xCBFA, 0xCDDA, 0xCEF4, 0xD1B9, 0xD4D1
	};
static char* lc_FirstLetter = "ABCDEFGHJKLMNOPQRSTWXYZ";
//�ڶ������޹�������ֻ��ͨ�����ķ�ʽ��ȡ
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
			if(H < 0x80)
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

wxString GetCMDPath(const wxString& commandLine,const wxString& workingDir)
{
	wxString cwd = ::wxGetCwd();
	wxString cmdPath;
	if (!workingDir.empty() && ::wxDirExists(workingDir))
		::wxSetWorkingDirectory(workingDir);
	if (commandLine.Len() >= MAX_PATH)
		return "";
	cmdPath = _GetCMDPath(commandLine);
	wxSetWorkingDirectory(cwd);
	return cmdPath;
}


// ----------------------------------------------------------------------------
// filter functions
// ----------------------------------------------------------------------------

// undo FilterOutValue
wxString UnEscapeString(const wxString& str)
{
    wxString strResult;
    if ( str.empty() )
        return strResult;

    strResult.reserve(str.length());

    wxString::const_iterator i = str.begin();
    const bool bQuoted = *i == '"';
    if ( bQuoted )
        ++i;

    for ( const wxString::const_iterator end = str.end(); i != end; ++i )
    {
        if ( *i == wxT('\\') )
        {
            if ( ++i == end )
            {
                wxLogWarning(_("trailing backslash ignored in '%s'"), str.c_str());
                break;
            }

            switch ( (*i).GetValue() )
            {
                case wxT('n'):
                    strResult += wxT('\n');
                    break;

                case wxT('r'):
                    strResult += wxT('\r');
                    break;

                case wxT('t'):
                    strResult += wxT('\t');
                    break;

                case wxT('\\'):
                    strResult += wxT('\\');
                    break;

                case wxT('"'):
                    strResult += wxT('"');
                    break;
            }
        }
        else // not a backslash
        {
            if ( *i != wxT('"') || !bQuoted )
            {
                strResult += *i;
            }
            else if ( i != end - 1 )
            {
                wxLogWarning(_("unexpected \" at position %d in '%s'."),
                             i - str.begin(), str.c_str());
            }
            //else: it's the last quote of a quoted string, ok
        }
    }

    return strResult;
}

// quote the string before writing it to file
wxString EscapeString(const wxString& str)
{
   if ( !str )
      return str;

  wxString strResult;
  strResult.Alloc(str.Len());

  // quoting is necessary to preserve spaces in the beginning of the string
  bool bQuote = wxIsspace(str[0]) || str[0] == wxT('"');

  if ( bQuote )
    strResult += wxT('"');

  wxChar c;
  for ( size_t n = 0; n < str.Len(); n++ ) {
    switch ( str[n].GetValue() ) {
      case wxT('\n'):
        c = wxT('n');
        break;

      case wxT('\r'):
        c = wxT('r');
        break;

      case wxT('\t'):
        c = wxT('t');
        break;

      case wxT('\\'):
        c = wxT('\\');
        break;

      case wxT('"'):
        if ( bQuote ) {
          c = wxT('"');
          break;
        }
        //else: fall through

      default:
        strResult += str[n];
        continue;   // nothing special to do
    }

    // we get here only for special characters
    strResult << wxT('\\') << c;
  }

  if ( bQuote )
    strResult += wxT('"');

  return strResult;
}


wxString wxURL_GET(const wxString &uri,const wxString& proxy)
{
	wxURL u(uri);
	wxURLError err = u.GetError();
	if (err != wxURL_NOERR)
		return wxString::Format("Error: %d",err);
	if (!proxy.empty())
		u.SetProxy(proxy);
	wxInputStream *in   = u.GetInputStream();
	wxString Response;
    wxStringOutputStream out(&Response);
    if(in && in->IsOk())
        in->Read(out);
    delete in;
	return Response;
}

#ifdef __WXMSW__

BOOL chkSysCmd(const wxString& cmdLine,size_t * const cmdStart = NULL)
{
	wxString cmdFlags(CMDFLAG_STRS);
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

	if (cmdName.substr(0,8).find("://") != wxNOT_FOUND //��ַ����
		 || cmdName.StartsWith("::") //:: ����ϵͳ���ܵ���
		 || cmdName.StartsWith("\\\\")//�����ַ��ϵͳ���ܵ���
		)
	{
		return true;
	}

	return false;
}
//�򵥵ķ�������Ͳ�������
wxString ParseCmd(const wxString& cmdLine,wxString* const cmdArg,const wxString& workDir)
{
	size_t cmd_len = cmdLine.Len();
	wxString cmdFlags(CMDFLAG_STRS);
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
		for(++n;n<cmd_len;)
		{
			if (cmdLine[n++] == '\"')
				break;
		}
		cmd = GetCMDPath(cmdLine.substr(cmd_pos,n-cmd_pos),workDir);

		goto getParam;
	}
	else
	{
		wxString tmp = cmdLine.substr(n);
		size_t k = -1;
		if (tmp.StartsWith("::"))//��"::"��ͷ��ϵͳ���ܵ���
		{
			k = tmp.find(" ");
			cmd = tmp.substr(0,k);
		}
		else if (tmp.substr(0,8).find("://") !=  wxNOT_FOUND)//��ַ����
			cmd = tmp;
		else
			cmd = GetCMDPath(tmp,workDir);

		if (cmd.Len())
		{
			n = k;
			goto getParam;
		}
	}
	size_t lastSpace = 0;
	bool InSpace = false;
	for(;n<cmd_len;++n)
	{
		wxChar c = cmdLine[n];
		if (c == ' ')
		{
			if (InSpace)
				continue;
			lastSpace = n;
			wxString _tmp = GetCMDPath(cmdLine.substr(cmd_pos,n-cmd_pos),workDir);
			if (_tmp.Len())
			{
				cmd = _tmp;
				break;
			}
		}
		else if (c == '/' && InSpace)//�ո�֮��� '/'��Ϊ�ǲ���
		{
			cmd = cmdLine.substr(cmd_pos,lastSpace-cmd_pos);
			break;
		}
		else if (c == ':' && lastSpace)//�ո�֮���ٳ����̷�,һ����Ϊ�ո����������ǲ���
		{
			n = lastSpace;
			cmd = cmdLine.substr(cmd_pos,n-cmd_pos);
			break;
		}
		InSpace = c == ' ';
	}

getParam:
	if (cmdArg && n < cmd_len)
	{
		for(;n<cmd_len;++n)
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

	if (cmd.empty())
		return cmd;
	return cmd_flag + cmd;
}

static wxString _GetCMDPath(const wxString& commandLine)
{
	size_t cmdIndex;
	if (commandLine.empty() || chkSysCmd(commandLine,&cmdIndex))
		return commandLine;

	wxString cmdName =::wxExpandEnvVars(commandLine.substr(cmdIndex));
	wxFileName fn = wxFileName(cmdName);

	if (fn.HasVolume() && fn.GetVolume().Len() > 1)
		return wxEmptyString;
	if (fn.IsDir() && !::wxDirExists(cmdName))
		return wxEmptyString;
	//����ļ����ڷ����ļ�·��
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
		if (!hasext)//����չ�������.exe
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
			if(!::wxEndsWithPathSeparator(strPath))//·��������"\\"��β�Զ����
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

DWORD RunCMD(const wxString& cmdLine,const wxString& cmdArg,const wxString& workDir)
{
	wxString arg;
	wxString argt = wxEmptyString;
	wxString cmd = cmdLine;
	wxString workdir = workDir;
	bool winexec = false;

	//�滻{%c}Ϊ����������
	if (cmd.find("{%c}") != wxNOT_FOUND)
		cmd.Replace("{%c}",GetClipboardText());

	//�滻{%wt}Ϊ�������
	if (cmd.find("{%wt}") != wxNOT_FOUND)
		cmd.Replace("{%wt}",g_controller->GetWindowText(g_controller->GetForegroundWindow()));

	if (cmd.find("{%wd}") != wxNOT_FOUND)
		cmd.Replace("{%wd}",wxString::Format("%d",g_controller->GetForegroundWindow()));

	if (cmd.find("{%p}") != wxNOT_FOUND)
		cmd.Replace("{%p}",cmdArg);
	else if (cmd.find("{%p+}") != wxNOT_FOUND)
		cmd.Replace("{%p+}",cmdArg);
	else
		argt = wxT(' ') + cmdArg;

	cmd = ParseCmd(cmd,&arg,workDir);
	if (cmd.empty())//û���ҵ�����
	{
		winexec = true;
		cmd = cmdLine;
	}
	if (workdir.empty())
		workdir = wxPathOnly(cmd);

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

	return g_controller->ShellExecute(cmd,arg,workdir);
}

static int AddALTRunCMD(lua_State *L)
{
	if (!lua_istable(L, -1))
		return 0;
	int it = lua_gettop(L);

	lua_rawgeti(L,it,3);//�����
	wxString commandName(wxString(lua_tostring(L, -1), wxConvLocal));

	lua_rawgeti(L,it,4);//��ע
	wxString commandDesc(wxString(lua_tostring(L, -1), wxConvLocal));

	lua_rawgeti(L,it,5);//����
	wxString commandLine(wxString(lua_tostring(L, -1), wxConvLocal));

	lua_rawgeti(L,it,2);//��������
	wxString commandParam(wxString(lua_tostring(L, -1), wxConvLocal));

	lua_settop(L,it);//�ָ�

	if (!commandParam.Trim(false).empty())//�����Ҫ����������ǿ�Ʋ�����־
		commandLine.insert(0,'+');
	if (g_config->AddCmd(commandLine,commandName,wxEmptyString,commandDesc) > 0)
		return 1;
	ShowErrinfo(ShowCMDErrInfo);
	return 0;
}
/*
	����뺯��,Ŀǰֻ֧��ALTRUN�����ļ�,�ļ���SHORTCUTLIST.txt
*/
int importCMD(wxString& filename)
{
	if (!g_lua || !g_config)
		return -1;
	if (wxFileNameFromPath(filename).Upper().StartsWith("SHORTCUTLIST.") == false || 
		wxMessageBox("���ļ�������ALTRun�������ļ����Ƿ�������?","ALTRun�������ʾ",wxYES_NO|wxICON_INFORMATION) != wxYES)
		return -1;
	lua_State *L = g_lua->GetLua();

	if (!g_lua->get_func(LUA_ReadALTRunConfig_FUNC))
	{
		wxMessageBox("�ù�������Ҫ��LUA���������ڣ���ȷ��LuaEx\\base.lua�ļ�����","����");
		return 0;
	}

	lua_pushstring(L,filename.c_str());
	if (lua_pcall(L, 1, 1, 0) || !lua_istable(L,-1))
	{
		wxMessageBox("LUA�ű�ִ�����󣬻򷵻�ֵ����");
		lua_pop(L, 1);
		return 0;
	}
	int it=lua_gettop(L);
	int success = 0;
	int failure = 0;
	lua_pushnil(L);
	while(lua_next(L, it))
	{
		if (AddALTRunCMD(L))
			++success;
		else
			++failure;
		lua_pop(L, 1);
	}
	lua_pop(L,1);
	wxMessageBox(wxString::Format("�ɹ�����%d������,ʧ��%d��!",success,failure));
	return success;
}

#endif
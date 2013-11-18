#include "ALMRunCommon.h"
#include "MerryCommand.h"
#include "wx/dir.h"

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
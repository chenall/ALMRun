#include "MerryCommand.h"
#include "MerryCommandManager.h"
#include "MerryLua.h"
#include "MerryInformationDialog.h"
#include "DlgParam.h"
#include "ALMRunConfig.h"
//
const MerryCommand* LastCmd;

 MerryCommand::MerryCommand(int commandID, const wxString& commandName, const wxString& commandDesc,const wxString& commandLine, int funcRef, const wxString& triggerKey,const int order)
{
	m_commandID = commandID & 0xFFFF;
	m_flags = commandID >> 16;
	m_commandName = commandName;
	m_commandFunc = funcRef;
	m_triggerKey = triggerKey;
	m_commandDesc = commandDesc;
	m_commandLine = commandLine;
	m_commandFName = commandName;
	m_order = order;

	if (commandName.empty() || m_flags == -1)
		return;
	static int li_SecPosValue[] = {
		0xB0A1, 0xB0C5, 0xB2C1, 0xB4EE, 0xB6EA, 0xB7A2, 0xB8C1, 0xB9FE, 0xBBF7, 0xBFA6, 0xC0AC,
		0xC2E8, 0xC4C3, 0xC5B6, 0xC5BE, 0xC6DA, 0xC8BB, 0xC8F6, 0xCBFA, 0xCDDA, 0xCEF4, 0xD1B9, 0xD4D1
	 };
	static char* lc_FirstLetter = "ABCDEFGHJKLMNOPQRSTWXYZ";
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
	int W;
	UCHAR H,L;
	UINT i, len;
	std::string name = commandName.ToStdString();
	len = name.length();
	m_commandFName.Append('\n');
	for (i = 0; i < len; ++i)
	{
		H = name[i];
		L = ((i+1)< len)?name[i+1]:0;//防止访问越界.
		//汉字从16区(A1)开始到87区(F7)
		//一个区94个,位码 A1 - FE
		if (H < 0xB0 ||  L < 0xA1 || H > 0xF7 || L == 0xFF)
		{
			m_commandFName.Append(H);
			continue;
		}

		if (H < 0xD8)//(H >= 0xB0 && H <=0xD7)//查询文字在一级汉字区(16-55)
		{
			W = (H << 8) | L;
			for (int j = 22; j >= 0; --j)
			{
				if (W >= li_SecPosValue[j])
				{
					m_commandFName.Append(lc_FirstLetter[j]);
					break;
				}
			}
		}
		else// if (H >= 0xD8 && H <= 0xF7)//查询中文在二级汉字区(56-87)
			m_commandFName.Append(ls_SecondSecTable[H-0xD8][L-0xA1]);
		++i;
	}
	#ifdef _ALMRUN_CONFIG_H_
	//获取排序值信息
	if (g_config && g_config->order_conf)
		m_order = g_config->order_conf->ReadLong(m_commandName,0);
	#endif
}

MerryCommand::~MerryCommand()
{
	if (!g_lua)
		return;

	lua_State* L = g_lua->GetLua();
	assert(L);
	luaL_unref(L, LUA_REGISTRYINDEX, m_commandFunc);
}

wxString MerryCommand::GetDetails() const
{
	wxString cmd_from;
	if (m_flags & CMDS_FLAG_DIRS)
	{
		cmd_from = wxT("目录扫描(ALMRUN.INI)");
	}
	else if (m_flags & CMDS_FLAG_ALTRUN)
	{
		cmd_from = wxT("ALTRun配置文件");
	}
	else if (m_flags & CMDS_FLAG_LUA)
	{
		cmd_from = wxT("LUA脚本扩展");
	}
	else if (m_flags & CMDS_FLAG_CMDS)
	{
		cmd_from = wxT("ALMRun命令(ALMRUN.INI)");
	}
	else
	{
		cmd_from = wxT("未知配置");
	}
	return wxString::Format(wxT("配置文件：%s\nID:[%d] %s\n命令:%s\n热键: %s\n"),cmd_from,m_flags>>4,this->m_commandDesc,this->m_commandLine,this->m_triggerKey);
}

wxString MerryCommand::GetCmd() const
{
	if (!m_commandLine.empty())
		return m_commandLine;
	if (m_commandFunc)
		return wxT("执行LUA命令");
	return wxEmptyString;
}

int MerryCommand::SetOrder()
{
	static time_t chgtime;
#ifdef _ALMRUN_CONFIG_H_
	if (g_config && !m_commandName.empty())
		return (m_order = g_config->SetcmdOrder(m_commandName));
#endif
	return ++m_order;
}

int MerryCommand::GetOrder() const
{
	return m_order;
}

void MerryCommand::Execute(const wxString& commandArg) const
{

	if (!g_lua)
		return;

	lua_State* L = g_lua->GetLua();
	wxString cmdArg = commandArg;
	assert(L);
	if (cmdArg.empty() && (m_commandLine[0] == '+' || m_commandLine.Find("{%p+}") != wxNOT_FOUND))
	{
		DlgParam *dlg = new DlgParam(NULL,-1,m_commandName);
		if (dlg->ShowModal() == wxID_OK)
			cmdArg = dlg->getvalue();
		dlg->Destroy();
		dlg = NULL;
		if (cmdArg.empty())
			return;
	}

	if (m_commandFunc == 0)
	{
		lua_getglobal(L, "CmdCallFunc");
		if (lua_isnil(L, 1))
		{
			lua_pop(L, 1);
			return;
		}
		lua_pushstring(L, m_commandLine.c_str());
		lua_pushstring(L, cmdArg.c_str());
	}
	else if (m_commandFunc == -2)
	{
		if (m_commandLine == "LastCmd" && LastCmd)
			LastCmd->Execute(wxEmptyString);
		return;

	}
	else
	{
		if (m_commandFunc == -1)
			lua_getglobal(L, m_commandLine.c_str());
		else
			lua_rawgeti(L, LUA_REGISTRYINDEX, m_commandFunc);
		assert(lua_isfunction(L, -1));
		lua_pushstring(L, cmdArg.c_str());
		lua_pushnumber(L, m_commandID);
	}

	lua_pushnumber(L,0);

	if (lua_pcall(L, 3, 0, 0))
	{
		new MerryInformationDialog(
			wxString::Format(wxT("Command %s execute failed"), m_commandName),
			wxString(lua_tostring(L, -1), wxConvLocal)
		);
		lua_pop(L, 1);
	}
	if (m_commandName.empty() || m_flags == -1)
		return;

	const_cast<MerryCommand*>(this)->SetOrder();
	if (m_commandFunc != -1)
		LastCmd = this;
}

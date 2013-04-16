local eventTable = {}
function addEventHandler(eventName, eventHandler)
	if not eventTable[eventName] then
		eventTable[eventName] = {}
	end

	local events = eventTable[eventName] 
	table.insert(events, eventHandler)

	if not _G[eventName] then
		_G[eventName] = function(...)
			for _, _eventHandler in ipairs(events) do
				_eventHandler(...)
			end
		end
	end
end

function enterString(str)
	local t = {}
	str:gsub('.', function(c)
		table.insert(t, c)
	end)
	enterKey(table.unpack(t))
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function get_name(file)
	return file:match("[^\\]+$")
end

-- 自动扫描指定目录下指定扩展名程序到列表中
function scan_dir(path,ext,sub)
	if ext == nil then
		ext = "*.exe"
	end
	if sub == nil then
		sub = -1
	end
	path = path:gsub("%%(%S+)%%",os.getenv)
	local Files = ListDir(path,ext,sub)
	if Files == nil then 
		return
	end
	for key,value in pairs(Files) do  
		m_name = get_name(value)
		addCommand{ name = m_name,cmd = value}
	end
end

-- Events
local histroy = io.open(ALMRUN_CONFIG_PATH .. 'histroy.lua', 'a')
addEventHandler('onUndefinedCommand', function(commandName, commandArg)
	local commandNameArray = { commandName }
	if MAC then
		table.insert(commandNameArray, "/Applications/" .. commandName .. ".app")
		table.insert(commandNameArray, "/Applications/Utilities/" .. commandName .. ".app")
	end

	for _, commandNameFull in ipairs(commandNameArray) do
		if shellExecute(commandNameFull, commandArg) then
			addCommand{ name = commandName, func = function() shellExecute(commandNameFull) end }
			histroy:write(string.format("addCommand{ name = [[%s]], func = function() shellExecute([[%s]]) end }\n",
				commandName, commandNameFull))
			histroy:flush()
			break
		end
	end
end)

addEventHandler('onClose', function()
	histroy:close()
-- 保存命令优先级设置开始
	local f = io.open(MERRY_ORDER_FILE,'w+')
	for i,v in pairs(CmdOrder) do
		f:write(v..'|'..i.."\n")
	end
	f:close()
-- 保存命令优先级设置完成
	showWindow(curHideWindow, 'normal')
end)

-- 默认的命令调用函数
CmdCallFunc = function(cmdLine,cmdArg,Flags)
-- 命令行以'@'开头,隐藏窗口执行
	local show = 'normal'
	local DestDir = ''
	if cmdLine:sub(1,1) == '@' then
		cmdLine = cmdLine:sub(2)
		show = 'hide'
	end

	cmdLine = cmdLine:gsub("%%(%S+)%%",os.getenv) --系统环境变量扩展

	local pos = cmdLine:find(":::") -- 正常使用:::分隔参数
	if pos == nil then
		T_arg = cmdLine:match("%s+[%-|/]%a") --如果命令行中出现" -"或" /",也认为是参数部份,要分隔开来.
		if T_arg == nil then--按文件名分隔参数程序需要带有扩展名,否则会运行错误
			cmd = cmdLine:gsub("(.*%.[^%s]+).*", "%1")
			arg = cmdLine:gsub("(.*%.[^%s]+)(.*)$", "%2")
		else
			pos = cmdLine:find(T_arg,1,true)
			cmd = cmdLine:sub(1,pos-1)
			arg = cmdLine:sub(pos+1)
		end
	else
		cmd = cmdLine:sub(1,pos-1)
		arg = cmdLine:sub(pos+3)
	end
	if not (arg == "") then
		cmdArg = arg..' '..cmdArg
	end
-- 提取可执行程序所在目录
	if cmd:match("^%a:") then
		local FileName = cmd:match("[^\\]+$")
		DestDir = cmd:sub(1,-FileName:len()-1)
	end
	if (Flags%2) == 1 then
		shellExecute("explorer.exe","/n,/select,"..cmd,'','')
	else
		shellExecute(cmd,cmdArg,DestDir,show)
	end
end

--本函数在每添加一个命令之后调用,获取对应命令的优先级,返回数值越大优先级越高.
function GetCmdOrder(cmdName)
	local order = CmdOrder[cmdName]
	if order then
		return order
	else
		return 0
	end
end

--本函数在每执行一条命令之后调用,返回数值越大优先级越高.
function SetCmdOrder(cmdName,order)
	order = order + 1
	CmdOrder[cmdName] = order
	return order
end

function read_csv (file,delims)
  local fp = assert(io.open (file))
  local csv = {}
  for line in fp:lines() do
    local row = {}
    for value in line:gmatch("[^"..delims.."]+") do
      row[#row+1] = trim(value)
    end
    if not (row[1] == nil or row[1] == "") then
		csv[#csv+1] = row
    end
  end
	fp:close()
  return csv
end

-- 偷懒,直接调用ALTRUN的配置,^_^
function altrun_config(file)
	local altrun = read_csv(file,"|")
	for i=1,#altrun do
		addCommand{ name = altrun[i][3], desc = altrun[i][4],cmd = altrun[i][5]}
	end
end

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
-- 过滤字符串首尾空格函数
function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-- 获取文件名函数
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
addEventHandler('onUndefinedCommand', function(commandName, commandArg)
	local commandNameArray = { commandName }
	if MAC then
		table.insert(commandNameArray, "/Applications/" .. commandName .. ".app")
		table.insert(commandNameArray, "/Applications/Utilities/" .. commandName .. ".app")
	end
	local f = io.open(ALMRUN_CONFIG_PATH .. 'histroy.lua', 'a')
	for _, commandNameFull in ipairs(commandNameArray) do
		if shellExecute(commandNameFull, commandArg) then
			addCommand{ name = commandName, func = function() shellExecute(commandNameFull) end }
			f:write(string.format("addCommand{ name = [[%s]], desc='未定义命令,在histroy.lua文件中',func = function() shellExecute([[%s]]) end }\n",
				commandName, commandNameFull))
			break
		end
	end
	f:close()
end)
addEventHandler('onClose', function()
-- 保存命令优先级设置开始
	local f = io.open(ALMRUN_ORDER_FILE,'w+')
	for i,v in pairs(CmdOrder) do
		f:write(v..'|'..i.."\n")
	end
	f:close()
-- 保存命令优先级设置完成
    if curHideWindow then
	showWindow(curHideWindow, 'normal')
    end
end)
function SplitCmd(cmdLine)
   cmdLine = trim(cmdLine)
   local pos = cmdLine:find(":::") -- 正常使用:::分隔参数
   local cmd,arg
    if pos then
	cmd = cmdLine:sub(1,pos-1)
	arg = cmdLine:sub(pos+3)
	return cmd,arg
    end
    --带引号的文件名
    cmd = cmdLine:match("^\"(.-)\"")
    if cmd then
	arg = cmdLine:match("^\".-\"%s*(.*)")
	return cmd,arg
    end
    --如果命令行中出现" -"或" /",也认为是参数部份,要分隔开来.
    cmd = cmdLine:match("^(.-)%s+[%-|/]%a")
    if cmd then
	arg = cmdLine:match("%s+[%-|/]%a.*")
	return cmd,arg
    end
    cmd = cmdLine:match(".-%.[^%s]+")
    if cmd then
	arg = cmdLine:sub(cmd:len()+1)
	return cmd,arg
    end
    return cmdLine
end
-- 默认的命令调用函数
CmdCallFunc = function(cmdLine,cmdArg)
    -- 自动参数替换{%p}
    if cmdLine:match("{%%[pP]%+?}") then
	cmdLine = cmdLine:gsub("{%%[pP]%+?}",cmdArg)
	cmdArg = nil
    end
    -- 替换{%c}为剪贴板内容
    if cmdLine:match("{%%c}") then
	cmdLine = cmdLine:gsub("{%%c}",GetClipboardData())
    end
    -- 替换{%wt}为窗体标题
    if cmdLine:match("{%%wt}") then
	cmdLine = cmdLine:gsub("{%%wt}",GetWindowText(getForegroundWindow()))
    end
    -- 以上{%p},{%c},{%wt}是为了兼容ALTRUN参数设置
    cmdLine = cmdLine:gsub("%%(%S+)%%",os.getenv) --系统环境变量扩展
    local cmd,arg = SplitCmd(cmdLine)
    if arg then
	if cmdArg then
	    cmdArg = arg..' '..cmdArg
	else
	    cmdArg = arg
	end
    end
-- 提取可执行程序所在目录
    local DestDir = cmd:match("^(.-)[^\\]+$")
    shellExecute(cmd,cmdArg,DestDir)
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
function read_altrun_config (file)
  local fp = assert(io.open (file))
  local csv = {}
  for line in fp:lines() do
    local row = {}
    for value in line:gmatch("[^|]+") do
      row[#row+1] = trim(value)
    end
    if not (row[1] == nil or row[1] == "") then
	row[5] = line:gsub("[^|]+|","",4)
	csv[#csv+1] = row
    end
  end
	fp:close()
  return csv
end
-- 偷懒,直接调用ALTRUN的配置,^_^
function altrun_config(file)
    local FileName = file:match("[^\\]+$")
    local DestDir = file:sub(1,-FileName:len()-1)
    if not FileExists(file) then--文件不存在
	return
    end
    local altrun = read_altrun_config(file)
    for i=1,#altrun do
	if altrun[i][5]:sub(1,1) == "." then
	    altrun[i][5] = DestDir .. altrun[i][5]
	end
	if altrun[i][5]:sub(1,2) == "@." then
	    altrun[i][5] = "@" .. DestDir .. altrun[i][5]:sub(2)
	end
	addCommand{ name = altrun[i][3],desc=altrun[i][4],cmd = altrun[i][5]}
    end
end

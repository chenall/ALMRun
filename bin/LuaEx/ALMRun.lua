ALMRUN_CONFIG_PATH = ALMRUN_HOME .. "config\\"
ALMRUN_ORDER_FILE = ALMRUN_CONFIG_PATH .. 'order.txt'

--LUA错误捕获函数
function error_hook(msg)
    message(msg:gsub("^(.+):(%d+):(.+)$","文件：%1\n错误行 [%2] 以后的内容将被忽略\n%3"),"Configure failed")
end

--使用luajit的xpcall来调用可以防此由于意外错误导致程序崩溃
function DoFile(file)
    xpcall(dofile,error_hook,file)
end

DoFile(ALMRUN_HOME .. 'LuaEx/base.lua')--基本库

--读取用户扩展库
local Files = ListDir(ALMRUN_CONFIG_PATH,"*.lua",-1)
if Files == nil then 
    return
end
for key,value in pairs(Files) do  
    DoFile(value)
end

CmdOrder = {}--命令排序表
--[[
	order.txt 内容格式  排序号|命令名
	例子:
	1|radmin
	100|airview
	注:你也可以使用自己喜欢的格式,需要修改读取和保存的代码
		读取的代码在下面,保存的代码在base.lua的onClose函数里面.
]]

--读取命令排序表数据后面添加命令时要用到
local f = io.open(ALMRUN_ORDER_FILE,'r')
if f then
	for line in f:lines() do
		local row = {}
		for value in line:gmatch("[^|]+") do
			row[#row+1] = trim(value)
		end
		if not (row[1] == nil or row[1] == "") then
			CmdOrder[row[2]]=row[1]
		end
	end
	f:close()
end


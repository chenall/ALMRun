ALMRUN_CONFIG_PATH = GetEnv('ALMRUN_ROOT') .. "config\\"
MERRY_ORDER_FILE = ALMRUN_CONFIG_PATH .. 'order.lua'
CmdOrder = {}--命令排序表
--[[
	order.lua 内容格式  排序号|命令名
	例子:
	1|radmin
	100|airview
	注:你也可以使用自己喜欢的格式,需要修改读取和保存的代码
		读取的代码在下面,保存的代码在base.lua的onClose函数里面.
]]
dofile(ALMRUN_CONFIG_PATH .. 'base.lua')--基本库
--读取命令排序表数据后面添加命令时要用到
local f = io.open(MERRY_ORDER_FILE,'r')
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

dofile(ALMRUN_CONFIG_PATH .. 'common.lua')
dofile(ALMRUN_CONFIG_PATH .. 'histroy.lua')
dofile(ALMRUN_CONFIG_PATH .. 'my_conf.lua')

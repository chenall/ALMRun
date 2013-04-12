MERRY_ROOT_PATH = 'config/'
MERRY_ORDER_FILE = MERRY_ROOT_PATH .. 'order.lua'
CmdOrder = {}
local f = io.open(MERRY_ORDER_FILE,'r')
if f then
	f:close()
	dofile(MERRY_ORDER_FILE)--命令优先级信息
end
dofile(MERRY_ROOT_PATH .. 'base.lua')
dofile(MERRY_ROOT_PATH .. 'common.lua')
dofile(MERRY_ROOT_PATH .. 'histroy.lua')
dofile(MERRY_ROOT_PATH .. 'my_conf.lua')


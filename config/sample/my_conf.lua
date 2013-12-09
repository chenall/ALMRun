-- 高级模式,使用LUA函数自定义命令
addCommand{ name = "test_func", desc = "命令函数测试",cmd = function(arg,id,flags)
	message('测试',"运行test命令,参数:"..arg..',命令ID:'..id)
end}

-- 普通模式,直接调用cmd参数指定的程序
addCommand{ name = "test_cmd", desc = "单独命令测试",cmd = [[cmd.exe /c pause >nul|echo 参数:]]}
addCommand{ name = "test_notepad", desc = "Notepad",cmd = [[%WinDir%\notepad.exe]]}
addCommand{ name = "EmptyRecycle", desc = "清空回收站",cmd = function () EmptyRecycleBin() end }
--[[
 扫描指定目录自动添加命令
 scan_dir(PATH,EXT,SUB)
 PATH  要扫描的目录
 EXT   条件比如*.exe(所有.exe的文件),k*.exe(所有以k开头的文件)
 SUB   扫描级别,默认(-1)
       -1 不限.
       0  不管子目录
       N  N级子目录
]]
--新版建议在ALMRun.ini中添加.速度会比较快.
--scan_dir([[E:\绿色软件]],"*.exe")

-- 自动添加程序菜单内容(新版改到ALMRun.ini中)
--scan_dir(GetShellFolder(2),"*.lnk")
-- 自动添加桌面快捷方式
--scan_dir(GetShellFolder(0),"*.lnk")

-- 调用altrun的配置
altrun_config("ShortCutList.txt")

-- 命令匹配模式(可以在ALMRUN.INI里面设置)
-- 1 匹配开头
-- 2 使用lua函数HookCompre自定义匹配
-- 0 默认,任意位置匹配
--config{CompareMode=0}
-- 命令匹配函数,可以自己扩展,当CompareMode为2时调用
--function HookCompare(commandName,Prefix)
--	return commandName:lower():find(Prefix:lower())
--end

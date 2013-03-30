-- 高级模式,使用LUA函数自定义命令
addCommand{ name = "test_func", desc = "命令函数测试",func = function(arg,id,flags)
	message('测试',"运行test命令,参数:"..arg..',命令ID:'..id,"aa")
end}

-- 普通模式,直接调用cmd参数指定的程序
addCommand{ name = "test_cmd", desc = "单独命令测试",cmd = "cmd.exe /c pause >nul|echo 参数:"}

-- 扫描指定目录自动添加
scan_dir("E:\\绿色软件","*.exe")

-- 调用altrun的配置
altrun_config("ShortCutList.txt")

-- 命令匹配模式
-- 1 匹配开头
-- 2 使用lua函数HookCompre自定义匹配
-- 0 默认,任意位置匹配
config{CompareMode=0}
-- 命令匹配函数,可以自己扩展,当CompareMode为2时调用
--function HookCompare(commandName,Prefix)
--	return commandName:lower():find(Prefix:lower())
--end
